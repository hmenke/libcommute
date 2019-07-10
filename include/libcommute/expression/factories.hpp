/*******************************************************************************
 *
 * This file is part of libcommute, a C++11/14/17 header-only library allowing
 * to manipulate polynomial expressions with quantum-mechanical operators.
 *
 * Copyright (C) 2016-2019 Igor Krivenko <igor.s.krivenko@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 ******************************************************************************/
#ifndef LIBCOMMUTE_FACTORIES_HPP_
#define LIBCOMMUTE_FACTORIES_HPP_

#include "../utility.hpp"

namespace libcommute {

// TODO: factories like c_dag, c, n, etc for static indices

#if __cplusplus >= 201703L
namespace dyn {
// TODO: factories like c_dag, c, n, etc for dynamic indices
}
#endif

} // namespace libcommute

#endif