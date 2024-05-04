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

#ifndef BUSAPB4_HPP
#define BUSAPB4_HPP

#define BUSAPB4_HPP

#include <businterface.hpp>
#include <tasks.hpp>
#include <clock.hpp>
#include <buffer.hpp>
#include <log.hpp>

//#define DBG_BUSAPB4_H

namespace RoaLogic
{
    using namespace common;
namespace bus
{
    #define L (!1)
    #define H (!0)

    using namespace RoaLogic::testbench;
    using namespace RoaLogic::testbench::tasks;
    using namespace common;

    /**
     * @class cBusAPB4
     * @author Richard Herveille, Bjorn Schouteten
     * @brief APB Bus Interface Class
     * @version 0.1
     * @date 3-may-2023
     *
     * @details This is a class to drive an APB4 bus in a Verilator testbench
     * 
     */
    template <typename PADDR_t, typename PDATA_t> 
    class cBusAPB4 : public cBusInterface<PADDR_t,PDATA_t>
    {
        private:
            cClock*   PCLK;
            uint8_t&  PRESETn;
            uint8_t&  PSEL;
            uint8_t&  PENABLE;
            PADDR_t&  PADDR;
            uint8_t&  PWRITE;
            PDATA_t&  PWDATA;
            PDATA_t&  PRDATA;
            uint8_t&  PREADY;
            uint8_t&  PSLVERR;

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
                     PCLK    (pclk   ),
                     PRESETn (presetn),                     
                     PSEL    (psel   ),
                     PENABLE (penable),
                     PADDR   (paddr  ),
                     PWRITE  (pwrite ),
                     PWDATA  (pwdata ),
                     PRDATA  (prdata ),
                     PREADY  (pready ),
                     PSLVERR (pslverr) 
            {
                #ifdef DBG_BUSAPB4_H
                DEBUG << "APB4 bus id(" << this->id() << ") constructed \n";
                #endif

                PREADY = L;
            }


            /**
             * @brief Destroy the cBusInterface
             */
            virtual ~cBusAPB4() 
            {
                #ifdef DBG_BUSAPB4_H
                DEBUG << "APB4 bus id(" << this->id() << ") destroyed \n";
                #endif
            }

            /**
             * @brief Perform a read transaction on the bus
             * @details This function will perform a read
             * on the APB bus. It will start reading from the address and continue
             * for the number passed in with burstCount. With this updating the
             * address and placing the data in the given buffer.
             *
             * @param address     Start address to read from
             * @param buffer      Databuffer to store the data read
             * @param burstCount  Number of transactions in this burst
             */
            virtual sCoRoutineHandler<bool> read(PADDR_t address, PDATA_t* buffer, unsigned burstCount=1) 
            {
                bool result = false;
                uint8_t addressOffset = sizeof(PDATA_t); // Determine the size of a single element so we can advance the address

                if(!this->busy())
                {
                    this->transactionStart();
                    this->_error = false;

                    // Perform the number of transaction passed by the user
                    for (size_t i = 0; i < burstCount; i++)
                    {
                        #ifdef DBG_BUSAPB4_H
                        DEBUG << "APB4 bus(" << this->id() << ") read from " << hex << unsigned(address + (addressOffset * i)) << '\n';
                        #endif

                        // Here we are at the IDLE phase
                        // Now we set the signals accordingly
                        PENABLE  = L;
                        PWRITE   = L;
                        PADDR    = (address + (addressOffset * i)); // Set the address, add the offset onto the address
                        PSEL     = H;

                        // Wait for the positive edge of the clock to transition into the access phase
                        waitPosEdge(PCLK);

                        // Set PENABLE high, so that we transition into the access phase with the next clock cycle
                        PENABLE  = H;

                        waitPosEdge(PCLK);

                        // Wait until PREADY is in the right state and then transition out of the transaction
                        // PENABLE is already at the right state, since we set it at that point
                        // Since this is a single transaction it ends here.
                        while (PREADY == L) waitPosEdge(PCLK);

                        PENABLE  = L;
                        buffer[i] = PRDATA;
                    }

                    // PSEL is only set when we transition back to the idle state, which is when there are no more transactions
                    PSEL     = L;
                    
                    this->_error = (PSLVERR == H);
                    result = !this->_error; // Set the result false in case we had a bus error

                    this->transactionEnd();
                }
                else
                {
                    FATAL << "APB4 bus(" << this->id() << ") in busy state\n";
                }

                co_return result;
            }

            /**
             * @brief Perform a burst write transaction on the bus
             * @details This function will perform a write on the APB bus.
             * It will write the first data element to the passed address and then 
             * continue to write the data in the buffer in the following addresses. Which
             * will continue for the amount in burstCount.
             *
             * @param address    Start address of the burst transaction
             * @param buffer     Databuffer that holds the data to write
             * @param burstCount Number of transactions in this burst
             */
            virtual sCoRoutineHandler<bool> write(PADDR_t address, PDATA_t* buffer, unsigned burstCount=1) 
            {
                bool result = false;
                uint8_t addressOffset = sizeof(PDATA_t); // Determine the size of a single element so we can advance the address

                if(!this->busy())
                {
                    this->transactionStart();
                    this->_error = false;

                    // Perform the number of transaction passed by the user
                    for (size_t i = 0; i < burstCount; i++)
                    {
                        #ifdef DBG_BUSAPB4_H
                        DEBUG << "APB4 bus(" << this->id() << ") write " << hex << unsigned(buffer[i]) 
                                << " to address " << hex << unsigned(address + (addressOffset * i)) << '\n';
                        #endif

                        // Here we are at the IDLE phase
                        // Now we set the signals accordingly
                        PENABLE  = L;
                        PWRITE   = H;
                        PWDATA   = buffer[i]; // Loop through the passed data buffer
                        PADDR    = (address + (addressOffset * i)); // Set the address, add the offset onto the address
                        PSEL     = H;

                        // Wait for the positive edge of the clock to transition into the access phase
                        waitPosEdge(PCLK);

                        // Set PENABLE high, so that we transition into the access phase with the next clock cycle
                        PENABLE  = H;

                        waitPosEdge(PCLK);

                        // Wait until PREADY is in the right state and then transition out of the transaction
                        // PENABLE is already at the right state, since we set it at that point
                        // Since this is a single transaction it ends here.
                        while (PREADY == L) waitPosEdge(PCLK);

                        PENABLE  = L;
                    }

                    // PSEL is only set when we transition back to the idle state, which is when there are no more transactions
                    PSEL     = L;
                    
                    this->_error = (PSLVERR == H);
                    result = !this->_error; // Set the result false in case we had a bus error

                    this->transactionEnd();
                }
                else
                {
                    FATAL << "APB4 bus(" << this->id() << ") in busy state\n";
                }

                co_return result;
            }
    };
}
}

#endif
