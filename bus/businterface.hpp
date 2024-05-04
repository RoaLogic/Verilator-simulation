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
     * @author Richard Herveille, Bjorn Schouteten
     * @brief Template base class for a bus-interface
     * @version 0.1
     * @date 27-mar-2024
     *
     * @details This is a template base class for testbench bus-interfaces
     * 
     */
    template <typename addrT = unsigned long, typename dataT = unsigned char> 
    class cBusInterface : public common::cUniqueId
    {
        protected:
            bool _busy;         //!< Busy flag indicator, tells if the bus is busy or not
            bool _error;        //!< Error flag indicator, tells if the bus is in a error state

        public:

            /**
             * @brief Constructor
             */
            cBusInterface() : _busy(false), _error(false) { }

            /**
             * @brief Destroy the cBusBase object
             */
            virtual ~cBusInterface() {}

            /** @brief Start a bus transaction
             * @details This function starts a bus transaction
             * by setting the _busy flag to true. It must be called
             * at the beginning of every transaction
             */
            virtual void transactionStart() { _busy = true; }

            /** @brief End a bus transaction
             * @details This function ends a bus transaction
             * by setting the _busy flag to false. It must be called 
             * at the end of every transaction
             */
            virtual void transactionEnd() { _busy = false; }

            /**
             * @brief Check if a bus transaction is busy
             *
             * @return true when a transaction is in progress
             */
            virtual bool busy() { return _busy; }


            /**
             * @brief Check if a bus transaction is done
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
             * @brief Perform a Read Transaction on the bus
             * @details This is a interface function and must be implemented 
             * in the derived class.
             *
             * @param address[in]       Start address of burst
             * @param buffer[out]       Data read by this function
             * @param burstCount[in]    Lenght of burst
             * @result                  Array of read data
             */
            virtual sCoRoutineHandler<bool> read(addrT address, dataT* buffer, unsigned burstCount = 1) = 0;

            /**
             * @brief Perform a Write transaction on the bus
             * @details This is a interface function and must be implemented 
             * in the derived class.
             *
             * @param address[in]       Start address of burst
             * @param data[in]          Pointer to Data to write
             * @param burstCount[in]    The number of data elements to write
             */
            virtual sCoRoutineHandler<bool> write(addrT address, dataT* buffer, unsigned burstCount = 1) = 0;
    };
}
}

#endif
