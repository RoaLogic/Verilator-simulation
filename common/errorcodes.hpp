/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//             Error code enum class                               //
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
 * @file errorcodes.h
 * @author Bjorn Schouteten
 * @brief Header file for all possible errorcodes
 * @version 0.1
 * @date 03-may-2023
 * 
 * @copyright See beginning of file
 */
#ifndef ERRORCODES_H
#define ERRORCODES_H

namespace RoaLogic
{
namespace common
{

    enum class eErrorCode
    {
        success = 0,                ///< The operation was successful
        noEvents,                   ///< No registered events
        noItemWithID,               ///< No item registered with this ID
        invalidState,               ///< The operation cannot be performed in this state
        noMemory,                   ///< Not enough memory left
        unInitialized,              ///< Component not initialized
        initialized,                ///< Component already initialized

        unspecifiedError = 250      ///< None of the other error messages accurately describe the issue
    };


    enum class eEvent
    {
        risingEdge,
        fallingEdge
    };

}
}

#endif // ERRORCODES_H
