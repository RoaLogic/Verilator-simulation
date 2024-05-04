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

#include <log.hpp>
#include <iostream>

namespace RoaLogic
{
namespace common
{
    cLog* cLog::_myPointer = nullptr;

    /**
     * @brief Construct a new cLog object
     * @details This function constructs the object for the singleton
     * 
     */
    cLog::cLog()
    {

    }

    /**
     * @brief Get the instance of the singleton
     * @details This function gets the instance of the logger
     * 
     * If no logger has been created, then it creates a new cLog
     * 
     * @return cLog* Pointer to the singleton object
     */
    cLog* cLog::getInstance()
    {
        if(_myPointer == nullptr)
        {
            _myPointer = new cLog();
        }

        return _myPointer;
    }

    /**
     * @brief Convert byte to eLogPriority
     * 
     * @param prio          The priority to convert
     * @return eLogPriority 
     */
    eLogPriority cLog::convertPriority(uint8_t prio)
    {
        eLogPriority returnValue;

        switch ((uint8_t)prio)
        {
        case 0:
            returnValue = eLogPriority::Debug;
            break;
        case 1:
            returnValue = eLogPriority::Log;
            break;
        case 2:
            returnValue = eLogPriority::Info;
            break;
        case 3:
            returnValue = eLogPriority::Warning;
            break;
        case 4:
            returnValue = eLogPriority::Error;
            break;
        
        default:
            returnValue = eLogPriority::Fatal;
            break;
        }

        return returnValue;
    }

    /**
     * @brief 
     * 
     * @param aPriority 
     * @param fileName 
     */
    void cLog::init(uint8_t aPriority, std::string fileName)
    {
        init(convertPriority(aPriority), fileName);
    }

    /**
     * @brief Initialize the logger class
     * @details This function initializes the logger
     * with the given priority
     * 
     * If a filename is given it will log all data to the file
     * If no file name is given it is printed to the terminal
     * 
     * @param aPriority     The log priority to run
     * @param fileName      The filename/path to store the log
     */
    void cLog::init(eLogPriority aPriority, std::string fileName)
    {
        if(!_initialized)
        {
            _logMutex.lock();

            if(fileName != "")
            {
                _saveToFile = true;
                _fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

                try
                {
                    _fileStream.open(fileName, std::fstream::out |std::fstream::app);
                }
                catch(const std::ifstream::failure e)
                {
                    throw std::runtime_error(std::string("File open failed: %s", e.what()));
                }               
            }
            else
            {
                _saveToFile = false;
            }

            _logFileName = fileName;
            _logPriority = aPriority;
            _initialized = true;
            _logMutex.unlock();
        }
    }

    /**
     * @brief Close the logger and if used close the file
     * 
     */
    void cLog::close()
    {
        if(_saveToFile)
        {
            try
            {
                _fileStream.close();
            }
            catch(const std::ifstream::failure e)
            {
                throw std::runtime_error(std::string("File open failed: %s", e.what()));
            }             
        }
    }

    /**
     * @brief Append data to the stream
     * @details This function appends data to the selected stream
     * It makes sure that the logger is initialized and checks if the 
     * currently log level is correct.
     * 
     * @param msg 
     */
    void cLog::appendStream(std::string msg)
    {
        if(_initialized)
        {
            if (_currentMsgPriority == eLogPriority::Fatal)
            {
                throw std::runtime_error(msg);
            }
            else
            {
                if(_saveToFile)
                {
                    _logMutex.lock();

                    try
                    {
                        _fileStream << msg;
                    }
                    catch(const std::ifstream::failure e)
                    {
                        throw std::runtime_error(std::string("File failure: %s", e.what()));
                    }

                    _logMutex.unlock();
                }
                else
                {
                    std::cout << msg;
                }
            }
        }
    }

    /**
     * @brief Log function 
     * @details Log function which must be appended with the corresponding log string
     * 
     * It will check and set the right priority and then print the corresponding log level string
     * 
     * @param aPriority 
     * @return cLog& 
     */
    cLog& cLog::log(eLogPriority aPriority)
    {
        if(aPriority >= _logPriority)
        {
            _currentMsgPriority = aPriority;

            switch (aPriority)
            {
            case eLogPriority::Debug   :
                appendStream("[DEBUG] ");
                break;
            case eLogPriority::Error   :
                appendStream("[ERROR] ");
                break;
            case eLogPriority::Info    :
                appendStream("[INFO] ");
                break;
            case eLogPriority::Log     :
                appendStream("[LOG] ");
                break;
            case eLogPriority::Warning :
                appendStream("[WARNING] ");
                break;   
            case eLogPriority::Fatal :
                appendStream("[FATAL] ");
                break;
            case eLogPriority::Append :
                break;

            default:
                break;
            } 
        }
        else
        {
            _currentMsgPriority = eLogPriority::Debug;
        }

        return *this;
    }

}
}



