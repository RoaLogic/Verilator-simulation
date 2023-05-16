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
 * @file programoptions.cpp
 * @author Bjorn Schouteten
 * @brief 
 * @version 0.1
 * @date 9 may 2023
 * @copyright See beginning of file
 * 
 */


#include <programoptions.hpp>
#include <log.hpp>
#include <iostream>

namespace RoaLogic
{
namespace common
{

    cProgramOptions::cProgramOptions()
    {
        add(sProgramOption{false, false, "-h", "--help", "", "Help for all command options"});
    }

    cProgramOptions::~cProgramOptions()
    {

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
    int cProgramOptions::parse(int argc, char* argv[])
    {
        std::vector<std::string_view> args(argv + 1, argv + argc);
        int returnValue = 0;

        std::cout << "Parse: num arguments:" << argc << "\n";

        if(argc < 0)
        {
            cLog::fatal("Negative input parameters!");
        }
        else if(argc > _cMaxArguments)
        {
            cLog::fatal("Too many input parameters!");
        }

        if(argc > 1)
        {
            for(const auto& arg: args)
            {
                for(auto& option: _options)
                {
                    std::cout << "Parse: compare arg: " << arg << " Option: " << option.shortOption 
                                << " " << option.longOption << "\n";

                    if((option.shortOption == arg) || (option.longOption == arg))
                    {
                        if(option.valid)
                        {
                            cLog::fatal("Cannot use " + option.shortOption + "/" + option.longOption + " Twice!" );
                        }
                        else
                        {
                            std::cout << "Parse: found option: " << arg << "\n";
                            option.valid = true;
                            returnValue++;
                            
                            if(option.optionHasSecond)
                            {
                                option.value = *(&arg + 1);
                            }
                        }
                    }
                }
            }
        }

        return returnValue;
    }

    /**
     * @brief add a program option
     * @details Add a program option to the program option check
     * 
     * This function adds the program option into the array, which checks if the program option is 
     * set.  
     * 
     * @param aOption       The option to set
     * @return int          Result of the operation
     */
    int cProgramOptions::add(sProgramOption aOption)
    {
        _options.push_back(aOption);
    }


    int cProgramOptions::add(std::string aLongOption, std::string description, bool hasSecondOption)
    {
        sProgramOption programOption
        {
            .valid = false,
            .optionHasSecond = hasSecondOption,
            .shortOption = "",
            .longOption = aLongOption,
            .value = "",
            .description = description,
        };

        return add(programOption);
    }

    int cProgramOptions::add(std::string aShortOption, std::string aLongOption, std::string description, bool hasSecondOption)
    {
        sProgramOption programOption
        {
            .valid = false,
            .optionHasSecond = hasSecondOption,
            .shortOption = aShortOption,
            .longOption = aLongOption,
            .value = "",
            .description = description,
        };

        return add(programOption);
    }

}
}
