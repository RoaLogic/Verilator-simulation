/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Source file for logging                                      //
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
 * @file log.cpp
 * @author Bjorn Schouteten
 * @brief 
 * @version 0.1
 * @date 9 may 2023
 * @copyright See beginning of file
 * 
 */


#include <log.hpp>
#include <iostream>

namespace RoaLogic
{
namespace common
{
    eLogPriority cLog::_logPriority = eLogPriority::Error;
    bool cLog::_initialized = false;
    bool cLog::_saveToFile = false;
    std::string cLog::_logFileName  = "";

    std::mutex cLog::_logMutex;

    uint32_t cLog::_debugFilter = 0ul;
    uint32_t cLog::_debugLevel = 0ul;

    void cLog::init(eLogPriority aPriority, std::string fileName)
    {
        if(!_initialized)
        {
            _logMutex.lock();

            if(fileName != "")
            {
                _saveToFile = true;
            }
            else
            {
                _saveToFile = false;
            }

            _logPriority = aPriority;
            _initialized = true;
            _logMutex.unlock();
        }
    }

    void cLog::logMessage(eLogPriority aPriority, std::string message)
    {
        if(_initialized)
        {
            if(aPriority >= _logPriority)
            {
                _logMutex.lock();

                if(_saveToFile)
                {

                }
                else
                {
                    std::cout << message;
                }

                _logMutex.unlock();
            }
        }
    }

    void cLog::debug(std::string message)
    {

    }
    
    void cLog::info(std::string message)
    {
        logMessage(eLogPriority::Info, "Info: " + message + "\n");
    }

    void cLog::warning(std::string message)
    {
        logMessage(eLogPriority::Warning, "Warning: " + message + "\n");
    }

    void cLog::error(std::string message)
    {
        logMessage(eLogPriority::Error, "Error: " + message + "\n");
    }

    void cLog::fatal(std::string message)
    {
        throw std::runtime_error(std::string("Fatal: ") + message + "\n");
    }
}
}



