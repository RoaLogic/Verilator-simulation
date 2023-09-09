/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for program options                               //
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
 * @file programoptions.hpp
 * @author Bjorn Schouteten
 * @brief 
 * @version 0.1
 * @date 9 may 2023
 * @copyright See beginning of file
 * 
 */

#ifndef PROGRAMOPTIONS_HPP
#define PROGRAMOPTIONS_HPP

#include <log.hpp>
#include <vector>
#include <memory>

namespace RoaLogic
{
namespace common
{

    enum class eArgument
    {
        no = 0,   // option never takes an argument
        required, // option always requires an argument
        optional  // option may take an argument
    };

    
    /// Option name type. Used in invalid_option exception.
    /**
     * unspecified: not specified
     * short_name:  The option's short name
     * long_name:   The option's long name
     */
    enum class cOptionName
    {
        unspecified,
        shortOption,
        longOption
    };


    class cOption
    {
        private:
            friend class cProgramOptions;

        protected:
            std::string _shortOption;
            std::string _longOption;
            std::string _description;

            virtual void parse(cOptionName what_name, const char* value) = 0;
            virtual void clear() = 0;

        public:

            cOption(const std::string aShortOption, 
                    const std::string aLongOption, 
                    std::string description);

            virtual ~cOption() = default;

            char getShortName();
            std::string getLongName();
            std::string getDescription();

            virtual eArgument getArgumentType() = 0;
            virtual size_t optionCount() = 0;
            virtual bool isSet() = 0;

    };

    /**
     * @brief 
     * 
     */
    class cProgramOptions
    {
        private:
        static const uint32_t _cMaxArguments = 20;

        std::string _description;
        std::vector <std::string> _unrecognizedOptions;
        std::vector <cOption*> _options;

        cOption* findOption(const std::string& aLongName);
        cOption* findOption(char aShortName);

        public:
        cProgramOptions(std::string aDescription = "");

        ~cProgramOptions() = default;

        void parse(int argc, char* argv[]);

        int add(cOption* aOption);

        inline std::string getDescription(void)
        {
            return _description;
        }

        inline std::vector<std::string>& unknownOptions()
        {
            return _unrecognizedOptions;
        }

        void printKnownOptions()
        {
            std::cout << _description << '\n';

            std::cout << "Options: \n";

            for (auto & option : _options) 
            {
                std::cout << option->getShortName() << "\t" << 
                             option->getLongName() << "\t\t" << 
                             option->getDescription() << "\n";
            }
        }


    };


}
}

#endif