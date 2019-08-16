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

#include "catch2/catch.hpp"

#include "int_complex.hpp"

#include <libcommute/expression/hc.hpp>
#include <libcommute/expression/factories.hpp>

using namespace libcommute;
using namespace static_indices;

TEST_CASE("Hermitian conjugate object (double)", "[hc_double]") {
  using namespace real;
  auto expr = 2 * c_dag("up", 1) * c("up", 2);
  CHECK((expr + hc) == (expr + conj(expr)));
  CHECK((expr - hc) == (expr - conj(expr)));
}

TEST_CASE("Hermitian conjugate object (complex)", "[hc_complex]") {
  using namespace complex;
  auto expr = std::complex<double>(2, 2) * c_dag("up", 1) * c("up", 2);
  CHECK((expr + hc) == (expr + conj(expr)));
  CHECK((expr - hc) == (expr - conj(expr)));
}

TEST_CASE("Hermitian conjugate object (int_complex)", "[hc_int_complex]") {
  using namespace complex;
  auto expr = int_complex(2,2) * c_dag<int_complex>("up", 1)
                               * c<int_complex>("up", 2);
  CHECK((expr + hc) == (expr + conj(expr)));
  CHECK((expr - hc) == (expr - conj(expr)));
}