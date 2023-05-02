/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Simulation Time Class for a Verilator Testbench Clock        //
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
/*!
 * @file SimTime.hpp
 * @author Richard Herveille
 * @brief Simulation time managment 
 * @version 0.1
 * @date 30-apr-2023
 * @copyright See beginning of file
 */

#ifndef SIMTIME_HPP
#define SIMTIME_HPP

//#define DBG_SIMTIME_H

#include <limits>       // std::numeric_limits
#include <iostream>

namespace RoaLogic
{
namespace testbench
{
namespace clock
{


    /**
     * @class SimTime
     * @author Richard Herveille
     * @brief Simulation Time Class for use in cClock and in cTestBench
     * @version 0.1
     * @date 30-apr-2023
     *
     * @details
     */
    class cSimtime_t
    {
        private:
        long double _realtime;  //realtime in seconds


        public:

        constexpr static long double max() { return std::numeric_limits<long double>::max(); };
 
        constexpr static long double seconds_per_minute = 60.0;
        constexpr static long double minutes_per_hour   = 60.0;
        constexpr static long double seconds_per_hour   = seconds_per_minute * minutes_per_hour;
        constexpr static long double hours_per_day      = 24.0;
        constexpr static long double minutes_per_day    = hours_per_day * minutes_per_hour;
        constexpr static long double seconds_per_day    = minutes_per_day * seconds_per_minute;
        constexpr static long double days_per_year      = 365.0;
        constexpr static long double seconds_per_year   = days_per_year * seconds_per_day;
        constexpr static long double seconds_per_Hz     = 1.0;


        //constructors
        cSimtime_t () : _realtime(0.0){};
        cSimtime_t (long double val) : _realtime(val){};

        //convert to long double
        //implicitly enables comparison functions
        operator long double() const { return _realtime; }

        //convert to frequency
        long double frequency(void) const { return (seconds_per_Hz / _realtime); }


        //output formats
        long double year   (void) const { return (_realtime / seconds_per_year  ); }
        long double day    (void) const { return (_realtime / seconds_per_day   ); }
        long double hour   (void) const { return (_realtime / seconds_per_hour  ); }
        long double minute (void) const { return (_realtime / seconds_per_minute); }
        long double s      (void) const { return _realtime;                        }
        long double ms     (void) const { return (_realtime * 1.0E3             ); }
        long double us     (void) const { return (_realtime * 1.0E6             ); }
        long double ns     (void) const { return (_realtime * 1.0E9             ); }
        long double ps     (void) const { return (_realtime * 1.0E12            ); }
        long double fs     (void) const { return (_realtime * 1.0E15            ); }
        long double as     (void) const { return (_realtime * 1.0E18            ); }

        long double PHz (void) const { return ( frequency() / 1.0E15 ); }
        long double THz (void) const { return ( frequency() / 1.0E12 ); }
        long double GHz (void) const { return ( frequency() / 1.0E9  ); }
        long double MHz (void) const { return ( frequency() / 1.0E6  ); }
        long double kHz (void) const { return ( frequency() / 1.0E3  ); }
        long double Hz  (void) const { return   frequency();            }
        long double mHz (void) const { return ( frequency() * 1.0E3  ); }
        long double uHz (void) const { return ( frequency() * 1.0E6  ); }
        long double nHz (void) const { return ( frequency() * 1.0E9  ); }
        long double pHz (void) const { return ( frequency() * 1.0E12 ); }
        long double fHz (void) const { return ( frequency() * 1.0E15 ); }


        //Overload operators
        cSimtime_t& operator+=(const cSimtime_t& rhs)
        {
            _realtime += rhs._realtime;
            return *this;
        }
/*
        friend cSimtime_t operator+(cSimtime_t lhs, const cSimtime_t& rhs)
        {
            //friends defined inside class body are inline and hidden from non-ADL lookup
            //passing lsh by value helps optimize chained a+b+c
            lhs += rhs;
            return lhs;
        }
*/
        cSimtime_t& operator-=(const cSimtime_t& rhs)
        {
            _realtime -= rhs._realtime;
            return *this;
        }
/*
        friend cSimtime_t operator-(cSimtime_t lhs, const cSimtime_t& rhs)
        {
            lhs -= rhs;
            return lhs;
        }
*/
        cSimtime_t& operator*=(const cSimtime_t& rhs)
        {
            _realtime *= rhs._realtime;
            return *this;
        }
/*
        friend cSimtime_t operator*(cSimtime_t lhs, const cSimtime_t& rhs)
        {
            lhs *= rhs;
            return lhs;
        }
*/
        cSimtime_t& operator/=(const cSimtime_t& rhs)
        {
            _realtime /= rhs._realtime;
            return *this;
        }
/*
        friend cSimtime_t operator/(cSimtime_t lhs, const cSimtime_t& rhs)
        {
            lhs /= rhs;
            return lhs;
        }
*/

        //overload <<
        friend std::ostream& operator<<(std::ostream& out, const cSimtime_t& t);
    };

