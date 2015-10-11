#pragma once

#include "driver.h"

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <atomic>


namespace method {

  template<typename Base, typename Socket>
  class Observer_channel : public Driver<Observer_channel<Base, Socket>, Socket> {
    public:
      Observer_channel()
        : Driver<Observer_channel<Base, Socket>, Socket>(),
          m_stop(false),
          m_thread(&Observer_channel<Base,Socket>::run,
              static_cast<Base*>(this)) {
      }

      ~Observer_channel() {
        m_stop = true;
        m_thread.join();
      }

      void operator () (ir::Time const& t,
          sim::Runset::Module_frame this_in,
          sim::Runset::Module_frame this_out,
          sim::Runset::Module_frame this_prev) {
        Socket in = this->from_frame(this_in);
        Socket prev = this->from_frame(this_prev);

        if( static_cast<Base*>(this)->clocked(in, prev) ) {
          LOG4CXX_TRACE(this->m_logger, "in : " << in << " -> push");
          while( !m_queue.push(in) )
            ;
        }
      }

      void run() {
        LOG4CXX_DEBUG(this->m_logger, "Starting observer thread");

        Socket sock;

        while( !m_stop ) {
          while( !m_stop && !m_queue.pop(sock) )
            ;

          LOG4CXX_TRACE(this->m_logger, "observing: " << sock);
          static_cast<Base*>(this)->cycle(sock);
        }
      }

    private:
      boost::lockfree::spsc_queue<Socket, boost::lockfree::capacity<64>> m_queue;
      std::atomic_bool m_stop;
      std::thread m_thread;
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
