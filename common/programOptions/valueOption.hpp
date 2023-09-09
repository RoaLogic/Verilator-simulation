/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Value option derived from base option                        //
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
 * @file valueoptions.hpp
 * @author Bjorn Schouteten
 * @brief 
 * @version 0.1
 * @date 22 may 2023
 * @copyright See beginning of file
 * 
 */

#ifndef VALUEOPTIONS_HPP
#define VALUEOPTIONS_HPP

#include <programoptions.hpp>
#include <sstream>
#include <string.h>
#include <log.hpp>

namespace RoaLogic
{
namespace common
{

    template <class T>
    class cValueOption : public cOption
    {
        protected:
            std::unique_ptr<T> _defaultValue;
            T* _assignedPointer;
            std::vector<T> _value;

            void parse(cOptionName what_name, const char* value);
            void clear();

        public:

            cValueOption(const std::string& aShortOption, 
                        const std::string& aLongOption, 
                        const std::string& aDescription);

            cValueOption(const std::string& aShortOption, 
                        const std::string& aLongOption, 
                        const std::string& aDescription, 
                        const T& defaultValue, 
                        T* assignedPointer = nullptr);

            eArgument getArgumentType();
            size_t optionCount();
            bool isSet();

            void setValue(const T& value);
            T valueOr(const T& defaultValue, size_t index = 0);
            T value(size_t index = 0);

            void setDefault(const T& value);
            bool hasDefault();

            T getDefault();
            bool getDefault(std::ostream& out);
    };

    /**
     * @brief Construct a new value option object
     * 
     * @param aShortOption the option's short name. Must be empty or one character. 
     * @param aLongOption the option's long name. Can be empty.
     * @param description the Option's description that will be shown in the help message
     */
    template <class T>
    cValueOption<T>::cValueOption( const std::string& aShortOption, 
                                const std::string& aLongOption, 
                                const std::string& aDescription) :
        cOption(aShortOption, aLongOption, aDescription)         
    {
        
    }

    
    /**
     * @brief Construct a new value option object
     * 
     * @param aShortOption the option's short name. Must be empty or one character. 
     * @param aLongOption the option's long name. Can be empty.
     * @param aDescription the Option's description that will be shown in the help message
     * @param defaultValue the Option's default value
     * @param assignedPointer pointer to a variable to assign the parsed command line value to
     */
    template <class T>
    cValueOption<T>::cValueOption( const std::string& aShortOption, 
                                const std::string& aLongOption, 
                                const std::string& aDescription, 
                                const T& defaultValue, 
                                T* assignedPointer ) :
        cOption(aShortOption, aLongOption, aDescription)
    {
        _assignedPointer = assignedPointer;
        setDefault(defaultValue);
    }

    /**
     * @brief Get the count of the values
     * 
     * @tparam T 
     * @return size_t The number of arguments in the list
     */
    template <class T>
    size_t cValueOption<T>::optionCount()
    {
        return _value.size();
    }

    /**
     * @brief Check if a value is set
     * 
     * @tparam T 
     * @return true     A value is set
     * @return false    No values are set
     */
    template <class T>
    bool cValueOption<T>::isSet()
    {
        return !_value.empty();
    }

    /**
     * @brief Get the Option's value, return default_value if not set.
     * 
     * @tparam T 
     * @param default_value return value if value is not set
     * @param index the zero based index of the value (if set multiple times)
     * @return T the Option's value at index "idx" or the default value or default_value
     */
    template <class T>
    T cValueOption<T>::valueOr(const T& defaultValue, size_t index)
    {
        T returnValue;
        if (index < _value.size())
        {
            returnValue = _value[index];
        }            
        else if (_defaultValue)
        {
            returnValue = *_defaultValue;
        }
        else
        {
            returnValue = _defaultValue;
        }
        
        return returnValue;
    }

