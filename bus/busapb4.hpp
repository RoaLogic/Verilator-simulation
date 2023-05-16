/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Class for Verilator APB Bus Interface                        //
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
 * @file busapb4.hpp
 * @author Richard Herveille
 * @brief APB4 Bus Interface class
 * @version 0.1
 * @date 3-may-2023
 * @copyright See beginning of file
 */

#ifndef BUSAPB4_HPP
#define BUSAPB4_HPP

#define BUSAPB4_HPP

#include <businterface.hpp>
#include <tasks.hpp>
#include <clock.hpp>

#define DBG_BUSAPB4_H

namespace RoaLogic
{
namespace bus
{
    using namespace RoaLogic::testbench;
    using namespace RoaLogic::testbench::tasks;

    template <typename PADDR_t = uint8_t,
              typename PDATA_t = uint8_t>
    struct apb4_t {
        uint8_t& PRESETn;
        uint8_t& PCLK;
        uint8_t& PSEL;
        uint8_t& PENABLE;
        PADDR_t& PADDR;
        uint8_t& PWRITE;
        PDATA_t& PWDATA;
        PDATA_t& PRDATA;
        uint8_t& PREADY;
        uint8_t& PSLVERR;
    };

    /**
     * @class cBusAPB4
     * @author Richard Herveille
     * @brief APB Bus Interface Class
     * @version 0.1
     * @date 3-may-2023
     *
     * @details This is a class to drive an APB4 bus in a Verilator testbench
     * 
     */
    template <typename PADDR_t = uint8_t,
              typename PDATA_t = uint8_t> class cBusAPB4 : public cBusInterface<PADDR_t, PDATA_t>
    {
        private:
            cClock*  _pclk;
            uint8_t& PRESETn;
            uint8_t& PSEL;
            uint8_t& PENABLE;
            PADDR_t& PADDR;
            uint8_t& PWRITE;
            PDATA_t& PWDATA;
            PDATA_t& PRDATA;
            uint8_t& PREADY;
            uint8_t& PSLVERR;

            bool   _busy;
            bool   _error;

        public:
            /**
             * @brief Construct a new cBusInterface object
             */
            cBusAPB4(cClock*  pclk,
                     uint8_t& presetn,
                     uint8_t& psel,
                     uint8_t& penable,
                     PADDR_t& paddr,
                     uint8_t& pwrite,
                     PDATA_t& pwdata,
                     PDATA_t& prdata,
                     uint8_t& pready,
                     uint8_t& pslverr) :
                     PRESETn (presetn),
                     _pclk   (pclk   ),
                     PSEL    (psel   ),
                     PENABLE (penable),
                     PADDR   (paddr  ),
                     PWRITE  (pwrite ),
                     PWDATA  (pwdata ),
                     PRDATA  (prdata ),
                     PREADY  (pready ),
                     PSLVERR (pslverr),
                     _busy          (false  ),
                     _error         (false  ) {
          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4 bus created id():" << std::endl;
          #endif
            };


            /**
             * @brief Destroy the cBusInterface
             */
            virtual ~cBusAPB4() {
          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4 bus destroyed id():" << std::endl;
          #endif
            };


            /**
             * @brief Is a transaction in progress?
             *
             * @return true when a transaction is in progress
             */
            bool busy() const { return  _busy; };


            /**
             * @brief Has current transaction completed?
             *
             * @return true when the current transaction completed
             */
            bool done() const { return !_busy; }; 


            /**
             * @brief Bus transacted terminated with/due to error?
             *
             * @return true when the bus transaction terminated with/due to an error
             */
            bool error() const { return _error; };


            /**
             * @brief Reset APB bus
             */
            virtual clockedTask_t reset(unsigned duration=1) {
                _busy  = true;
                _error = false;

          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4 bus id():... PRESET=0" << std::endl;
          #endif
                PRESETn = !1;
                for (int i=0; i<duration; i++)
                    waitPosedge(_pclk);


          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4 bus id():... PRESET=1" << std::endl;
          #endif
                PRESETn = !0;
                //waitPosedge(_pclk);

                _busy = false;
            };


            /**
             * @brief Perform a Single Read Transaction on the bus
             *
             * @param address[in] Address to read from
             * @return            Data read
             */
            virtual clockedTask_t read(PADDR_t address, PDATA_t& data) {
                _busy  = true;
                _error = false;

                PSEL   = !0;
                PADDR  = address;
                PWRITE = !1;
                waitPosedge(_pclk);

                PENABLE = !0;
                waitPosedge(_pclk);

                while (PREADY == !1) waitPosedge(_pclk);

                _busy  = false;
                _error = (PSLVERR == !0);

                PSEL = !1;
                PENABLE = !1;

                data = PRDATA;
            };


            /**
             * @brief Perform a Burst Read Transaction on the bus
             *
             * @param address[in]    Start address of burst
             * @param burstCount[in] Lenght of burst
             * @result               Array of read data
             */
            virtual clockedTask_t burstRead(PADDR_t address, PDATA_t* buffer, unsigned burstCount) {
              co_return;
            }


            /**
             * @brief Perform a Write Transaction on the bus
             *
             * @param address[in]  Address to write to
             * @param data[in]     Data to write
             */
            virtual clockedTask_t write(PADDR_t address, PDATA_t data) {
                _busy  = true;
                _error = false;

                PSEL   = !0;
                PADDR  = address;
                PWRITE = !0;
                PWDATA = data;
                waitPosedge(_pclk);

                PENABLE = !0;
                waitPosedge(_pclk);

                while (PREADY == !1) waitPosedge(_pclk);

                _busy  = false;
                _error = (PSLVERR == !0);

                PSEL = !1;
                PENABLE = !1;
            };


            /**
             * @brief Perform a Write Transaction on the bus
             *
             * @param address[in]  Start address of burst
             * @param data[in]     Pointer to Data to write
             */
            virtual clockedTask_t burstWrite(PADDR_t address, PDATA_t* buffer, unsigned burstCount) {
              co_return;
            };
    };
}
}

#endif
