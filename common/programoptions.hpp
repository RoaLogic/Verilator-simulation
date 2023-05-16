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

namespace RoaLogic
{
namespace common
{

    struct sProgramOption
    {
        bool valid;
        bool optionHasSecond;
        std::string shortOption;
        std::string longOption;
        std::string value;
        std::string description;
    };
    

    /**
     * @brief 
     * 
     */
    class cProgramOptions
    {
        private:
        static const uint32_t _cMaxArguments = 20;
        std::vector <sProgramOption> _options;

        public:
        cProgramOptions();

        ~cProgramOptions();

        int parse(int argc, char* argv[]);

        int add(sProgramOption aOption);
        int add(std::string aLongOption, std::string description, bool hasSecondOption);
        int add(std::string aShortOption, std::string aLongOption, std::string description , bool hasSecondOption);

    };

}
}

#endif