#pragma once

#include "driver.h"

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <atomic>


namespace method {

  /** Clocked channel to observe a module during simulation.
   *
   * @tparam Derived Derived class for CRTP.
   * @tparam Socket Type of socket of targetted module. Generate with cellwrap.
   *
   * Specialize this class to observe modules during simulation in a synchronized fashion.
   * The derived class needs to implement a clocked() and a cycle() function
   * that get called to detect clock events and process data.
   *
   * - bool clocked(Socket in, Socket prev)
   * - void cycle(Socket port)
   *
   * Observer_channel launches a thread and passes data from the simulation
   * thread through a boost::lockfree::spsc_queue. For each data item the
   * cycle() function is called individually in the observer's thread.
   * */
  template<typename Derived, typename Socket>
  class Observer_channel : public Driver<Observer_channel<Derived, Socket>, Socket> {
    public:
      /** Constructor.
       *
       * Start a new thread for observation using the run() member. */
      Observer_channel()
        : Driver<Observer_channel<Derived, Socket>, Socket>(),
          m_stop(false),
          m_thread(&Observer_channel<Derived,Socket>::run,
              static_cast<Derived*>(this)) {
      }

      /** Destructor.
       *
       * Signal a stop request to the thread and wait for it to finish. */
      ~Observer_channel() {
        m_stop = true;
        m_thread.join();
      }

      /** Call back for sim::Simulation_engine. */
      void operator () (ir::Time const& t,
          sim::Runset::Module_frame this_in,
          sim::Runset::Module_frame this_out,
          sim::Runset::Module_frame this_prev) {
        Socket in = this->from_frame(this_in);
        Socket prev = this->from_frame(this_prev);

        if( static_cast<Derived*>(this)->clocked(in, prev) ) {
          LOG4CXX_TRACE(this->m_logger, "in : " << in << " -> push");
          while( !m_queue.push(in) )
            ;
        }
      }

      /** Entry point for the observer's thread.
       *
       * Polls the queue and calls Derived::cycle() for each data item.  Also
       * monitors m_stop to exit the thread on request.
       * */
      void run() {
        LOG4CXX_DEBUG(this->m_logger, "Starting observer thread");

        Socket sock;

        while( !m_stop ) {
          while( !m_stop && !m_queue.pop(sock) )
            ;

          LOG4CXX_TRACE(this->m_logger, "observing: " << sock);
          static_cast<Derived*>(this)->cycle(sock);
        }
      }

    private:
      /** Queue for inter-thread communication. */
      boost::lockfree::spsc_queue<Socket, boost::lockfree::capacity<64>> m_queue;

      /** Flag for requesting the thread to stop. */
      std::atomic_bool m_stop;

      /** Thread object. */
      std::thread m_thread;
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
