/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for a Verilator Clock object                      //
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

#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <simtime.hpp>
#include <uniqueid.hpp>

#include <coroutine>
#include <queue>
#include <cassert>
#include <log.hpp>

namespace RoaLogic
{
namespace testbench
{
namespace clock
{

    using namespace units;
    using namespace std;

//#define DBG_CLOCK_H

    /**
     * @brief Enum to describe the possible
     * clock edges that a clock can trigger on
     * 
     */
    enum class eClockEdge
    {
        positive,
        negative
    };

    #define waitPosEdge(clk) co_await cClockAwaitable(clk, eClockEdge::positive); 
    #define waitNegEdge(clk) co_await cClockAwaitable(clk, eClockEdge::negative); 

    /**
     * @class cClock
     * @author Richard Herveille, Bjorn Schouteten
     * @brief Clock object 
     * 
     * @details clock object to handle single clock inputs on the verilated design
     * The object holds a reference to the clock pin variable and the high and low period.
     * It will transition from low-to-high or high-to-low according to the time of the high and low periods.
     * 
     * Next to this it will also check if there is a coroutine waiting for a positive or negative edge, if this
     * is the case the coroutine will be resumed.
     */
    class cClock : common::cUniqueId
    {
        private:
        uint8_t&    _clk;             //!< Points to testbench clock variable
        simtime_t  _lowPeriod;        //!< Clock Low Period in seconds
        simtime_t  _highPeriod;       //!< Clock High Period in seconds
        simtime_t  _timeToNextEvent;  //!< Time until next event
        simtime_t  _precision;        //!< Time precision to toggle the clock

        std::queue<std::coroutine_handle<>> posedgeQueue; //!< Positive edge coroutine queue
        std::queue<std::coroutine_handle<>> negedgeQueue; //!< Negative edge coroutine queue

        /**
         * @brief Toggle the clock pin
         * @details Within this function the clock pin is toggled
         * 
         * It inverses the current state, toggling the pin
         * Following this it sets the time to the next event, according
         * to the set high and low period
         * 
         * At last it checks the state of the pin and resumes any
         * waiting coroutines by calling the appropiate handler
         */
        void toggle(void)
        {
            //toggle clock
            _clk = !_clk;

            //Update TimeKeep
            _timeToNextEvent = _clk ? _highPeriod : _lowPeriod;

            //call routines waiting for posedge/negedge
            if (_clk)
            {
                resumeWaitForPosedge();
            }
            else
            {
                resumeWaitForNegedge();
            }
        }

        /**
         * @brief Resume functions waiting on rising clock edge
         * @details This function checks if there are any coroutines
         * waiting on the positive edge.
         * 
         * When a positive edge has occured, this function must be called.
         * It will check if there are any coroutines waiting for a positive
         * edge, if this is the case it will resume all the waiting coroutines
         * in the queue.
         * 
         * Note that a copy of the queue is needed, due that when resuming coroutines 
         * new coroutines are added into the queue again, looping them around. When 
         * there is a copy, we can handle the copy and the newly added coroutines
         * are for the next edge of the clock
         */
        void resumeWaitForPosedge()
        {
            if(!posedgeQueue.empty())
            {
                // Create a new temporary empty queue
                std::queue<std::coroutine_handle<>> queueCopy;

                // Swap empty queue and posedgeQueue
                posedgeQueue.swap(queueCopy);

                do
                {
                    //pop the function from the queue
                    std::coroutine_handle<> h = queueCopy.front();
                    queueCopy.pop();

                    //resume the coroutine
                    h.resume();
                } while (!queueCopy.empty());
            }
        }

        /**
         * @brief Resume functions waiting on negative clock edge
         * @details This function checks if there are any coroutines
         * waiting on the negative edge.
         * 
         * When a negative edge has occured, this function must be called.
         * It will check if there are any coroutines waiting for a negative
         * edge, if this is the case it will resume all the waiting coroutines
         * in the queue.
         * 
         * Note that a copy of the queue is needed, due that when resuming coroutines 
         * new coroutines are added into the queue again, looping them around. When 
         * there is a copy, we can handle the copy and the newly added coroutines
         * are for the next edge of the clock
         */
        void resumeWaitForNegedge()
        {
            if(!negedgeQueue.empty())
            {
                // Create a new temporary empty queue
                std::queue<std::coroutine_handle<>> queueCopy;

                // Swap empty queue and posedgeQueue
                negedgeQueue.swap(queueCopy);

                do
                {
                    //pop the function from the queue
                    std::coroutine_handle<> h = queueCopy.front();
                    queueCopy.pop();

                    //resume the coroutine
                    h.resume();
                } while (!queueCopy.empty());
            }
        }

        public:

        /**
         * @brief Construct a new clock object
         * @details Constructor of the clock object
         * 
         * Map the corresponding inputs to the right interal variables
         * Set the first event to the lowPeriod, since the clock 
         * starts from 0 (low)
         * 
         * @param[in] clk         A reference to a clock pin
         * @param[in] precision   Precision setting of the clock      
         * @param[in] LowPeriod   The low period of the clock
         * @param[in] HighPeriod  The high period of the clock
         */
        cClock(uint8_t& clk, simtime_t precision, simtime_t LowPeriod, simtime_t HighPeriod) :
            _clk(clk),
            _precision(precision),
            _lowPeriod(LowPeriod),
            _highPeriod(HighPeriod)
        {
            //set clock low
            _clk = 0;

            //clock is low, so TimeToNextEvent is LowPeriod
            _timeToNextEvent = LowPeriod;

            #ifdef DBG_CLOCK_H
            DEBUG << "CLOCK_H (" << id() << ") constructor lvl=" << (unsigned)_clk << " LowPeriod=" << _timeToNextEvent << "\n";
            #endif
        }


