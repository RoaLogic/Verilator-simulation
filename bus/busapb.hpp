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
 * @file busapb.hpp
 * @author Richard Herveille
 * @brief APB Bus Interface class
 * @version 0.1
 * @date 3-may-2023
 * @copyright See beginning of file
 */

#ifndef BUSAPB_HPP
#define BUSAPB_HPP

#define BUSAPB_HPP

namespace RoaLogic
{
namespace bus
{
    /**
     * @class cBusAPB
     * @author Richard Herveille
     * @brief APB Bus Interface Class
     * @version 0.1
     * @date 3-may-2023
     *
     * @details This is a class to drive an APB4 bus in a Verilator testbench
     * 
     */
    template <typename PADDR_t = uint8_t,
              typename PDATA_t = uint8_t> class cBusAPB4 : cBusBase
    {
        private:
          uint8_t& _PCLK;
          uint8_t& _PSEL;
          uint8_t& _PENABLE;
          PADDR_t& _PADDR;
          uint8_t& _PWRITE;
          PDATA_t& _PWDATA;
          PDATA_t& _PRDATA;
          uint8_t& _PREADY;
          uint8_t& _PSLVERR;

        public:
            /**
             * @brief Construct a new cBusInterface object
             */
            cBusAPB4(uint8_t& pclk,
                     uint8_t& psel,
                     uint8_t& penable,
                     PADDR_t& paddr,
                     uint8_t& pwrite,
                     PDATA_t& pwdata,
                     PDATA_t& prdata,
                     uint8_t& pready,
                     uint8_t& pslverr) :
                     _PCLK    (pclk   ),
                     _PSEL    (psel   ),
                     _PENABLE (penable),
                     _PADDR   (paddr  ),
                     _PWRITE  (pwrite ),
                     _PWDATA  (pwdata ),
                     _PRDATA  (prdata ),
                     _PREADY  (pready ),
                     _PSLVERR (pslverr) {};


            /**
             * @brief Destroy the cBusInterface
             */
            virtual ~cBusAPB4(void) {};


            /**
             * @brief Perform a Single Read Transaction on the bus
             *
             * @param address[in] Address to read from
             * @return            Data read
             */
            virtual PDATA_t read(PADDR_t address) { return 0; };


            /**
             * @brief Perform a Burst Read Transaction on the bus
             *
             * @param address[in]    Start address of burst
             * @param burstCount[in] Lenght of burst
             * @result               Array of read data
             */
            virtual PDATA_t* burstRead(PADDR_t address, unsigned burstCount) { return NULL; }


            /**
             * @brief Perform a Write Transaction on the bus
             *
             * @param address[in]  Address to write to
             * @param data[in]     Data to write
             */
            virtual void write(PADDR_t address, PDATA_t data) {};


            /**
             * @brief Perform a Write Transaction on the bus
             *
             * @param address[in]  Start address of burst
             * @param data[in]     Pointer to Data to write
             */
            virtual void burstWrite(PADDR_t address, unsiged burstCount, PDATA_t* data) {};
    };
}
}

#endif
