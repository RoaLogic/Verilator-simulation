/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Header file for logging                                      //
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
/**
 * @file log.hpp
 * @author Bjorn Schouteten
 * @brief 
 * @version 0.1
 * @date 9 may 2023
 * @copyright See beginning of file
 * 
 */

#ifndef LOG_HPP
#define LOG_HPP

#include <mutex>

namespace RoaLogic
{
namespace common
{
    
    enum class eLogPriority
    {
        Debug,
        Log,
        Info,
        Warning,
        Error,
        Fatal
    };

    /**
     * @brief 
     * 
     */
    class cLog
    {
        private:
        static eLogPriority _logPriority;
        static uint32_t _debugFilter;
        static uint32_t _debugLevel;
        static std::string _logFileName;
        static bool _initialized;
        static bool _saveToFile;

        static std::mutex _logMutex;

        static void logMessage(eLogPriority aPriority, std::string message);

        public:
        static void init(eLogPriority aPriority, std::string fileName);

        static void debug  (std::string message);
        static void info   (std::string message);
        static void warning(std::string message);
        static void error  (std::string message);
        static void fatal  (std::string message);


    };

}
}

#endif