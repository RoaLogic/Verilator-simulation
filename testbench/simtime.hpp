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
//             Copyright (C) 2024 Roa Logic BV                     //
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

#ifndef SIMTIME_HPP
#define SIMTIME_HPP

//#define DBG_SIMTIME_H

#include <limits>       // std::numeric_limits
#include <iostream>
#include <log.hpp>

namespace RoaLogic
{
namespace testbench
{
namespace clock
{
    template <typename T> class cSimtime_t;
    typedef cSimtime_t<long double> simtime_t;

    /**
     * @class SimTime
     * @author Richard Herveille
     * @brief Simulation Time Class for use in cClock and in cTestBench
     * @version 0.1
     * @date 30-apr-2023
     *
     * @details
     */
    template <typename T> class cSimtime_t
    {
        private:
        T _realtime;  //realtime in seconds

        public:

        constexpr static T max() { return std::numeric_limits<T>::max(); };
 
        constexpr static T seconds_per_minute = 60.0;
        constexpr static T minutes_per_hour   = 60.0;
        constexpr static T seconds_per_hour   = seconds_per_minute * minutes_per_hour;
        constexpr static T hours_per_day      = 24.0;
        constexpr static T minutes_per_day    = hours_per_day * minutes_per_hour;
        constexpr static T seconds_per_day    = minutes_per_day * seconds_per_minute;
        constexpr static T days_per_year      = 365.0;
        constexpr static T seconds_per_year   = days_per_year * seconds_per_day;
        constexpr static T seconds_per_Hz     = 1.0;

        //constructors
        cSimtime_t () : _realtime(0.0){};
        cSimtime_t (T val) : _realtime(val){};

        //convert to T
        //implicitly enables comparison functions
        operator T() const { return _realtime; }

        //convert to frequency
        T frequency(void) const { return (seconds_per_Hz / _realtime); }

        //output formats
        T year   (void) const { return (_realtime / seconds_per_year  ); }
        T day    (void) const { return (_realtime / seconds_per_day   ); }
        T hour   (void) const { return (_realtime / seconds_per_hour  ); }
        T minute (void) const { return (_realtime / seconds_per_minute); }
        T s      (void) const { return  _realtime;                       }
        T ms     (void) const { return (_realtime * 1.0E3             ); }
        T us     (void) const { return (_realtime * 1.0E6             ); }
        T ns     (void) const { return (_realtime * 1.0E9             ); }
        T ps     (void) const { return (_realtime * 1.0E12            ); }
        T fs     (void) const { return (_realtime * 1.0E15            ); }
        T as     (void) const { return (_realtime * 1.0E18            ); }

        T PHz    (void) const { return ( frequency() / 1.0E15         ); }
        T THz    (void) const { return ( frequency() / 1.0E12         ); }
        T GHz    (void) const { return ( frequency() / 1.0E9          ); }
        T MHz    (void) const { return ( frequency() / 1.0E6          ); }
        T kHz    (void) const { return ( frequency() / 1.0E3          ); }
        T Hz     (void) const { return   frequency();                    }
        T mHz    (void) const { return ( frequency() * 1.0E3          ); }
        T uHz    (void) const { return ( frequency() * 1.0E6          ); }
        T nHz    (void) const { return ( frequency() * 1.0E9          ); }
        T pHz    (void) const { return ( frequency() * 1.0E12         ); }
        T fHz    (void) const { return ( frequency() * 1.0E15         ); }

        //Overload operators
        cSimtime_t& operator+=(const cSimtime_t& rhs)
        {
            _realtime += rhs._realtime;
            return *this;
        }

        cSimtime_t& operator-=(const cSimtime_t& rhs)
        {
            _realtime -= rhs._realtime;
            return *this;
        }

        cSimtime_t& operator*=(const cSimtime_t& rhs)
        {
            _realtime *= rhs._realtime;
            return *this;
        }

        cSimtime_t& operator/=(const cSimtime_t& rhs)
        {
            _realtime /= rhs._realtime;
            return *this;
        }

        //overload <<
        friend std::ostream& operator<<(std::ostream& out, const simtime_t& t);
    };

