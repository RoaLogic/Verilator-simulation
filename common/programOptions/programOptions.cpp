/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base source file for program options                         //
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

#include <programoptions.hpp>
#include <log.hpp>

namespace RoaLogic
{
namespace common
{
    /**
     * @brief Construct a cOption object
     * 
     * @param aShortOption  The short option
     * @param aLongOption   The long option
     * @param description   Description of the option
     */
    cOption::cOption(const std::string aShortOption, const std::string aLongOption, std::string description) :
        _shortOption(aShortOption),
        _longOption(aLongOption),
        _description(description)
    {

    }

    /**
     * @brief Get the short name of the option
     * 
     * @return char     The short name
     */
    char cOption::getShortName()
    {
        return _shortOption[0];
    }

    /**
     * @brief Get the long name of the option
     * 
     * @return std::string  The long option name
     */
    std::string cOption::getLongName()
    {
        return _longOption;
    }
    
    /**
     * @brief Get the description of the option
     * 
     * @return std::string  The description of the option
     */
    std::string cOption::getDescription()
    {
        return _description;
    }


    /**
     * @brief Construct a new c Program Options::c Program Options object
     * 
     * @param aDescription 
     */
    cProgramOptions::cProgramOptions(std::string aDescription) : 
        _description(std::move(aDescription))
    {

    }

    /**
     * @brief Find a option 
     * @details This function finds a option through it's long name
     * 
     * @param aLongName     The long name of the option to find
     * @return cOption_ptr  A shared pointer to the option
     */
    cOption* cProgramOptions::findOption(const std::string& aLongName)
    {
        cOption* optionPtr = nullptr;

        for (const auto option : _options)
        {
            if (option->getLongName() == aLongName)
            {
                optionPtr = option;
                break;
            }
        }

        return optionPtr;
    }

    /**
     * @brief Find a option
     * @details This function finds a option through it's short name
     * 
     * @param aShortName    The short name to searh for
     * @return cOption*     A shared pointer to the option
     */
    cOption* cProgramOptions::findOption(char aShortName)
    {
        cOption* optionPtr = nullptr;

        for (const auto option : _options)
        {
            if (option->getShortName() == aShortName)
            {
                optionPtr = option;
                break;
            }
        }

        return optionPtr;
    }
    
    /**
     * @brief Add a program option 
     * @details Add a program option to the list
     * 
     * @param aOption           The option to add

     * @return 0                succesfull
     * @return 1                unsuccesfull
     */
    int cProgramOptions::add(cOption* aOption)
    {
        int result = 0;
        
        for (const auto& o : _options)
        {
            if ((aOption->getShortName() != 0) && (aOption->getShortName() == o->getShortName()))
            {
                ERROR << "duplicate short option name '-" << std::string(1, aOption->getShortName()) << "'"; 
                result = 1;
            }
                
            if (!aOption->getLongName().empty() && (aOption->getLongName() == (o->getLongName())))
            {
                ERROR << "duplicate long option name '--" << aOption->getLongName() << "'";
                result = 1;
            }                
        }

        if(result == 0)
        {
            _options.push_back(aOption);
        }        

        return result;
    }

    /**
     * @brief Parse the program options
     * @details Parse the options given into the program 
     * 
     * First check if the given parameters are valid, not to much or no parameters
     * 
     * When there are parameters given, check them with the known options in this program. This is done by 
     * looping through the known parameters and the given parameters into the system. It check for double parameters.
     * 
     * Found options will be set valid and depending if it has a second parameter value this will be stored. 
     * 
     * @param argc  The number of given parameters
     * @param argv  The array of given parameters
     * @return int  The number of found parameters
     */
    void cProgramOptions::parse(int argc, char* argv[])
    {
        for (int n = 1; n < argc; ++n)
        {
            const std::string arg(argv[n]);

            if (arg.find("--") == 0)
            {
                /// long option arg
                std::string opt = arg.substr(2);
                std::string optarg;
                size_t equalIdx = opt.find('=');

                if (equalIdx != std::string::npos)
                {
                    optarg = opt.substr(equalIdx + 1);
                    opt.resize(equalIdx);
                }

                cOption* option = findOption(opt);

                if (option != nullptr)
                {
                    if (option->getArgumentType() == eArgument::no)
                    {
                        if (!optarg.empty())
                        {
                            option = nullptr;
                        }                            
                    }
                    else if (option->getArgumentType() == eArgument::required)
                    {
                        if (optarg.empty() && n < argc - 1)
                        {
                            optarg = argv[++n];
                        }                            
                    }
                }

                if (option)
                {
                    std::cout << ("Option found: " + option->getLongName() + "\t\toptarg: " + optarg.c_str() + "\n");
                    option->parse(cOptionName::longOption, optarg.c_str());
                }                    
                else
                {
                    _unrecognizedOptions.push_back(arg);
                }
                    
            }
            else if (arg.find('-') == 0)
            {
                /// short option arg
                std::string opt = arg.substr(1);
                bool unknown = false;

                for (size_t m = 0; m < opt.size(); ++m)
                {
                    char c = opt[m];
                    std::string optarg;
                    cOption* option = findOption(c);

                    if (option != nullptr)
                    {
                        if (option->getArgumentType() == eArgument::required)
                        {
                            /// use the rest of the current argument as optarg
                            optarg = opt.substr(m + 1);
                            /// or the next arg
                            if (optarg.empty() && n < argc - 1)
                            {
                                optarg = argv[++n];
                            }
                                
                            m = opt.size();
                        }
                        else if (option->getArgumentType() == eArgument::optional)
                        {
                            /// use the rest of the current argument as optarg
                            optarg = opt.substr(m + 1);
                            m = opt.size();
                        }
                        else if(option->getArgumentType() == eArgument::no)
                        {

                        }
                    }

                    if (option)
                    {
                        std::cout << ("Option found: " + option->getLongName() + "\t\toptarg: " + optarg.c_str() + "\n");
                        option->parse(cOptionName::shortOption, optarg.c_str());
                    }                                            
                    else
                    {
                        unknown = true;
                    }                        
                }
                if (unknown)
                {
                    _unrecognizedOptions.push_back(arg);
                }                    
            }
            else
            {
                _unrecognizedOptions.push_back(arg);
            }
        }

        // for (auto& opt : _options)
        // {
        //     if (!opt->is_set())
        //     {
        //         std::string option = opt->getLongName().empty() ? std::string(1, opt->getShortName()) : opt->getLongName();
        //         throw invalid_option(opt.get(), invalid_option::Error::missing_option, "option \"" + option + "\" is required");
        //     }
        // }
    }

}
}
