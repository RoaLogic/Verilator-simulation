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
 * @file Subject.hpp
 * @author Bjorn Schouteten
 * @brief Header file for the subject class
 * @version 0.1
 * @date 03-may-2023
 * 
 * @copyright See beginning of file
 */
#ifndef SUBJECT_H
#define SUBJECT_H

#include <observer.hpp>
#include <string.h>
#include <stdint.h>
#include <errorcodes.hpp>

namespace RoaLogic
{
namespace common
{

    /**
     * @class cSubject
     * @author Bjorn Schouteten
     * @brief This class holds the subject to which an observer can subscribe
     * @version 0.1
     * @date 03-may-2023
     * 
     * @details Class holds the subject to which an observer can subscribe
     *          A class which notify's on an event, should be inherited in classes which use this functionality
     * 
     */
    class cSubject
    {
        protected:
        
        static const uint32_t _cMaxObservers = 5;
        cObserver* _observers[_cMaxObservers];        
        
        public: 

        /**
         * @brief Construct a new Subject:: Subject object
         * @details Create the array and set all values to zero
         * 
         */
        cSubject(void)
        {
            memset(_observers, 0, sizeof(cObserver)* _cMaxObservers );
        }

        /**
         * @brief Destroy the Subject:: Subject object
         * @details Reset the subscribed array back to zero
         */
        ~cSubject(void)
        {
            memset(_observers, 0, sizeof(cObserver)* _cMaxObservers );
        }

        /**
         * @brief register an observer
         * 
         * @details Register an observer for this subject
         *          Loops to the array and checks if any free spot is available
         * 
         * @param aObserver     The reference to the observer
         * @return NoMemory     No free spot in the array, couldn't subscribe
         * @return Success      Could succesfully subscribe
         */
        eErrorCode registerObserver(cObserver* aObserver)
        {
            eErrorCode result = eErrorCode::noMemory;

            if(aObserver)
            {
                for(uint8_t i = 0; (i < _cMaxObservers) && (result != eErrorCode::success); i++)
                {
                    if(!_observers[i] )
                    {
                        _observers[i] = aObserver;
                        result = eErrorCode::success;
                    }
                }
            }  

            return result;
        }

        /**
         * @brief Remove observer from array
         * 
         * @details Remove the given observer from the list
         *          Function goes through every registered element on the list 
         *          when it finds the corresponding observer the entry is set to zero
         * 
         * @param aObserver     The observer to remove
         * @return NoItemWithID The given observer is not registered
         * @return Success      Observer removed succesfully
         */
        eErrorCode removeObserver(cObserver* aObserver)
        {
            eErrorCode result = eErrorCode::noItemWithID;

            if(aObserver)
            {
                for(uint8_t i = 0; (i < _cMaxObservers) && (result != eErrorCode::success) ; i++)
                {
                    if(_observers[i] == aObserver)
                    {
                        _observers[i] = nullptr;
                        result = eErrorCode::success;
                    }
                }
            }  
            
            return result;
        }

        /**
         * @brief Notify function
         * 
         * @details Notify function for the event type
         *          Called when an event occurs, travesers through the list and calls every registerd notify function
         * 
         * @param aEvent The event what has occured
         * @return NoEvents No registered events
         * @return Success  Succesfully handled event
         */
        eErrorCode notifyObserver(eEvent aEvent)
        {
            eErrorCode result = eErrorCode::noEvents;

            for(uint8_t i = 0; i < _cMaxObservers; i++)
            {
                if(_observers[i])
                {
                    result = _observers[i]->notify(aEvent);
                }
            }

            return result;
        }
    };


}
}

#endif // SUBJECT_H
