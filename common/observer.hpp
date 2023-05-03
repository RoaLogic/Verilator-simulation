/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//             Subject header file for observer pattern            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
//             Copyright (C) 2023 Roa Logic BV                     //
//             www.roalogic.com                                    //
//                                                                 //
//     This source file may be used and distributed without        //
//   restrictions, provided that this copyright statement is       //
//   not removed from the file and that any derivative work        //
//   contains the original copyright notice and the associated     //
//   disclaimer.                                                   //
//                                                                 //
//     This soure file is free software; you can redistribute      //
//   it and/or modify it under the terms of the GNU General        //
//   Public License as published by the Free Software              //
//   Foundation, either version 3 of the License, or (at your      //
//   discretion) any later versions.                               //
//   The current text of the License can be found at:              //
//   http://www.gnu.org/licenses/gpl.html                          //
//                                                                 //
//     This source file is distributed in the hope that it will    //
//   be useful, but WITHOUT ANY WARRANTY; without even the         //
//   implied warranty of MERCHANTABILITY or FITTNESS FOR A         //
//   PARTICULAR PURPOSE. See the GNU General Public License for    //
//   more details.                                                 //
//                                                                 //
/////////////////////////////////////////////////////////////////////

/**
 * @file observer.hpp
 * @author Bjorn Schouteten
 * @brief Header file for the observer class and the corresponding events
 * @version 0.1
 * @date 03-may-2023
 * 
 * @copyright See beginning of file
 */


#ifndef OBSERVER_H
#define OBSERVER_H

#include <errorcodes.hpp>

namespace RoaLogic
{
namespace common
{

    /**
     * @class cObserver
     * @author B.Schouteten
     * @brief This class holds the interface for the observer pattern
     * @version 0.1
     * @date 03-may-2023
     * 
     * @details Class holds the interface which can be subscribe to subjects
     *          Inherit from this class and implement the notify function for the corresponding events
     * 
     */
    class cObserver
    {
        public: 

        virtual eErrorCode notify(eEvent aEvent){return eErrorCode::noEvents;};
    };
}
}

#endif // OBERSERVER_H
