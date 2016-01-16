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

#include <iostream>

#include "caf/all.hpp"
#include "caf/io/all.hpp"
#include "caf/riac/all.hpp"

using namespace caf;

int main(int argc, char** argv) {
  actor_system_config cfg;
  cfg.load<io::middleman>();
  actor_system system{cfg};
  uint16_t port;
  auto r = message_builder(argv + 1, argv + argc).extract_opts({
    {"nexus,n", "run nexus in server mode"},
    {"probe,p", "run as probe", port}
  });
  if (! r.error.empty() || r.opts.count("help") > 0 || ! r.remainder.empty()) {
    std::cerr << r.error << std::endl << std::endl << r.helptext << std::endl;
    return 1;
  }
  auto nexus = system.spawn<riac::nexus>(false);
  system.middleman().publish(nexus, static_cast<uint16_t>(port));
}