    //streaming operator
    inline std::ostream& operator<<(std::ostream& out, const simtime_t& t)
    {
        if (t > simtime_t::seconds_per_year  ) { return out << t.year()   << "years";   }
        if (t > simtime_t::seconds_per_day   ) { return out << t.day()    << "days";    }
        if (t > simtime_t::seconds_per_hour  ) { return out << t.hour()   << "hours";   }
        if (t > simtime_t::seconds_per_minute) { return out << t.minute() << "minutes"; }
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
        inline simtime_t operator""_yr     (long double        val) { return simtime_t(val * simtime_t::seconds_per_year  ); }
        inline simtime_t operator""_year   (long double        val) { return simtime_t(val * simtime_t::seconds_per_year  ); }
        inline simtime_t operator""_d      (long double        val) { return simtime_t(val * simtime_t::seconds_per_day   ); }
        inline simtime_t operator""_day    (long double        val) { return simtime_t(val * simtime_t::seconds_per_day   ); }
        inline simtime_t operator""_h      (long double        val) { return simtime_t(val * simtime_t::seconds_per_hour  ); }
        inline simtime_t operator""_hr     (long double        val) { return simtime_t(val * simtime_t::seconds_per_hour  ); }
        inline simtime_t operator""_hour   (long double        val) { return simtime_t(val * simtime_t::seconds_per_hour  ); }
        inline simtime_t operator""_min    (long double        val) { return simtime_t(val * simtime_t::seconds_per_minute); }
        inline simtime_t operator""_minutes(long double        val) { return simtime_t(val * simtime_t::seconds_per_minute); }
        inline simtime_t operator""_s      (long double        val) { return simtime_t(val * 1.0                          ); }
        inline simtime_t operator""_ms     (long double        val) { return simtime_t(val / 1.0E3                        ); }
        inline simtime_t operator""_us     (long double        val) { return simtime_t(val / 1.0E6                        ); }
        inline simtime_t operator""_ns     (long double        val) { return simtime_t(val / 1.0E9                        ); }
        inline simtime_t operator""_ps     (long double        val) { return simtime_t(val / 1.0E12                       ); }
        inline simtime_t operator""_fs     (long double        val) { return simtime_t(val / 1.0E15                       ); }
        inline simtime_t operator""_as     (long double        val) { return simtime_t(val / 1.0E18                       ); }

        inline simtime_t operator""_yr     (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_year  ); }
        inline simtime_t operator""_year   (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_year  ); }
        inline simtime_t operator""_d      (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_day   ); }
        inline simtime_t operator""_day    (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_day   ); }
        inline simtime_t operator""_h      (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_hour  ); }
        inline simtime_t operator""_hr     (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_hour  ); }
        inline simtime_t operator""_hour   (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_hour  ); }
        inline simtime_t operator""_min    (unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_minute); }
        inline simtime_t operator""_minutes(unsigned long long val) { return simtime_t( (long double)val * simtime_t::seconds_per_minute); }
        inline simtime_t operator""_s      (unsigned long long val) { return simtime_t( (long double)val * 1.0                          ); }
        inline simtime_t operator""_ms     (unsigned long long val) { return simtime_t( (long double)val / 1.0E3                        ); }
        inline simtime_t operator""_us     (unsigned long long val) { return simtime_t( (long double)val / 1.0E6                        ); }
        inline simtime_t operator""_ns     (unsigned long long val) { return simtime_t( (long double)val / 1.0E9                        ); }
        inline simtime_t operator""_ps     (unsigned long long val) { return simtime_t( (long double)val / 1.0E12                       ); }
        inline simtime_t operator""_fs     (unsigned long long val) { return simtime_t( (long double)val / 1.0E15                       ); }
        inline simtime_t operator""_as     (unsigned long long val) { return simtime_t( (long double)val / 1.0E18                       ); }

        inline simtime_t operator""_PHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E15)); }
        inline simtime_t operator""_THz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E12)); }
        inline simtime_t operator""_GHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E9 )); }
        inline simtime_t operator""_MHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E6 )); }
        inline simtime_t operator""_KHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E3 )); }
        inline simtime_t operator""_Hz     (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0   )); }
        inline simtime_t operator""_mHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E3 )); }
        inline simtime_t operator""_uHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E6 )); }
        inline simtime_t operator""_nHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E9 )); }
        inline simtime_t operator""_pHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E12)); }
        inline simtime_t operator""_fHz    (long double        val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E15)); }

        inline simtime_t operator""_PHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E15)); }
        inline simtime_t operator""_THz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E12)); }
        inline simtime_t operator""_GHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E9 )); }
        inline simtime_t operator""_MHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E6 )); }
        inline simtime_t operator""_KHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0E3 )); }
        inline simtime_t operator""_Hz     (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val * 1.0   )); }
        inline simtime_t operator""_mHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E3 )); }
        inline simtime_t operator""_uHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E6 )); }
        inline simtime_t operator""_nHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E9 )); }
        inline simtime_t operator""_pHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E12)); }
        inline simtime_t operator""_fHz    (unsigned long long val) { return simtime_t(simtime_t::seconds_per_Hz / (val / 1.0E15)); }
    }

}
}
}

#endif
