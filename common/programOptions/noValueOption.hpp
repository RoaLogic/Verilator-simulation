/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    No value option derived from base option                        //
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

#ifndef NOVALUEOPTIONS_HPP
#define NOVALUEOPTIONS_HPP

#include <programoptions.hpp>
#include <log.hpp>

namespace RoaLogic
{
namespace common
{

    class cNoValueOption : public cOption
    {
        protected:
            void parse(cOptionName what_name, const char* value);
            void clear();
            bool _state;

        public:

        cNoValueOption(const std::string& aShortOption, 
                     const std::string& aLongOption, 
                     const std::string& aDescription, 
                     bool defaultState);

        eArgument getArgumentType();
        bool isSet();
        size_t optionCount();
    };
    
    /**
     * @brief Construct a new value option object
     * 
     * @param aShortOption the option's short name. Must be empty or one character. 
     * @param aLongOption the option's long name. Can be empty.
     * @param aDescription the Option's description that will be shown in the help message
     * @param defaultState the Option's default state
     */
    cNoValueOption::cNoValueOption( const std::string& aShortOption, 
                                const std::string& aLongOption, 
                                const std::string& aDescription, 
                                bool defaultState) :
        cOption(aShortOption, aLongOption, aDescription)
    {
        _state = defaultState;
    }

    size_t cNoValueOption::optionCount()
    {
        return 1;
    }

    /**
     * @brief Check if the option is set
     * 
     * @return true     Option is set
     * @return false    option is not set
     */
    bool cNoValueOption::isSet()
    {
        return _state;
    }

    /**
     * @brief Get the option's argument type
     * 
     * @tparam T 
     * @return eArgument    The argument type as enum
     */
    eArgument cNoValueOption::getArgumentType()
    {
        return eArgument::no;
    }

    /**
     * @brief Parse the option
     * @details Since there is nothing to parse, 
     * just set the state active, since the value is set
     * 
     * @param what_name     Name of the option
     * @param value         The value, which is ignored
     */
    inline void cNoValueOption::parse(cOptionName what_name, const char* value)
    {
        _state = true;
    }

    /**
     * @brief Clear the set option
     * 
     */
    void cNoValueOption::clear()
    {
        _state = false;
    }
}
}

#endif