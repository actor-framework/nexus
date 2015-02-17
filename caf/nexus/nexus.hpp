/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2015                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef CAF_NEXUS_NEXUS_HPP
#define CAF_NEXUS_NEXUS_HPP

#include <map>

#include "caf/all.hpp"
#include "caf/riac/all.hpp"

namespace caf {
namespace nexus {

class nexus : public riac::nexus_type::base {
 public:
  behavior_type make_behavior() override;

 private:
  void broadcast();
  void add_listener(riac::listener_type hdl);
  std::map<actor_addr, node_id> m_probes;
  riac::probe_data_map m_data;
  std::set<riac::listener_type> m_listeners;
};

} // namespace nexus
} // namespace caf

#endif // CAF_NEXUS_NEXUS_HPP
