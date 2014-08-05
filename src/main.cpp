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

#include <iostream>

#include "caf/all.hpp"
#include "caf/io/all.hpp"
#include "caf/probe_event/all.hpp"
#include "caf/nexus/nexus.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "usage: nexus PORT" << std::endl;
    return 1;
  }
  caf::probe_event::announce_types();
  auto port = std::stoi(argv[1]);
  auto nexus = caf::spawn_typed<caf::nexus::nexus>();
  caf::io::typed_publish(nexus, static_cast<uint16_t>(port));
  caf::await_all_actors_done();
  caf::shutdown();
}
