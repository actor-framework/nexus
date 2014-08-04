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

#include "caf/nexus/nexus.hpp"

#include "caf/all.hpp"
#include "caf/probe_event/all.hpp"

#define CHECK_SENDER(TypeName)                                                 \
  if (!last_sender()) {                                                        \
    std::cerr << #TypeName << " received from an invalid sender" << std::endl; \
    return;                                                                    \
  }                                                                            \
  static_cast<void>(0)

#define HANDLE_UPDATE(TypeName, FieldName)                                     \
  [=](const TypeName& FieldName) {                                             \
    if (!last_sender()) {                                                      \
      std::cerr << #TypeName << " received from an invalid sender"             \
                << std::endl;                                                  \
      return;                                                                  \
    }                                                                          \
    m_data[last_sender()].FieldName = FieldName;                               \
    broadcast();                                                               \
  }

namespace caf {
namespace nexus {

void nexus::broadcast() {
  for (auto& l : m_listeners) {
    // we now for sure that l can handle last_dequeued()
    send(actor_cast<actor>(l), last_dequeued());
  }
}

void nexus::add_listener(probe_event::sink hdl) {
  if (m_listeners.insert(hdl).second) {
    monitor(hdl);
    // send initial information
    for (auto& kvp : m_data) {
      auto& data = kvp.second;
      send(hdl, data.node);
      for (auto& route : data.direct_routes) {
        send(hdl, probe_event::new_route{route, true});
      }
    }
  }
}

nexus::behavior_type nexus::make_behavior() {
  return {
    HANDLE_UPDATE(probe_event::node_info, node),
    HANDLE_UPDATE(probe_event::ram_usage, ram),
    HANDLE_UPDATE(probe_event::work_load, load),
    [=](const probe_event::new_route& route) {
      CHECK_SENDER(probe_event::new_route);
      if (route.is_direct
          && m_data[last_sender()].direct_routes.insert(route.to).second) {
        broadcast();
      }
    },
    [=](const probe_event::route_lost& route) {
      CHECK_SENDER(probe_event::new_route);
      if (m_data[last_sender()].direct_routes.erase(route.to) > 0) {
        broadcast();
      }
    },
    [=](const probe_event::new_message&) {
      // TODO: implement me
    },
    [=](const probe_event::add_listener& req) {
      add_listener(actor_cast<probe_event::sink>(req.listener));
    },
    [=](const probe_event::add_typed_listener& req) {
      add_listener(req.listener);
    }
  };
}

} // namespace nexus
} // namespace caf