        /**
         * @brief Destroy the cClock object
         * 
         */
        virtual ~cClock(void) 
        {
            #ifdef DBG_CLOCK_H
            DEBUG << "CLOCK_H (" << id() << ") destructor\n";
            #endif
        }


        /**
         * @brief Set and get the Low Period 
         * 
         * @param[in] Period  The low period to set  
         * @return The new low period
         */
        virtual simtime_t setLowPeriod(simtime_t Period)
        {
            _lowPeriod = Period;
            return _lowPeriod;
        }


        /**
         * @brief Get the Low Period 
         * 
         * @return The low period
         */
        virtual simtime_t getLowPeriod(void) const
        {
            return _lowPeriod;
        }


        /**
         * @brief Set and get the High Period 
         * 
         * @param[in] Period  The high period to set  
         * @return The new high period
         */
        virtual simtime_t setHighPeriod(simtime_t Period)
        {
            _highPeriod = Period;
            return _highPeriod;
        }


        /**
         * @brief Get the high Period 
         * 
         * @return The high period
         */
        virtual simtime_t getHighPeriod(void) const
        {
            return _highPeriod;
        }


        /**
         * @brief Get the Period 
         * 
         * @return Period of the clock
         */
        virtual simtime_t getPeriod(void) const
        {
            return (_lowPeriod + _highPeriod);
        }


        /**
         * @brief Get the Frequency
         * 
         * @return Frequency of the clock
         */
        virtual long double getFrequency(void) const
        {
            return getPeriod().frequency();
        }


        /**
         * @brief Get the Time To Next Event
         * 
         * @return The time untill the next event
         */
        virtual simtime_t getTimeToNextEvent(void) const
        {
            #ifdef DBG_CLOCK_H
            DEBUG << "CLOCK_H(" << id() << ") - getTimeToNextEvent:" << _timeToNextEvent << "\n";
            #endif

            return _timeToNextEvent;
        }


        /**
         * @brief Update the time
         * @details This function updates the time of the clock
         * 
         * The clock manager controls the clocks and tells them the time that 
         * has passed. This time is then subtracted from the time to next event,
         * by this we know what time we are. If the time of the clock is smaller
         * then the precision we must toggle the clock. 
         * 
         * Note: When time is smaller then the precision we can't show it
         * any better in the waveform, so we can toggle the clock already.
         * 
         * @param[in] TimePassed The time that has passed since previous function
         * @return The time untill the next event
         */
        virtual simtime_t updateTime(simtime_t TimePassed)
        {
            #ifdef DBG_CLOCK_H
            DEBUG << "CLOCK_H(" << id() << ") updateTime(" << TimePassed << ")\n";
            #endif
            //Subtract time-passed from the time until the next event
            _timeToNextEvent -= TimePassed;

            //_TimeToNextEvent should never be negative
            assert(_timeToNextEvent >= 0);

            //It's possible that there are some small rouding errors in the 
            //calculation so therefore use a window of the given precision and 0
            if (_timeToNextEvent >= 0 && _timeToNextEvent < _precision) 
            {
                toggle();
            }

            return _timeToNextEvent;
        }

        /**
         * @brief Wait for clock edge
         * @details This function add a coroutine handle to
         * the internal queues.
         * 
         * When a coroutine is suspended and waits untill a clock edge 
         * this function is called by the clockAwaitable. It then tells
         * on which edge we are waiting and passes the coroutine handle.
         * This handle is placed in the corresponding queue and when
         * that event happens the coroutine is resumed again.
         * 
         * @param[in] edge  The edge type the coroutine waits for
         * @param[in] h     Handle to the coroutine
         */
        void waitEdge(eClockEdge edge, coroutine_handle<> h)
        {
            switch (edge)
            {
            case eClockEdge::positive:
                #ifdef DBG_CLOCK_H
                DEBUG << "CLOCK_H(" << id() << ") wait positive edge\n";
                #endif
                posedgeQueue.push(h);
                break;

            case eClockEdge::negative:
                #ifdef DBG_CLOCK_H
                DEBUG << "CLOCK_H(" << id() << ") wait negative edge\n";
                #endif
                negedgeQueue.push(h);
                break;
            
            default:
                FATAL << "Unkown clock edge \n";
                break;
            }
        }
    };

    /**
     * @class cClock
     * @author Bjorn Schouteten
     * @brief Clock awaitable
     * @version 0.1
     * @date 30-apr-2023
     * 
     * @details This class is the awaitable for any clock events
     * It is used to suspend a coroutine and let it wait for the corresponding clock edge.
     * Nothing else can be done with this clock awaitable.
     * 
     * It always suspends the coroutine and calls the corresponding function to add the 
     * coroutine handle into the clock queue. When constructing the awaitable the clock 
     * and edge is passed. 
     * 
     * This class is universal and must be used for every clock
     * 
     * Usage: co_await cClockAwaitable(pclk, eClockEdge::positive);
     * 
     */
    class cClockAwaitable
    {
        private:
        cClock* _clock;
        eClockEdge _edge;

        public:
        cClockAwaitable(cClock* aClock, eClockEdge edge) : _clock(aClock), _edge(edge){};

        bool await_ready()
        {
            return false;
        }

        auto await_suspend(coroutine_handle<> handle)
        {
            _clock->waitEdge(_edge, handle);
            return std::noop_coroutine();
        }

        void await_resume()
        {

        }
    };

}
}
}

#endif