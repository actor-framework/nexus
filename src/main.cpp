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

using std::cout;
using std::cerr;
using std::endl;

using namespace caf;

int main(int argc, char** argv) {
  actor_system_config cfg{argc, argv};
  cfg.load<io::middleman>();
  riac::add_message_types(cfg);
  actor_system system{cfg};
  uint16_t port = 0;
  auto r = message_builder(argv + 1, argv + argc).extract_opts({
    {"port,p", "bind Nexus to given port", port}
  });
  if (! r.error.empty() || r.opts.count("help") > 0 || ! r.remainder.empty())
    return cerr << r.error << std::endl << std::endl << r.helptext << endl, 1;
  auto nexus = system.spawn<riac::nexus>(false);
  auto pport = system.middleman().publish(nexus, port);
  cout << "published Nexus at port " << pport << endl;
}
