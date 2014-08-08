/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2014                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENCE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef CAF_NEXUS_NEXUS_HPP
#define CAF_NEXUS_NEXUS_HPP

#include <map>

#include "caf/all.hpp"
#include "caf/probe_event/all.hpp"

namespace caf {
namespace nexus {

class nexus : public probe_event::nexus_type::base {
 public:
  behavior_type make_behavior() override;

 private:
  template<typename Data>
  void broadcast(const Data& data) {
    for (auto& l : m_listeners) {
      // we now for sure that l can handle last_dequeued()
      send(actor_cast<actor>(l), last_dequeued());
    }
  }
  void add_listener(probe_event::sink hdl);
  struct probe_data {
    probe_event::node_info node;
    optional<probe_event::ram_usage> ram;
    optional<probe_event::work_load> load;
    std::set<node_id> direct_routes;

  };
  std::map<actor_addr, probe_data> m_data;
  std::set<probe_event::sink> m_listeners;
};

} // namespace nexus
} // namespace caf

#endif // CAF_NEXUS_NEXUS_HPP
