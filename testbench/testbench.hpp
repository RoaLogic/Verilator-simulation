/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for Verilator Testbench                           //
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

#ifndef TESTBENCH_HPP
#define TESTBENCH_HPP

//For Verilator methods
#include <verilated.h>
#include <verilated_vcd_c.h>

//Clock Manager
#include "clockmanager.hpp"

//Assertions
#include <cassert>

//For logging
#include "log.hpp"
#include "tasks.hpp"

//#define DBG_TESTBENCH_H

namespace RoaLogic
{
namespace testbench
{
    using namespace clock;

    /**
     * @class cTestBench
     * @author Richard Herveille, Bjorn Schouteten
     * @brief Template base class for a testbench
     * @version 0.1
     * @date 30-apr-2023
     *
     * @details This is a template base class for testbenches 
     * 
     * This object controls the testbench in a generic way, which is the same for 
     * every testbench object. So every single testbench will be setup, used and 
     * deleted the same. Making it easier for a user to use this without needing
     * the exact knowledge on how everything works together.
     *
     */
    template <class VM> class cTestBench
    {
        private:
            VerilatedContext*  _context;     //!< Verilator Context
            VerilatedVcdC*     _trace;       //!< Verilator Trace instance 
            cClockManager*     _clkMgr;      //<! Clock Manager
            bool               _traceActive; //!< Boolean to store if the trace is active or not
            bool               _finished;    //!< Bool to check if the testbench has finished
            simtime_t          _timeprecision = 0; //!< Time precision of our simulation

        protected:
            VM*                _core;     //!< Verilator Model to test

            /**
             * @brief tick the testbench clocks
             * 
             * @attention This function is needed to progress the testbench timing
             * 
             */
            virtual void tick(void) const
            {
                #ifdef DBG_TESTBENCH_H
                DEBUG << "TESTBENCH_H - tick()" << std::endl;
                #endif

                //There should be at least 1 clock
                assert (!_clkMgr->empty());

                /*
                    * This is the correct order
                    * 1. eval design (this causes all @posedge to trigger)
                    * 2. trace->dump (previously evaluated values)
                    * 3. toggle clock and update time
                    *    this resumes any waiting routines
                    * 4. eval design (this causes all @posedge to trigger)
                    */
                //eval logic
                _core->eval();

                //dump trace
                if (_traceActive && _trace)
                {
                    simtime_t time = _clkMgr->getTime();
                    _trace->dump( (vluint64_t)(time.s() / _timeprecision) );
                }

                //tick() clocks and eval logic
                _clkMgr->tick();

                _core->eval();
            }

            /**
             * @brief Finish the simulation
             * 
             */
            virtual void finish(void)
            {
                _finished = true;
            }

            /**
             * @brief Check if the system has finished
             * 
             * @return true
             * @return false
             */
            virtual bool finished(void) const
            {
                return (_context->gotFinish() | _finished);
            }

            /**
             * @brief Add a new clock to the testbench
             * @details This function adds a new clock to the current testbench
             * 
             * @param[in] Clock        Clock pin  
             * @param[in] LowPeriod   The period that the pin shall be low
             * @param[in] HighPeriod  The period that the pin shall be high
             * @return a pointer to the clock object
             */
            virtual cClock* addClock(uint8_t& Clock, simtime_t LowPeriod, simtime_t HighPeriod) const
            {
                return _clkMgr->add(Clock, LowPeriod, HighPeriod);
            }

            /**
             * @brief Add a new clock to the testbench
             * @details This function adds a new clock to the current testbench
             * 
             * @param[in] Clock       Clock pin 
             * @param[in] Period      The period of the clock pin
             * @return a pointer to the clock object 
             */
            virtual cClock* addClock(uint8_t& Clock, simtime_t Period) const
            {
                return _clkMgr->add(Clock, Period);
            }

            /**
             * @brief Get the runtime of the simulation
             * 
             * @return simulation runtime
             */
            virtual simtime_t getTime(void) const
            {
                return _clkMgr->getTime();
            }

            /**
             * @brief Return the precision of the simulation context
             * 
             * @return simtime_t Precision of the simulation context
             */
            virtual simtime_t getPrecision(void) const
            {
                return _timeprecision;
            }

        public:
            /**
             * @brief Construct a new cTestBench object
             * 
             * @param[in] context           The verilated context the object exists in
             * @param[in] traceActive       Boolean to set the trace active or inactive
             */
            cTestBench(VerilatedContext* context, bool traceActive) :
                _context(context),
                _finished(false),
                _traceActive(traceActive)
            {
                if(traceActive)
                {
                    Verilated::traceEverOn(true);
                }

                _timeprecision = pow(10, 0 - context->timeprecision());

                _core = new VM; // Create a new verilator model
                _clkMgr = new cClockManager(_timeprecision); //Create new Clock Manager
                _trace = nullptr;
            }

            /**
             * @brief Destroy the cTestBench object
             * 
             */
            virtual ~cTestBench(void)
            {
                if(_traceActive)
                {
                    closeTrace();
                }

                //save time before we destroy object
                simtime_t time = getTime();

                //Final Cleanup
                _core->final();
                delete _core;

                //Destroy Clock Manager
                delete _clkMgr;

                #ifdef DBG_TESTBENCH_H
                INFO << "Testbench finished at " << time << "\n";
                #endif
            }

            /**
             * @brief Open a trace file
             * 
             * @param[in] fileName  Name of the trace file
             */
            void opentrace(const char *fileName)
            {
                if(_traceActive)
                {
                    if (!_trace) 
                    {
                        _trace = new VerilatedVcdC;
                        _core->trace(_trace, 99);
                        _trace->open(fileName);
                    }
                }
                else
                {
                    ERROR << "Trace is not active, no need to open";
                }
            }

            void opentrace(std::string fileName)
            {
                opentrace(fileName.c_str());
            }

            /**
             * @brief Close any open trace
             * 
             */
            void closeTrace(void)
            {
                if (_trace) 
                {
                    _trace->flush();
                    _trace->close();
                    _trace = NULL;
                }
            }
    };
}
}

#endif
