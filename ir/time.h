#pragma once

#include <cmath>
#include <ostream>
#include <istream>
#include <stdexcept>

namespace ir {

  struct Time {
    enum Unit {
      s = 0,
      ms = -3,
      us = -6,
      ns = -9,
      ps = -12
    };

    long long v;
    int magnitude;

    Time() : v(0), magnitude(ps) {}
    Time(long long value, Unit unit) {
      v = value;
      magnitude = unit;
    }

    Time operator + (Time const& o) const {
      Time rv;

      if( magnitude < o.magnitude ) {
        rv.magnitude = magnitude;
        rv.v = v + (o.v * std::pow(10, o.magnitude - magnitude));
      } else {
        rv.magnitude = o.magnitude;
        rv.v = o.v + (v * std::pow(10, magnitude - o.magnitude));
      }

      return rv;
    }

    long long value(Unit u) const {
      if( magnitude == u )
        return v;
      else if( magnitude < u )
        return v / std::pow(10, u - magnitude);
      else
        return v * std::pow(10, magnitude - u);
    }

    Time to_unit(Unit u) const {
      Time rv;
      rv.v = value(u);
      rv.magnitude = u;
      return rv;
    }
  };


  inline bool operator < (Time const& a, Time const& b) {
    return static_cast<double>(a.v) * std::pow(10.0, a.magnitude)
      < static_cast<double>(b.v) * std::pow(10.0, b.magnitude);
  }


  inline bool operator == (Time const& a, Time const& b) {
    return (a.v == b.v) && (a.magnitude == b.magnitude);
  }


  inline bool operator != (Time const& a, Time const& b) {
    return !(a == b);
  }


}

namespace std {

  inline std::ostream& operator << (std::ostream& os, ir::Time const& t) {
    os << t.v;
    switch( t.magnitude ) {
      case 0:  os << "s"; break;
      case -3: os << "ms"; break;
      case -6: os << "us"; break;
      case -9: os << "ns"; break;
      case -12: os << "ps"; break;
      default: os << "x10^" << t.magnitude << " s";
    }
    return os;
  }


  inline std::istream& operator >> (std::istream& is, ir::Time& t) {
    std::string unit;
    is >> t.v;
    is >> unit;

    if( unit == "ps" ) t.magnitude = ir::Time::ps;
    else if( unit == "ns" ) t.magnitude = ir::Time::ns;
    else if( unit == "us" ) t.magnitude = ir::Time::us;
    else if( unit == "ms" ) t.magnitude = ir::Time::ms;
    else if( unit == "s" ) t.magnitude = ir::Time::s;
    else
      throw std::runtime_error("Unknown time unit");

    return is;
  }

}

