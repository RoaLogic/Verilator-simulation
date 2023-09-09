/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for Verilator Bus Interface                       //
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
 * @file businterface.hpp
 * @author Richard Herveille
 * @brief BusInterface base class
 * @version 0.1
 * @date 3-may-2023
 * @copyright See beginning of file
 */

#ifndef BUSINTERFACE_HPP
#define BUSINTERFACE_HPP

#define BUSINTERFACE_HPP

#include <uniqueid.hpp>
#include <tasks.hpp>

namespace RoaLogic
{
namespace bus
{
    using namespace RoaLogic::testbench::tasks;

    /**
     * @class cBusInterface
     * @author Richard Herveille
     * @brief Template base class for a bus-interface
     * @version 0.1
     * @date 3-may-2023
     *
     * @details This is a template base class for testbench bus-interfaces
     * 
     */
    template <typename addrT = unsigned long,
              typename dataT = unsigned char> class cBusInterface : public common::cUniqueId
    {
        public:
            bool _busy;
            bool _error;

            /**
             * @brief Constructor
             */
            cBusInterface() : _busy(false), _error(false) { }


            /**
             * @brief Destroy the cBusBase object
             */
            virtual ~cBusInterface() {}


            /**
             * @brief Denote start of transaction
             */
            virtual void transactionStart() { _busy=true; }


            /**
             * @brief Denote end of transaction
             */
            virtual void transactionEnd() { _busy=false; }


            /**
             * @brief Is a transaction in progress?
             *
             * @return true when a transaction is in progress
             */
            virtual bool busy() { return _busy; }


            /**
             * @brief Has current transaction completed?
             *
             * @return true when the current transaction completed
             */
            virtual bool done() { return !_busy; }


            /**
             * @brief Bus transacted terminated with/due to error?
             *
             * @return true when the bus transaction terminated with/due to an error
             */
            virtual bool error() { return _error; }


            /**
             * @brief Reset bus
             *
             * @param duration Number of cycles to assert the reset signal.
             *                 The default duration is 1 cycle
             */
            virtual clockedTask_t reset(unsigned duration=1) =0;


            /**
             * @brief Idles the bus
             *
             * @param duration Number of idle cycles
             *                 The default is 1 cycle
             */
            virtual clockedTask_t idle(unsigned duration=1) =0;


            /**
             * @brief Perform a single read transaction on the bus
             *
             * @param address     Address to read from
             * @param data        Reference to databuffer to store the data read from the bus
             */
            virtual clockedTask_t read(addrT address, dataT& data) =0;


            /**
             * @brief Perform a Burst Read Transaction on the bus
             *
             * @param address[in]    Start address of burst
             * @param burstCount[in] Lenght of burst
             * @result               Array of read data
             */
            virtual clockedTask_t burstRead(addrT address, dataT* buffer, unsigned burstCount) { co_return; }


            /**
             * @brief Perform a single write transaction on the bus
             *
             * @param address[in]  Address to write to
             * @param data[in]     Data to write
             */
            virtual clockedTask_t write(addrT address, dataT data) =0;


            /**
             * @brief Perform a Write Transaction on the bus
             *
             * @param address[in]  Start address of burst
             * @param data[in]     Pointer to Data to write
             */
            virtual clockedTask_t burstWrite(addrT address, dataT* buffer, unsigned burstCount) { co_return; }
    };
}
}

#endif