    /**
     * @brief Get the Option's value. Will throw if option at index idx is not available
     * 
     * @tparam T 
     * @param index the zero based index of the value (if set multiple times)
     * @return T the Option's value at index "index"
     */
    template <class T>
    T cValueOption<T>::value(size_t idx)
    {
        T returnValue;

        if (!this->isSet() && _defaultValue)
        {
            returnValue = *_defaultValue;
        }
        else if(!isSet() || (idx >= optionCount()))
        {
            std::string message;
            if (!isSet())
            {
                message.append("option not set: ");
            }                
            else
            {
                message.append("index out of range ( " + idx );
                message.append(" ) for ");
            }                

            if (getShortName() != 0)
            {
                message.append("-" + getShortName());
            }                
            else
            {
                message.append("--" + getLongName());
            }               

            FATAL << message;
        }
        else
        {
            returnValue =  _value[idx];
        }

        return returnValue;
    }

    /**
     * @brief Set the Option's default value
     * 
     * @tparam T 
     * @param value the default value if not specified on command line
     */
    template <class T>
    void cValueOption<T>::setDefault(const T& value)
    {
        this->_defaultValue.reset(new T);
        *this->_defaultValue = value;
    }

    /**
     * @brief Check if the Option has a default value
     * 
     * @tparam T 
     * @return true if the Option has a default value
     */
    template <class T>
    bool cValueOption<T>::hasDefault()
    {
        return (this->_defaultValue != nullptr);
    }

    /**
     * @brief Get the Option's default value. Will throw if no default is set.
     * 
     * @tparam T 
     * @return the Option's default value
     */
    template <class T>
    T cValueOption<T>::getDefault()
    {
        if (!hasDefault())
        {
            FATAL << ("no default value set");
        }
            
        return *this->_defaultValue;
    }

    /**
     * @brief Get the Option's default value and print it on the given outstream
     * 
     * @tparam T 
     * @param out   The output stream to print the value on
     * @return true When option has default value
     */
    template <class T>
    bool cValueOption<T>::getDefault(std::ostream& out)
    {
        bool returnValue = false;
        if (hasDefault())
        {
            out << *this->_defaultValue;
            returnValue = true;
        } 
        
        return returnValue;
    }

    /**
     * @brief Get the option's argument type
     * 
     * @tparam T 
     * @return eArgument    The argument type as enum
     */
    template <class T>
    eArgument cValueOption<T>::getArgumentType()
    {
        return eArgument::required;
    }

    
    template <>
    inline void cValueOption<std::string>::parse(cOptionName what_name, const char* value)
    {
        if (strlen(value) == 0)
        {
            FATAL << ("no default value set");
        }        
        _value.push_back(value);
    }


    template <>
    inline void cValueOption<bool>::parse(cOptionName /*what_name*/, const char* value)
    {
        bool val = false;

        if(value != nullptr)
        {
            val =  ((strcmp(value, "1") == 0) || (strcmp(value, "true") == 0) || (strcmp(value, "True") == 0) || (strcmp(value, "TRUE") == 0));
        }

        _value.push_back(val);
    }


    template <class T>
    inline void cValueOption<T>::parse(cOptionName what_name, const char* value)
    {
        size_t parsed_value;
        std::string strValue;
        if (value != nullptr)
        {
            strValue = value;
        }        

        std::stringstream ss(strValue);
        int valuesRead = 0;

        while (ss.good())
        {
            if (ss.peek() != EOF)
            {
                ss >> parsed_value;
            }                
            else
            {
                break;
            }              

            valuesRead++;
        }

        if (ss.fail())
        {
            if(what_name == cOptionName::shortOption)
            {
                FATAL << "Invalid argument for: " << this->getShortName();
            }
            else
            {
                FATAL << "Invalid argument for: " << this->getLongName();
            }
        }           

        _value.push_back(parsed_value);
    }

    template <class T>
    void cValueOption<T>::clear()
    {
        _value.clear();
    }
}
}

#endif