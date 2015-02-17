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

#include "caf/nexus/nexus.hpp"

#include <iostream>

#include "caf/all.hpp"
#include "caf/riac/all.hpp"

using std::cout;
using std::cerr;
using std::endl;

#define CHECK_SOURCE(TypeName, VarName)                                        \
  if (VarName.source_node == caf::invalid_node_id) {                           \
    cerr << #TypeName << " received with invalid source node" << endl;         \
    return;                                                                    \
  } else {                                                                     \
    cout << "received " << #TypeName << endl;                                  \
  }                                                                            \
  static_cast<void>(0)

#define HANDLE_UPDATE(TypeName, FieldName)                                     \
  [=](const TypeName& FieldName) {                                             \
    if (FieldName.source_node == caf::invalid_node_id) {                       \
      cerr << #TypeName << " received with invalid source node" << endl;       \
      return;                                                                  \
    }                                                                          \
    cout << "received " << #TypeName << endl;                                  \
    m_data[FieldName.source_node].FieldName = FieldName;                       \
    broadcast();                                                      \
  }

namespace {

  std::string format_down_msg(const std::string& type, const caf::down_msg& dm){
    std::stringstream ds;
    ds << type << " "
       << to_string(dm.source) << " exited with reason "
       << dm.reason;
    return ds.str();
  }

} // namespace <anonymous>


namespace caf {
namespace nexus {

void nexus::add_listener(riac::listener_type hdl) {
  if (m_listeners.insert(hdl).second) {
    cout << "new listener: "
         << to_string(actor_cast<actor>(hdl))
         << endl;
    monitor(hdl);
    send(hdl, m_data);
  }
}

void nexus::broadcast() {
  for (auto& l : m_listeners) {
    // we now for sure that l can handle last_dequeued()
    send(actor_cast<actor>(l), last_dequeued());
  }
}

nexus::behavior_type nexus::make_behavior() {
  return {
    [=](const riac::node_info& ni) {
      if (ni.source_node == caf::invalid_node_id) {
        cerr << "node_info received with invalid source node" << endl;
        return;
      }
      cout << "received node_info " << endl;
      m_data[ni.source_node].node = ni;
      auto& ls = last_sender();
      m_probes[ls] = ls.node();
      monitor(ls);
      broadcast();
    },
    HANDLE_UPDATE(riac::ram_usage, ram),
    HANDLE_UPDATE(riac::work_load, load),
    [=](const riac::new_actor_published& msg) {
      CHECK_SOURCE(riac::actor_published, msg);
      auto addr = msg.published_actor;
      auto nid = msg.source_node;
      if (addr == invalid_actor_addr) {
        cerr << "received riac::actor_published "
             << "with invalid actor address"
             << endl;
        return;
      }
      if (m_data[nid].known_actors.insert(addr).second) {
        monitor(addr);
      }
      m_data[nid].published_actors.insert(std::make_pair(addr, msg.port));
      broadcast();
    },
    [=](const riac::new_route& route) {
      CHECK_SOURCE(riac::new_route, route);
      if (route.is_direct
          && m_data[route.source_node].direct_routes.insert(route.dest).second) {
        broadcast();
      }
    },
    [=](const riac::route_lost& route) {
      CHECK_SOURCE(riac::route_lost, route);
      if (m_data[route.source_node].direct_routes.erase(route.dest) > 0) {
        cout << "new route" << endl;
        broadcast();
      }
    },
    [=](const riac::new_message& msg) {
      // TODO: reduce message size by avoiding the complete msg
      CHECK_SOURCE(riac::new_message, msg);
      cout << "new message" << endl;
      broadcast();
    },
    [=](const riac::add_listener& req) {
      //cout << "new listerner" << endl;
      add_listener(actor_cast<riac::listener_type>(req.listener));
    },
    [=](const riac::add_typed_listener& req) {
      add_listener(req.listener);
    },
    [=](const down_msg& dm) {
      if (m_listeners.erase(actor_cast<riac::listener_type>(dm.source)) > 0) {
        cout << format_down_msg("listener", dm) << endl;
        return;
      }
      auto probe_addr = m_probes.find(dm.source);
      if (probe_addr != m_probes.end()) {
        cout << format_down_msg("probe", dm) << endl;
        riac::node_disconnected nd{probe_addr->second};
        send(this, nd);
        auto i = m_data.find(probe_addr->second);
        if (i != m_data.end()
            && i->second.known_actors.erase(probe_addr->first) > 0) {
          return;
        }
      }
    },
    [=](const riac::node_disconnected& nd) {
      m_data.erase(nd.source_node);
      broadcast();
    }
  };
}

} // namespace nexus
} // namespace caf
