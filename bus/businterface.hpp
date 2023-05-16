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
            cBusInterface() : cUniqueId() {};

            /**
             * @brief Destroy the cBusBase object
             */
            virtual ~cBusInterface() {};


            /**
             * @brief Reset bus
             *
             */
            virtual clockedTask_t reset(unsigned duration=1) =0;


            /**
             * @brief Perform a Single Read Transaction on the bus
             *
             * @param address[in] Address to read from
             * @return            Data read
             */
            virtual clockedTask_t read(addrT address, dataT& data) =0;


            /**
             * @brief Perform a Burst Read Transaction on the bus
             *
             * @param address[in]    Start address of burst
             * @param burstCount[in] Lenght of burst
             * @result               Array of read data
             */
            virtual clockedTask_t burstRead(addrT address, dataT* buffer, unsigned burstCount) =0;


            /**
             * @brief Perform a Write Transaction on the bus
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
            virtual clockedTask_t burstWrite(addrT address, dataT* buffer, unsigned burstCount) =0;
    };
}
}

#endif
