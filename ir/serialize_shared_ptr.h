#pragma once

#include <memory>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost {
  namespace serialization {
    
    template<typename Archive, typename T>
    inline void save(Archive& ar,
        std::shared_ptr<T> const& pt,
        unsigned int const version) {
      const T* ptr = pt.get();
      ar << boost::serialization::make_nvp("px", ptr);
    }

    template<typename Archive, typename T>
    inline void load(Archive& ar,
        std::shared_ptr<T>& pt,
        unsigned int const version) {
    }

    template<typename Archive, typename T>
    inline void serialize(Archive& ar, std::shared_ptr<T>& pt, unsigned int const v) {
      split_free(ar, pt, v);
    }

  }
}
