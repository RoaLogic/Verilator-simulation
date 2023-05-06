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
 * @file clock.hpp
 * @author Richard Herveille
 * @brief Clock object
 * @version 0.1
 * @date 30-apr-2023
 * @copyright See beginning of file
 */

#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <simtime.hpp>
#include <uniqueid.hpp>

#include <queue>
#include <cassert>

namespace RoaLogic
{
namespace testbench
{
namespace clock
{

//#define DBG_VCLOCK_H

    /**
     * @class cClock
     * @author Richard Herveille
     * @brief Clock object 
     * @version 0.1
     * @date 30-apr-2023
     * 
     * @details clock object to handle single clock inputs on the verilated design
     * The object holds a reference to the clock pin variable and the high and low period.
     * It will transition from low-to-high or high-to-low according to the time of the high and low periods.
     */
    class cClock : common::cUniqueId
    {
        private:
        uint8_t&    _clk;             //!< Points to testbench clock variable
        simtime_t  _lowPeriod;        //!< Clock Low Period in seconds
        simtime_t  _highPeriod;       //!< Clock High Period in seconds
        simtime_t  _timeToNextEvent;  //!< Time until next event

        //queues for posedge/negedge coroutine functions
        std::queue<std::function<void()>> posedgeQueue;
        std::queue<std::function<void()>> negedgeQueue;


        public:

        /**
         * @brief Construct a new VClock object
         * 
         * @param[in] clk         A reference to a clock pin   
         * @param[in] LowPeriod   The low period of the clock
         * @param[in] HighPeriod  The high period of the clock
         */
        cClock(uint8_t& clk, simtime_t LowPeriod, simtime_t HighPeriod) :
            _clk(clk),
            _lowPeriod(LowPeriod),
            _highPeriod(HighPeriod)
        {
            //set clock low
            _clk = 0;

            //clock is low, so TimeToNextEvent is LowPeriod
            _timeToNextEvent = LowPeriod;

      #ifdef DBG_VCLOCK_H
          std::cout << "VCLOCK_H constructor(" << id() << ") lvl=" << _clk << " LowPeriod=" << _timeToNextEvent << std::endl;
      #endif
        }


        /**
         * @brief Destroy the cClock object
         * 
         */
        virtual ~cClock(void) 
        {

        };


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
         * @return simtime_t 
         */
        virtual simtime_t getPeriod(void) const
        {
            return (_lowPeriod + _highPeriod);
        }


        /**
         * @brief Get the Frequency
         * 
         * @return long double 
         */
        virtual long double getFrequency(void) const
        {
            return getPeriod().frequency();
        }


        /**
         * @brief Get the Time To Next Event object
         * 
         * @return float 
         */
        virtual simtime_t getTimeToNextEvent(void) const
        {
      #ifdef DBG_VCLOCK_H
            std::cout << "VCLOCK_H(" << id() << ") - getTimeToNextEvent:" << _timeToNextEvent << "\n";
      #endif

            return _timeToNextEvent;
        }


        /**
         * @brief Update the time to the next event
         * 
         * @param[in] TimePassed The time that has passed since previous function
         * @return The time untill the next event
         */
        virtual simtime_t updateTime(simtime_t TimePassed)
        {
            //Subtract time-passed from the time until the next event
            _timeToNextEvent -= TimePassed;


            //_TimeToNextEvent should never be negative
            assert(_timeToNextEvent >= 0);

            //if TimeToNextEvent==0, then toggle the clock
            if (_timeToNextEvent==0) 
            {
                toggle();
            }

            return _timeToNextEvent;
        }


        /**
         * @brief Toggle the clock pin
         * 
         * @return the new state of the pin
         */
        uint8_t toggle(void)
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

            //return new state
            return _clk;
        }


        /**
         * @brief Add co_routine function waiting for posedge
         */
        void addWaitForPosedge(std::function<void()> h)
        {
      #ifdef DBG_VCLOCK_H
          std::cout << "addWaitForPosedge(" << id() << ")\n";
      #endif
            posedgeQueue.push(h);
        }


        /**
         * @brief Add co_routine function waiting for negedge
         */
        void addWaitForNegedge(std::function<void()> h)
        {
            negedgeQueue.push(h);
        }


        /**
         * @brief Resume functions waiting on rising clock edge
         */
        void resumeWaitForPosedge()
        {
            //is there anything to do?
            if (!posedgeQueue.empty())
            {
      #ifdef DBG_VCLOCK_H
          std::cout << "resumeWaitForPosedge(" << id() << ")\n";
      #endif
                //create new empty queue
                std::queue<std::function<void()>> posedgeQueueCopy;

                //swap empty queue and posedgeQueue
                posedgeQueueCopy.swap(posedgeQueue);

                //we know there's at least 1 element in posedgeQueueCopy
                do
                {
                    //pop the function from the queue
                    std::function<void()> h = posedgeQueueCopy.front();
                    posedgeQueueCopy.pop();

                    //call the function
                    h();
                } while (!posedgeQueueCopy.empty());
            }
        }


        /**
         * @brief Resume functions waiting on rising clock edge
         */
        void resumeWaitForNegedge()
        {
            //is there anything to do?
            if (!negedgeQueue.empty())
            {
                //create new empty queue
                std::queue<std::function<void()>> negedgeQueueCopy;

                //swap empty queue and negedgeQueue
                negedgeQueueCopy.swap(negedgeQueue);

                //we know there's at least 1 element in negedgeQueueCopy
                do
                {
                    //pop the function from the queue
                    std::function<void()> h = negedgeQueueCopy.front();
                    negedgeQueueCopy.pop();

                    //call the function
                    h();
                } while (!negedgeQueueCopy.empty());
            }
        }
    };
}
}
}

#endif
