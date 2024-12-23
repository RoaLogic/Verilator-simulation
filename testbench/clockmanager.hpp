/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for Verilator Testbench Clock Manager             //
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

#ifndef CLOCKMANAGER_HPP
#define CLOCKMANAGER_HPP

#include <clock.hpp>

//For vector
#include <vector>

//For MAXFLOAT
#include <limits>

//For logging
#include <log.hpp>

//#define DBG_CLOCKMANAGER_H
 
namespace RoaLogic
{
namespace testbench
{
namespace clock
{

    /**
     * @class cClockManager
     * @author Richard Herveille, Bjorn Schouteten
     * @brief Clock Manager Class for use in testbenches 
     * 
     * @details Manage the clock inputs of a verilated design
     */
    class cClockManager
    {
        private:
            std::vector<cClock*> *_clocks;   //!< Collection holding all clocks
            simtime_t _time;                 //!< Simulation time
            simtime_t _precision;            //!< Simulation precision

        public:

            /**
             * @brief Construct a new cClockManager object
             * 
             */
            cClockManager(simtime_t precision) : 
                _precision(precision),
                _time(0.0)
            {
                //Create new collection
                _clocks = new std::vector<cClock*>();
            }


            /**
             * @brief Destroy the cClockManager object
             * 
             */
            virtual ~cClockManager(void)
            {
                //Remove and destroy all clocks from collection
                for (const auto clock : *_clocks)
                {
                    delete clock;
                }            

                //Destroy collection
                delete _clocks;
                _clocks = NULL;
            }

            /**
             * @brief Add a new clock as reference
             * 
             * @param[in] Clock Clock object to add
             */
            virtual void add(cClock &Clock) const
            {
                _clocks->push_back(&Clock);
            }

            /**
             * @brief Add a new clock as pointer
             * 
             * @param[in] Clock Clock object to add
             */
            virtual void add(cClock* Clock) const
            {
                _clocks->push_back(Clock);
            }

            /**
             * @brief Add and create a new clock
             * 
             * @param[in] clk         Clock pin  
             * @param[in] LowPeriod   The period that the pin shall be low
             * @param[in] HighPeriod  The period that the pin shall be high
             * @param[in] on           The clock is directly active, default = true
             * @return cClock*        Pointer to the cClock object that is created
             */
            virtual cClock* const add(uint8_t& Clock, simtime_t LowPeriod, simtime_t HighPeriod, bool on = true) const
            {
                //Create new VClock
                cClock* clock = new cClock(Clock, _precision, LowPeriod, HighPeriod, on);

                //Add clock
                add(clock);

                //Return new object
                return clock;
            }

            /**
             * @brief Add and create a new clock
             * 
             * @param[in] clk         Clock pin
             * @param[in] Period      The period for the clock 
             * @param[in] on           The clock is directly active, default = true
             * @return cClock*        Pointer to the cClock object that is created
             */
            virtual cClock* const add(uint8_t& Clock, simtime_t Period, bool on = true) const
            {
                return add(Clock, Period/2.0, Period/2.0, on);
            }

            /**
             * @brief Check if there are any clocks
             * 
             * @return true     There is atleast one clock
             * @return false    No clocks
             */
            virtual bool empty(void) const
            {
                return _clocks->empty();
            }

            /**
             * @brief Time manager tick function
             * @details This function is the actual time manager for all clocks
             * 
             * It checks the periods of the clocks against the settings of each clock individually. If the 
             * time has passed it changes the clock input.
             * 
             * @return It returns the current passed time
             */
            simtime_t tick(void)
            {
                #ifdef DBG_CLOCKMANAGER_H
                DEBUG << "CLOCKMANAGER_H - tick() \n";
                #endif

                //Current minimum time to next event = maximum float value
                simtime_t minTimeToNextEvent = simtime_t::max();

                //Find next event
                for (const auto clk : *_clocks)
                {
                    simtime_t timeToNextEvent = clk->getTimeToNextEvent();

                    if (timeToNextEvent < minTimeToNextEvent)
                    {
                        minTimeToNextEvent = timeToNextEvent;
                    }             

                    #ifdef DBG_CLOCKMANAGER_H
                    //DEBUG << "CLOCKMANAGER_H - tick: " << clk->getTimeToNextEvent() << "," << minTimeToNextEvent << "\n";
                    #endif

                }

                //Update all clocks
                //Clocks automatically toggle when their time reaches 0
                for (const auto clk : *_clocks)
                {
                    clk->updateTime(minTimeToNextEvent);
                }

                //Store new time
                _time += minTimeToNextEvent;

                return _time;
            }

            /**
             * @brief get the current time
             * 
             * @return float 
             */
            virtual simtime_t getTime(void) const
            {
                return _time;
            }
    };
}
}
}

#endif
