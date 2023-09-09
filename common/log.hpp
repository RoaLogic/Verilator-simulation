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
#include <iostream>
#include <fstream>

#define DEBUG cLog::getInstance()->log(eLogPriority::Debug)
#define LOG cLog::getInstance()->log(eLogPriority::Log)
#define INFO cLog::getInstance()->log(eLogPriority::Info)
#define WARNING cLog::getInstance()->log(eLogPriority::Warning)
#define ERROR cLog::getInstance()->log(eLogPriority::Error)
#define FATAL cLog::getInstance()->log(eLogPriority::Fatal)

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
        eLogPriority _logPriority = eLogPriority::Error;
        eLogPriority _currentMsgPriority = eLogPriority::Debug;
        std::string _logFileName = "";
        bool _initialized = false;
        bool _saveToFile = false;

        std::fstream _fileStream;
        std::mutex _logMutex;
        static cLog* _myPointer;

        cLog();

        cLog& operator=(const cLog&){ return *this; };  // assignment operator is private

        void appendStream(std::string msg);
        eLogPriority convertPriority(uint8_t prio); 

        public:

        static cLog* getInstance();

        void init(uint8_t aPriority, std::string fileName);
        void init(eLogPriority aPriority, std::string fileName);
        void close();

        cLog& log(eLogPriority aPriority);
        
        template <typename T>
        cLog& operator<<(const T& msg);
    };

    /**
     * @brief Operator overload to append to the log output
     * 
     * @tparam T        
     * @param msg       Message to put out
     * @return cLog&    Pointer to the current cLog object
     */
    template <typename T>
    cLog& cLog::operator<<(const T& msg)
    {
        if(_currentMsgPriority >= _logPriority )
        {
            std::string messageString;

            messageString += msg;

            this->appendStream(messageString);
        }

        return *this;
    }

}
}

#endif