    //streaming operator
    inline std::ostream& operator<<(std::ostream& out, const cSimtime_t& t)
    {
        if (t > cSimtime_t::seconds_per_year  ) { return out << t.year()   << "years";   }
        if (t > cSimtime_t::seconds_per_day   ) { return out << t.day()    << "dayss";   }
        if (t > cSimtime_t::seconds_per_hour  ) { return out << t.hour()   << "hours";   }
        if (t > cSimtime_t::seconds_per_minute) { return out << t.minute() << "minutes"; }
        if (t > 1.0                          ) { return out << t.s()      << "sec";     }
        if (t > 1.0E-3                       ) { return out << t.ms()     << "ms";      }
        if (t > 1.0E-6                       ) { return out << t.us()     << "us";      }
        if (t > 1.0E-9                       ) { return out << t.ns()     << "ns";      }
        if (t > 1.0E-12                      ) { return out << t.ps()     << "ps";      }
        if (t > 1.0E-15                      ) { return out << t.fs()     << "fs";      }
        
        return out << t.as() << "as";
    }

    /* Units
     */
    namespace units 
    {
        /* cSimtime_t
         */
        inline cSimtime_t operator""_yr     (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_year  ); }
        inline cSimtime_t operator""_year   (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_year  ); }
        inline cSimtime_t operator""_d      (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_day   ); }
        inline cSimtime_t operator""_day    (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_day   ); }
        inline cSimtime_t operator""_h      (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_hour  ); }
        inline cSimtime_t operator""_hr     (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_hour  ); }
        inline cSimtime_t operator""_hour   (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_hour  ); }
        inline cSimtime_t operator""_min    (long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_minute); }
        inline cSimtime_t operator""_minutes(long double val) { return cSimtime_t(val / cSimtime_t::seconds_per_minute); }
        inline cSimtime_t operator""_s      (long double val) { return cSimtime_t(val                                ); }
        inline cSimtime_t operator""_ms     (long double val) { return cSimtime_t(val / 1.0E3                        ); }
        inline cSimtime_t operator""_us     (long double val) { return cSimtime_t(val / 1.0E6                        ); }
        inline cSimtime_t operator""_ns     (long double val) { return cSimtime_t(val / 1.0E9                        ); }
        inline cSimtime_t operator""_ps     (long double val) { return cSimtime_t(val / 1.0E12                       ); }
        inline cSimtime_t operator""_fs     (long double val) { return cSimtime_t(val / 1.0E15                       ); }
        inline cSimtime_t operator""_as     (long double val) { return cSimtime_t(val / 1.0E18                       ); }

        inline cSimtime_t operator""_yr     (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_year  ); }
        inline cSimtime_t operator""_year   (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_year  ); }
        inline cSimtime_t operator""_d      (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_day   ); }
        inline cSimtime_t operator""_day    (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_day   ); }
        inline cSimtime_t operator""_h      (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_hour  ); }
        inline cSimtime_t operator""_hr     (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_hour  ); }
        inline cSimtime_t operator""_hour   (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_hour  ); }
        inline cSimtime_t operator""_min    (unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_minute); }
        inline cSimtime_t operator""_minutes(unsigned long long val) { return cSimtime_t( (long double)val / cSimtime_t::seconds_per_minute); }
        inline cSimtime_t operator""_s      (unsigned long long val) { return cSimtime_t( (long double)val                                ); }
        inline cSimtime_t operator""_ms     (unsigned long long val) { return cSimtime_t( (long double)val / 1.0E3                        ); }
        inline cSimtime_t operator""_us     (unsigned long long val) { return cSimtime_t( (long double)val / 1.0E6                        ); }
        inline cSimtime_t operator""_ns     (unsigned long long val) { return cSimtime_t( (long double)val / 1.0E9                        ); }
        inline cSimtime_t operator""_ps     (unsigned long long val) { return cSimtime_t( (long double)val / 1.0E12                       ); }
        inline cSimtime_t operator""_fs     (unsigned long long val) { return cSimtime_t( (long double)val / 1.0E15                       ); }
        inline cSimtime_t operator""_as     (unsigned long long val) { return cSimtime_t( (long double)val / 1.0E18                       ); }


        inline cSimtime_t operator""_PHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E15); }
        inline cSimtime_t operator""_THz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E12); }
        inline cSimtime_t operator""_GHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E9 ); }
        inline cSimtime_t operator""_MHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E6 ); }
        inline cSimtime_t operator""_KHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E3 ); }
        inline cSimtime_t operator""_Hz     (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val)         ); }
        inline cSimtime_t operator""_mHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E3 ); }
        inline cSimtime_t operator""_uHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E6 ); }
        inline cSimtime_t operator""_nHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E9 ); }
        inline cSimtime_t operator""_pHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E12); }
        inline cSimtime_t operator""_fHz    (long double val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E15); }

        inline cSimtime_t operator""_PHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E15); }
        inline cSimtime_t operator""_THz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E12); }
        inline cSimtime_t operator""_GHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E9 ); }
        inline cSimtime_t operator""_MHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E6 ); }
        inline cSimtime_t operator""_KHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) / 1.0E3 ); }
        inline cSimtime_t operator""_Hz     (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val)         ); }
        inline cSimtime_t operator""_mHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E3 ); }
        inline cSimtime_t operator""_uHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E6 ); }
        inline cSimtime_t operator""_nHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E9 ); }
        inline cSimtime_t operator""_pHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E12); }
        inline cSimtime_t operator""_fHz    (unsigned long long val) { return cSimtime_t((cSimtime_t::seconds_per_Hz / val) * 1.0E15); }

    }

}
}
}

#endif
