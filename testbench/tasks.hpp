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
//             Copyright (C) 2024 Roa Logic BV                     //
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

#ifndef TASKS_HPP
#define TASKS_HPP

#include <coroutine>
#include <cassert>
#include <clock.hpp>

namespace RoaLogic
{
namespace testbench
{
namespace tasks
{
    using namespace std;

    /**
     * @struct  cCoRoutineHandler
     * @author  Richard Herveille, Bjorn Schouteten
     * @brief   Test object 
     * 
     * @details Typically tests stimulate the DUT on a cycle by cycle basis
     *          Multiple test could be running at the same time (think transaction based verification)
     *          This implies that multiple tests must be able to stop and resume, typically on a clock edge
     */
    template <typename T>
    struct sCoRoutineHandler
    {
        struct promise_type;
        using handle_t = coroutine_handle<promise_type>;

        //MUST include nested object type promise_type
        struct promise_type
        {
            T _myValue;                     //!< This variable is accessible from the coroutine function
            std::exception_ptr _exception;  //!< Store exception pointer
            coroutine_handle<> waitingCoroutine;

            /**
             * @brief Get the return object
             * @details This function returns the created heap memory object 
             * of the coroutine
             * 
             * It must be included in the promise_type
             * 
             * @return sCoRoutineHandler<T>     Returns the heap memory object which is created
             */
            sCoRoutineHandler<T> get_return_object()
            {
                return {coroutine_handle<promise_type>::from_promise(*this)};
            }

            /**
             * @brief Initial suspend
             * @details This function is called at the beginning of a coroutine and determines
             * if the coroutine should suspend at the beginning and wait for a resume or that it
             * continues untill any of the co_expr is used.
             * 
             * When returning suspend_never the coroutine will run until a co_expr
             * When returning suspend_always the coroutine will directly be suspended
             * 
             * For testbenches we want the coroutine function to run until a co_expr and therefore 
             * we return suspend_never
             * 
             * @return suspend_never 
             */
            suspend_never initial_suspend() { return {}; }

            /**
             * @brief final suspend
             * @details This function is called at the end of a coroutine, it is used
             * to cleanup the coroutine. It always returns a awaitable type, where it has 
             * predefined behaviour for the standard awaitables.
             * 
             * When final_suspend returns suspend_never then the object is automatically destroyed
             * When final_suspend returns suspend_always then the object's state remains accessible
             * 
             * It's also possible to return a own awaitable, which is the case in this function,
             * it checks if there is a waiting coroutine. If this is the case we return the 
             * waiting coroutine, which is at this point resumed again. If there is no waiting
             * coroutine we return noop_coroutine, this is a special coroutine handle that has
             * no resume or destroy method, indicating to the coroutine that there is nothing 
             * to resume and it should return to it's caller.
             * 
             * Do note that .done() cannot be called if the object is already destroyed, therefore 
             * we can keep track of the final suspend state with the boolean _finished.
             * 
             * For testbenches we will keep the state, so that we can get the result of a coroutine 
             * at the end.
             * 
             * @return auto
             */
            auto final_suspend() noexcept 
            {
                struct awaiter
                {
                    coroutine_handle<> waitingCoroutine;
                    bool await_ready() const noexcept { return false;}
                    void await_resume() const noexcept {}

                    coroutine_handle<> await_suspend(coroutine_handle<promise_type> h) const noexcept
                    {
                        return waitingCoroutine ? waitingCoroutine : std::noop_coroutine();
                    }
                };
                
                return awaiter{waitingCoroutine}; 
            }

            /**
             * @brief unhandled exception
             * @details This function handles a exception
             * which occurs in the time of a coroutine. When this happens it stores
             * the exception into the structure which can then be gotten to check 
             * from outside of the promise type
             */
            void unhandled_exception()
            {
                _exception = std::current_exception();
            }

            /**
             * @brief yield_value
             * @details This function receives the value passed to co_yield.
             * It will receive the value and store it in our object itself so it
             * can be returned from the promise_type back to the user
             * 
             * This function must return either suspend_always or suspend_never,
             * where both mean the same as for initial_suspend.
             * 
             * 
             * @param val               Value passed by co_yield <value>
             * @return suspend_always   Always suspend the coroutine
             */
            
            suspend_always yield_value(T value)
            {
                _myValue = value;
                return {};
            }

            /**
             * @brief return value
             * @details This function is called just before final suspend and is used 
             * to return a value from the user. A promise type must hold either
             * void return_value() or void return_void() depending on how the coroutine 
             * is used. 
             * 
             * return_value() is used with "co_return e"
             * return_void() is used with "co_return"
             * 
             * For our testbench we will use return_value so we can get test results
             * from our test. (This can always be ignored)
             * 
             * CAUTION: falling of the end without a return_void() or return_value() 
             * causes undefined behaviour!!!!
             * 
             * @param T The value which is returned of the coroutine
             */
            void return_value(T value) 
            {
                _myValue = value;
            }
        };

        handle_t _h; //!< Handle to coroutine

        /**
         * @brief Construct a new coroutine handle
         * @details Constructor of the coroutine handler
         * 
         * @param h 
         */
        sCoRoutineHandler(handle_t h) : _h(h) {};

        /**
         * @brief Destroy the coroutine object
         * @details This function destroys the object
         * 
         * Destroy the object if there is a object
         * 
         */
        ~sCoRoutineHandler()
        {
            if(_h)
            {
                _h.destroy();
            }
        }

        /**
         * @brief Check if the coroutine is done
         * @details
         * 
         * This functions checks if the coroutine has finished, due that 
         * final_suspend returns suspend_always we cannot call .done().
         * Therefore the _finished is introduced in the promise type and 
         * we can check accordingly.
         * 
         * @return true 
         * @return false 
         */
        explicit operator bool() 
        {
            return _h.done(); // Use this when final_suspend returns suspend_never
            //return _h.promise()._finished; // Use this when final_suspend returns suspend_always
        }

        /**
         * @brief Resume the coroutine at the last suspended point
         * 
         */
        void resume()
        {
            _h.resume();
        }

        /**
         * @brief Get the Result 
         * @details This function returns the value gotten from the 
         * promise type. Or easier said, it returns the result of
         * the coroutine. This can either be the result from 
         * co_yield as well as co_return.
         * 
         * @attention When final_suspend returns suspend_always we cannot get 
         * the value of the co_return expr. This value will then be changed, due
         * that the promise is already deleted.
         * 
         * @return T    Returns coroutine value or false when there is no value
         */
        T getValue()
        {
            if (_h) 
            {
                return std::move(_h.promise()._myValue);
            }
            else
            {
                return false;
            }
        }

        /**
         * @brief The await ready function for the awaitable type
         * 
         * @return false    Always return false to suspend the coroutine
         */
        bool await_ready()
        {
            return false;
        }

        /**
         * @brief The await suspend function for the awaitable type
         * @details This function suspends the coroutine and implements
         * the needed logic. It stores the passed coroutine handle into the
         * waitingCoroutine handle of our current promise type.
         * 
         * This can then be used to resume the coroutine that called co_await
         * 
         * @param h     The coroutine handle which calls the co_await
         */
        void await_suspend(std::coroutine_handle<> h)
        {
            _h.promise().waitingCoroutine = h;
        }

        /**
         * @brief The await resume function for the awaitable type
         * @details This function is called when the coroutine is resumed again
         * It will return the result of the coroutine, which is placed in 
         * the promise type.
         * 
         * @return auto     Returns the result of the coroutine
         */
        auto await_resume()
        {
            return std::move(_h.promise()._myValue);
        }
    };
}
}
}

#endif
