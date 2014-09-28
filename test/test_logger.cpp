#include <gtest/gtest.h>
#include <boost/log/trivial.hpp>


TEST(logger, hello) {
  BOOST_LOG_TRIVIAL(info) << "hello world"; 
}
