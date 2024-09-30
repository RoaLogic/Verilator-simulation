/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Class for Verilator Uart Interface                           //
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

#ifndef UARTINTERFACE_HPP
#define UARTINTERFACE_HPP

#define UARTINTERFACE_HPP

#include <uniqueid.hpp>
#include <tasks.hpp>
#include <clock.hpp>

namespace RoaLogic
{
namespace uart
{
    using namespace RoaLogic::testbench::clock;
    using namespace RoaLogic::testbench::tasks;

    enum class eUartErrorCode
    {
        Succesfull,
        Busy,
        NoData,
        ReceiveError,
        StopBitError
    };

    /**
     * @class cUart
     * @author Bjorn Schouteten
     * @brief Template base class for a uart
     * @version 0.1
     * @date 11-may-2024
     *
     * @details This is a class for testbench uart 
     * 
     */
    class cUart : public common::cUniqueId
    {
        private:
            cClock*   CLK;
            uint8_t&  TX;
            uint8_t&  RX;
            uint32_t _myBaudrate = 9600;
            uint32_t _numberOfClocksToWait;
            uint8_t _numDataBits = 8;
            uint8_t _lastDataByte = 0;
            uint8_t _numStopBits = 1;
            bool _parityActive = false;
//            bool _busy;         //!< Busy flag indicator, tells if the bus is busy or not
//            bool _error;        //!< Error flag indicator, tells if the bus is in a error state

        public:

            /**
             * @brief Constructor
             */
            cUart(cClock* clkInput, uint8_t& TXpin, uint8_t& RXpin);

            ~cUart()
            {

            }

            sCoRoutineHandler<eUartErrorCode> receiveByte();

            uint8_t getLastReceivedByte(void){return _lastDataByte;};

    };

    class cTestbenchUart : public cUart
    {
        public:

        cTestbenchUart(cClock* clkInput, uint8_t& TXpin, uint8_t& RXpin) :
            cUart(clkInput, TXpin, RXpin)
        {

        }

    };

    class cTelnetUart : public cUart
    {

    };
}
}

#endif
