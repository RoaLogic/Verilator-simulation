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
 * @file testbench.hpp
 * @author Richard Herveille
 * @brief Clock object
 * @version 0.1
 * @date 30-apr-2023
 * @copyright See beginning of file
 */

#ifndef TESTBENCH_HPP
#define TESTBENCH_HPP

//For Verilator methods
#include <verilated.h>
#include <verilated_vcd_c.h>

//Clock Manager
#include <clockmanager.hpp>

//Assertions
#include <cassert>

//For std::cout
#include <iostream>

//#define DBG_TESTBENCH_H

namespace RoaLogic
{
namespace testbench
{

    using namespace clock;

    /**
     * @class cTestBench
     * @author Richard Herveille
     * @brief Template base class for a testbench
     * @version 0.1
     * @date 30-apr-2023
     *
     * @details This is a template base class for testbenches 
     * 
     * This object controls the testbench in a generic way, which is the same for 
     * every testbench object.
     *
     */
    template <class VM> class cTestBench
    {
        private:
            VerilatedContext* _context;  // Verilator Context 
            VerilatedVcdC*    _trace;    // Trace file           
            cClockManager*    _clkMgr;   // Clock Manager
            bool              _finished; // Testbench finished

        protected:
            VM*               _core;     //Verilator Model to test
    
        public:

            /**
             * @brief Construct a new cTestBench object
             * 
             * @param[in] context         The verilated context the object exists in
             */
            cTestBench(VerilatedContext* context) :
                _context(context),                
                _finished(false)
            {
                Verilated::traceEverOn(true);
                _core = new VM;
                //Create new Clock Manager
                _clkMgr = new cClockManager();
            }

            /**
             * @brief Destroy the cTestBench object
             * 
             */
            virtual ~cTestBench(void)
            {
                close();

                //save time before we destroy object
                simtime_t time = getTime();

                //Final Cleanup
                _core->final();
                delete _core;

                //Destroy Clock Manager
                delete _clkMgr;

                std::cout << "Testbench finished at " << time << "\n";
            }

            /**
             * @brief Open a trace file
             * 
             * @param vcdname 
             */
            void opentrace(const char *vcdname) 
            {
                if (!_trace) 
                {
                    _trace = new VerilatedVcdC;
                    _core->trace(_trace, 99);
                    _trace->open(vcdname);
                }
            }

            // Close a trace file
            void close(void) 
            {
                if (_trace) 
                {
                    _trace->flush();
                    _trace->close();
                    _trace = NULL;
                }
            }

            /**
             * @brief Evaluate the device under test
             * 
             */
            virtual void eval(void)
            {
                _core->eval();
            }

            /**
             * @brief Add a new clock to the testbench
             * @details This function adds a new clock to the current testbench
             * 
             * @param[in] clk         Clock pin  
             * @param[in] LowPeriod   The period that the pin shall be low
             * @param[in] HighPeriod  The period that the pin shall be high
             * @return a pointer to the clock object
             */
            virtual cClock* addClock(uint8_t& clk, simtime_t LowPeriod, simtime_t HighPeriod)
            {
          #ifdef DBG_TESTBENCH_H
              std::cout << "TESTBENCH_H - addClock (" << LowPeriod << "," << HighPeriod << ")\n";
          #endif
              return _clkMgr->add(clk, LowPeriod, HighPeriod);
            }

            /**
             * @brief Add a new clock to the testbench
             * @details This function adds a new clock to the current testbench
             * 
             * @param[in] clk         Clock pin 
             * @param[in] Period      The period of the clock pin
             * @return a pointer to the clock object 
             */
            virtual cClock* addClock(uint8_t& clk, simtime_t Period)
            {
                return _clkMgr->add(clk, Period);
            }

            /**
             * @brief tick the testbench clock
             * 
             * @attention This function is needed to progress the testbench timing
             * 
             */
            virtual void tick(void)
            {
                simtime_t time; 

                //There should be at least 1 clock
                assert (!_clkMgr->empty());

                //eval logic and tick clock(s)
                _core->eval();
                time = _clkMgr->tick();
                _core->eval();

                if (_trace)
                {
                    //apparently internal resolution is 1000x Verilator's VCD dump
                    _trace->dump( (vluint64_t)time.ns() *1000 );
                } 

                #ifdef DBG_TESTBENCH_H
                    std::cout << "TESTBENCH_H - Tick: " << time << "\n";
                #endif
            }

            /**
             * @brief Get the runtime of the simulation
             * 
             * @return simulation runtime
             */
            virtual simtime_t getTime(void)
            {
              return _clkMgr->getTime();
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
            virtual bool finished(void)
            {
                return (_context->gotFinish() | _finished);
            }

    };

}
}

#endif
