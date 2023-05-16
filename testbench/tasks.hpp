/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for a Task                                        //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
//             Copyright (C) 2023 Roa Logic BV                     //
//             www.roalogic.com                                    //
//                                                                 //
//     This source file may be used and distributed without        //
//   restriction provided that this copyright statement is not     //
//   removed from the file and that any derivative work contains   //
//   the original copyright notice and the associated disclaimer.  //
//                                                                 //
//      THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY        //
//   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED     //
//   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     //
//   FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR        //
//   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,           //
//   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES      //
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE     //
//   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR          //
//   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    //
//   LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT    //
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT    //
//   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           //
//   POSSIBILITY OF SUCH DAMAGE.                                   //
//                                                                 //
/////////////////////////////////////////////////////////////////////
/*!
 * @file task.hpp
 * @author Richard Herveille
 * @brief Test object
 * @version 0.2
 * @date 16-may-2023
 * @copyright See beginning of file
 */

#ifndef TASKS_HPP
#define TASKS_HPP

#include <coroutine>
#include <cassert>

namespace RoaLogic
{
namespace testbench
{
namespace tasks
{
    /**
     * @brief Function headers, typedefs, and defines to simplify test creation
     */
    enum class coyieldEvent_t;
    template <typename T> struct coyieldReturn_t;
    template <typename T> struct sCoRoutineHandler;
    

    //type definition for clocked tests
    typedef coyieldReturn_t<cClock*> coyieldReturnClock_t;
    typedef sCoRoutineHandler<coyieldReturnClock_t> clockedTask_t;


    //Macros for clockedTest
    #define waitPosedge(clk) (co_yield (coyieldReturnClock_t){clk,coyieldEvent_t::clockPosedge})
    #define waitNegedge(clk) (co_yield (coyieldReturnClock_t){clk,coyieldEvent_t::clockNegedge})


    /**
     * @enum    class coyieldEvent_t
     * @author  Richard Herveille
     * @brief   Enumeration class holding trigger event types
     * @version 01
     * @date    5-may-2023
     *
     * @details Enumeration class holding the different type of events that are supported
     *          The co_routine function provides the type to the yield_value routine
     *          The yield_value routine then determines which action to take for the event
     */
    enum class coyieldEvent_t 
    {
        clockPosedge,
        clockNegedge
    };



    /**
     * @struct  coyieldReturn_t
     * @author  Richard Herveille
     * @brief   co_yield return struct
     * @version 0.1
     * @date    5-may-2023
     *
     * @details This struct holds the data co_yield provides to the yield_value routine
     */
    template <typename T>
    struct coyieldReturn_t 
    {
        T              object;
        coyieldEvent_t event;
    };



    /**
     * @struct  cCoRoutineHandler
     * @author  Richard Herveille
     * @brief   Test object 
     * @version 0.2
     * @date    16-may-2023
     * 
     * @details Typically tests stimulate the DUT on a cycle by cycle basis
     *          Multiple test could be running at the same time (think transaction based verification)
     *          This implies that multiple tests must be able to stop and resume, typically on a clock ede
     *          To facilitate this tests are implemented as coroutines which yield when they need to wait
     *          for an event; typically rising or falling edge of the clock
     *          The posedge() and negedge() routines yield the test and register the test with the
     *          event queues in the clock class. When the clock toggles it calls the routines in the
     *          respective queues, thereby resuming the tests.
     */

    using namespace std;

    //std::suspend_never::await_ready always returns false
    //std::suspend_always::await_ready always returns true

    //"co_await" suspends function
    //"co_yield expr" expands to "co_await p.yield_value(expr)" where 'p' is the promise object
    //"co_return expr" returns final value expr. This can/should be the result of the test (pass/fail)

    template <typename T>
    struct sCoRoutineHandler 
    {
        struct promise_type;
        using handle_t = coroutine_handle<promise_type>;

        //MUST include nested object type promise_type
        struct promise_type
        {
            T _value;                       //This variable is accessible from the coroutine function
            std::exception_ptr _exception;  //Store exception pointer

            //MUST include get_return_object()
            //the result of get_return_object() is the return value of the coroutine function
            sCoRoutineHandler get_return_object()
            {
                return sCoRoutineHandler(handle_t::from_promise(*this));
            }

            //suspend_always suspends the coroutine function immediatly on entry
            //suspend_never actually runs the coroutine function until co_wait()
            //For a testbench test, running the coroutine function sounds the correct approach
            suspend_never initial_suspend() { return {}; }

            //If final_suspend returns suspend_never then the object is automatically destroyed
            //however the state is not accessible then anymore (obviously)
            //If final_suspend returns suspend_always then the object's state remains accessible
            //after the coroutine function finishes, but the object must be destroyed externally
            //Most notably .done() can not be called if the object is destroyed
            //For a testbench test destroying the object sounds the correct approach
            suspend_never final_suspend() noexcept { return {}; }

            //Unhandled exception handler
            void unhandled_exception()
            {
                //store exception (so we can throw it later on)
                _exception = std::current_exception();
            }

            //called when co_yield (expr) is called
            template<std::convertible_to<T> From>
            suspend_always yield_value(From&& val)
            {
                //store value here into promise_type variable _value
                //_value = std::forward<From>(val);
                switch (val.event)
                {
                  case coyieldEvent_t::clockPosedge:
                           val.object->addWaitForPosedge(handle_t::from_promise(*this));
                           break;

                  case coyieldEvent_t::clockNegedge:
                           val.object->addWaitForNegedge(handle_t::from_promise(*this));
                           break;
                }
                return {};
            }

            /* !! Either return_void or return_value, but not both !! */
     
            //called when 'co_return' is called (note: not "co_return e") or if the
            //coroutine function falls off the end (i.e. completes)
            //CAUTION: falling of the end without a return_void() causes undefined behaviour!!!!
            void return_void() {}
        };


        //Handle to coroutine
        handle_t _h;

        //constructor
        //store handle to coroutine function
        sCoRoutineHandler (handle_t h) : _h(h) {};

        //destructor
        ~sCoRoutineHandler()
        {
            //Let's not do that!!! In the new testbench setup the object gets immediately destroyed
            //resulting in segfaults

            //destroy coroutine function
            //_h.destroy();
        }

        //Did the co_routine function finish?
        explicit operator bool() 
        {
            return !_h.done();
        }
    };


    //Awaiter
    template <typename PromiseType>
    struct sGetPromise 
    {
        PromiseType *_p;

        //If await_ready() returns true, then co_await does not suspend the function
        //if await_ready() returns false, then await_suspend is called
        constexpr bool await_ready() const noexcept { return false; }

        //if await_suspend returns false, then co_await does not suspend the function
        bool await_suspend(coroutine_handle<PromiseType> h) 
        {
            //store handle to promise object
            _p = h.promise();

            //don't suspend function, because we want to get access to the promise object
            //such that we can feedback values
            return false;
        }

        //await_resume provides the return value of co_await expression
        constexpr PromiseType* await_resume() const noexcept 
        {
            return _p;
        }
    };
}
}
}

#endif
