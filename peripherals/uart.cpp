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

#include <uart.hpp>

//For logging
#include "log.hpp"
#include "tasks.hpp"

using namespace RoaLogic;
using namespace common;
using namespace testbench::clock;
using namespace testbench::tasks;
using namespace uart;

#define L (!1)
#define H (!0)

//#define DBG_UART_H

cUart::cUart(cClock* clkInput, uint8_t& TXpin, uint8_t& RXpin) :
    CLK(clkInput),
    TX(TXpin),
    RX(RXpin)
{
    // Bit time is 1 / Baudrate 
    simtime_t bitRate = (1.0 / _myBaudrate); // Make implicit double by using 1.0 instead of 1
    _numberOfClocksToWait = (bitRate/CLK->getPeriod());

    #ifdef DBG_UART_H
    DEBUG << "UART baudrate: "<< _myBaudrate << " Num clocks for single bit: " << _numberOfClocksToWait << "\n";
    #endif
}

sCoRoutineHandler<eUartErrorCode> cUart::receiveByte()
{
    // Check if the TX signal is high, else we are already receiving a byte
    eUartErrorCode result = TX ? eUartErrorCode::Succesfull : eUartErrorCode::Busy;
    uint32_t numberOfClockTicks = 0;
    uint8_t currentByte = 0;
    #ifdef DBG_UART_H
    uint64_t clockCount = 0;
    #endif
    
    if(result == eUartErrorCode::Succesfull)
    {
        // Wait for the start bit, which is indicated by a High to low transition
        do
        {
            waitPosEdge(CLK);
            #ifdef DBG_UART_H
            clockCount++;
            #endif

            if(TX == L)
            {
                numberOfClockTicks++;
            }
            else
            {
                numberOfClockTicks = 0;
            }
            
        // Make sure that the startbit is low until the middle of the sequence    
        } while (numberOfClockTicks < (_numberOfClocksToWait/2.0));

        #ifdef DBG_UART_H
        DEBUG << "UART startbit received, clockCount: " << clockCount <<  "\n";
        #endif

        // Now loop for the number of bits to receive
        for (size_t i = 0; i < _numDataBits; i++)
        {
            // Wait untill the middle of the element before reading and storing it
            // The synchornization with the middle is done during the startbit
            for (size_t y = 0; y < _numberOfClocksToWait; y++)
            {
                waitPosEdge(CLK);
                #ifdef DBG_UART_H
                clockCount++;
                #endif
            }
            
            // Read bit
            currentByte |= ((TX & 0x01 ) << i);

            #ifdef DBG_UART_H
            DEBUG << "UART Bit: " << i << " bit value: " << static_cast<int>(TX) << " ClockCount: " << clockCount << " \n";
            #endif
        }

        // if(_parityActive)
        // {
        //     for (size_t y = 0; y < _numberOfClocksToWait; y++)
        //     {
        //         waitPosEdge(CLK);
        //     }

        //     // Compare and check parity
        // }

        // Last loop is for the number of stop bits
        for (size_t i = 0; i < _numStopBits; i++)
        {
            // Wait untill the middle of the element before reading and storing it
            // The synchornization with the middle is done during the startbit
            for (size_t y = 0; y < _numberOfClocksToWait; y++)
            {
                waitPosEdge(CLK);
                #ifdef DBG_UART_H
                clockCount++;
                #endif
            }
            
            // Read bit
            if(TX == L)
            {
                result = eUartErrorCode::StopBitError;
            }
        }

        _lastDataByte = currentByte;
    }

    #ifdef DBG_UART_H
    DEBUG << "Clockcount: " << clockCount << "\n";
    #endif

    co_return result;
}