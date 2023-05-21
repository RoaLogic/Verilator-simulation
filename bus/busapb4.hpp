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
#include <buffer.hpp>

#define DBG_BUSAPB4_H

namespace RoaLogic
{
namespace bus
{
    #define L (!1)
    #define H (!0)

    using namespace RoaLogic::testbench;
    using namespace RoaLogic::testbench::tasks;
    using namespace common;

    /**
     * @class cBusAPB4
     * @author Richard Herveille
     * @brief APB Bus Interface Class
     * @version 0.1
     * @date 3-may-2023
     *
     * @details This is a class to drive an APB4 bus in a Verilator testbench
     *          The class requires a buffer. This can be any other class/type as long as it push_back() and pop_front() supports.
     * 
     */
    template <typename PADDR_t,
              typename PDATA_t,
              typename bufferType> class cBusAPB4 : public cBusInterface<PADDR_t,PDATA_t>
    {
        private:
            clock::cClock* _pclk;
            uint8_t&       PRESETn;
            uint8_t&       PSEL;
            uint8_t&       PENABLE;
            PADDR_t&       PADDR;
            uint8_t&       PWRITE;
            PDATA_t&       PWDATA;
            PDATA_t&       PRDATA;
            uint8_t&       PREADY;
            uint8_t&       PSLVERR;

//            bool   _busy;
            bool           _error;

        public:
            /**
             * @brief Construct a new cBusInterface object
             */
            cBusAPB4(clock::cClock* pclk,
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
                     PSLVERR (pslverr) {
          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4 bus id(" << this->id() << ") constructed" << std::endl;
          #endif
            }


            /**
             * @brief Destroy the cBusInterface
             */
            virtual ~cBusAPB4() {
          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4 bus id(" << this->id() << ") destroyed" <<  std::endl;
          #endif
            }



            /**
             * @brief Reset APB bus
             */
            virtual clockedTask_t reset(unsigned duration=1) {
                this->transactionStart();
                _error = false;

          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4(" << this->id() << ") PRESET=0" << std::endl;
          #endif
                PRESETn = L;
                for (int i=0; i<duration; i++) waitPosedge(_pclk);

          #ifdef DBG_BUSAPB4_H
              std::cout << "APB4(" << this->id() <<") PRESET=1" << std::endl;
          #endif
                PRESETn = H;
                //waitPosedge(_pclk);

                this->transactionEnd();
            }


            /**
             * @brief Perform a Single Read Transaction on the bus
             *
             * @param address[in]    Start address to read from
             * @param buffer[in]     Reference to buffer to store the data read
             * @param burstCount[in] Number of transactions in this burst
             */
            virtual clockedTask_t read(PADDR_t address, bufferType* buffer, unsigned burstCount=1) {
                this->transactionStart();
                _error = false;

                for (int transactionCnt=0; transactionCnt < burstCount; transactionCnt++)
                {
                    PSEL    = H;
                    PENABLE = L;
                    PADDR   = address;
                    PWRITE  = L;
                    waitPosedge(_pclk);

                    PENABLE = H;
                    waitPosedge(_pclk);

                    while (PREADY == L) waitPosedge(_pclk);

                    buffer->push_back(PRDATA);
                    _error = (PSLVERR == H);
                }

                PSEL    = L;
                PENABLE = L;

                this->transactionEnd();
            }


            /**
             * @brief Perform Write Transactions on the bus
             *
             * @param address[in]    Start address to write to
             * @param buffer[in]     Reference to buffer containint data to write
             * @param burstCount[in] Number of transactions in this burst
             */
            virtual clockedTask_t write(PADDR_t address, bufferType* buffer, unsigned burstCount=1) {
                this->transactionStart();
                _error = false;

                for (int transactionCnt=0; transactionCnt < burstCount; transactionCnt++)
                {
                    PSEL    = H;
                    PENABLE = L;
                    PADDR   = address;
                    PWRITE  = H;
                    PWDATA  = buffer->pop_front();
                    waitPosedge(_pclk);

                    PENABLE = H;
                    waitPosedge(_pclk);

                    while (PREADY == L) waitPosedge(_pclk);

                    _error = (PSLVERR == H);
                }

                PSEL    = L;
                PENABLE = L;

                this->transactionEnd();
            }
    };
}
}

#endif
