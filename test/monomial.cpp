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
#include "utility.hpp"

#include <libcommute/expression/generator_fermion.hpp>
#include <libcommute/expression/generator_boson.hpp>
#include <libcommute/expression/generator_spin.hpp>
#include <libcommute/expression/monomial.hpp>

#include <vector>

using namespace libcommute;

template<typename V> void check_equality(V const& v) {
  for(size_t i1 = 0; i1 < v.size(); ++i1) {
    for(size_t i2 = 0; i2 < v.size(); ++i2) {
      CHECK((v[i1] == v[i2]) == (i1 == i2));
      CHECK((v[i1] != v[i2]) == (i1 != i2));
    }
  }
}

template<typename V> void check_less_greater(V const& v) {
  for(size_t i1 = 0; i1 < v.size(); ++i1) {
    for(size_t i2 = 0; i2 < v.size(); ++i2) {
      CHECK((v[i1] < v[i2]) == (i1 < i2));
      CHECK((v[i1] > v[i2]) == (i1 > i2));
    }
  }
}

TEST_CASE("Monomials", "[monomial]") {

  // Setup
  using gen_type = generator<std::string, int>;
  using mon_type = monomial<std::string, int>;

  auto Cdag_dn = make_fermion(true, "dn", 0);
  auto A_y = make_boson(false, "y", 0);
  auto Sp_i = make_spin(spin_component::plus, "i", 0);
  auto S1z_j = make_spin(1, spin_component::z, "j", 0);

  std::vector<gen_type*> basis_gens = {&Cdag_dn, &A_y, &Sp_i, &S1z_j};
  std::vector<mon_type> monomials;

  // Monomial of order 0
  monomials.emplace_back();
  CHECK(monomials.back().size() == 0);

  // Monomial of order 1
  for(auto p0 : basis_gens)
    monomials.emplace_back(mon_type{p0});
  CHECK(monomials.back().size() == 1);
  CHECK(monomials.back() == mon_type(S1z_j));

  // Monomial of order 2
  for(auto p0 : basis_gens)
    for(auto p1 : basis_gens)
      monomials.emplace_back(mon_type{p0, p1});
  CHECK(monomials.back().size() == 2);
  CHECK(monomials.back() == mon_type(S1z_j, S1z_j));

  // Monomials of order 3
  for(auto p0 : basis_gens)
    for(auto p1 : basis_gens)
      for(auto p2 : basis_gens)
        monomials.emplace_back(mon_type{p0, p1, p2});
  CHECK(monomials.back().size() == 3);
  CHECK(monomials.back() == mon_type(S1z_j, S1z_j, S1z_j));

  // Monomials of order 4
  for(auto p0 : basis_gens)
    for(auto p1 : basis_gens)
      for(auto p2 : basis_gens)
        for(auto p3 : basis_gens)
          monomials.emplace_back(mon_type{p0, p1, p2, p3});
  CHECK(monomials.back().size() == 4);
  CHECK(monomials.back() == mon_type(S1z_j, S1z_j, S1z_j, S1z_j));

  SECTION("Equality and ordering") {
    check_equality(monomials);
    check_less_greater(monomials);
  }

  SECTION("Element access") {
    mon_type m0{};
    CHECK(m0.size() == 0);

    mon_type m4(Cdag_dn, A_y, Sp_i, S1z_j);
    CHECK(m4.size() == 4);
    CHECK(m4[0] == Cdag_dn);
    CHECK(m4[1] == A_y);
    CHECK(m4[2] == Sp_i);
    CHECK(m4[3] == S1z_j);
  }

  SECTION("const_iterator") {
    mon_type m0{};
    CHECK(m0.begin() == m0.end());
    CHECK(m0.begin() >= m0.end());
    CHECK(m0.begin() <= m0.end());

    mon_type m4(Cdag_dn, A_y, Sp_i, S1z_j);
    CHECK(m4.begin() != m4.end());
    CHECK(m4.cbegin() != m4.cend());
    CHECK_FALSE(m4.begin() == m4.end());
    CHECK_FALSE(m4.cbegin() == m4.cend());
    CHECK(m4.begin() < m4.end());
    CHECK(m4.begin() <= m4.end());
    CHECK_FALSE(m4.begin() > m4.end());
    CHECK_FALSE(m4.begin() >= m4.end());

    auto it = m4.begin();
    REQUIRE(*it == Cdag_dn);
    REQUIRE(it->algebra_id() == FERMION_ALGEBRA_ID);
    REQUIRE(it[2] == Sp_i);

    CHECK(*(it + 1) == A_y);
    CHECK(*(1 + it) == A_y);
    CHECK(*(m4.end() - 1) == S1z_j);

    CHECK(it++ == m4.begin());
    CHECK(*it == A_y);
    CHECK(*(++it) == Sp_i);
    it += 1;
    CHECK(*it == S1z_j);
    CHECK(*(it--) == S1z_j);
    CHECK(*it == Sp_i);
    CHECK(*(--it) == A_y);
    it -= 1;
    CHECK(*it == Cdag_dn);

    auto it1 = m4.begin(), it2 = m4.end();
    CHECK(it2 - it1 == 4);

    using std::swap;
    swap(it1, it2);
    CHECK(it2 - it1 == -4);
    CHECK(*it2 == Cdag_dn);
  }

  SECTION("Printing") {
    mon_type m0{};
    CHECK_THAT(m0, Prints<mon_type>(""));

    mon_type m4(Cdag_dn, A_y, Sp_i, S1z_j);
    CHECK_THAT(m4, Prints<mon_type>("C+(dn,0)A(y,0)S+(i,0)S1z(j,0)"));

    mon_type m121(Cdag_dn, A_y, A_y, S1z_j);
    CHECK_THAT(m121, Prints<mon_type>("C+(dn,0)[A(y,0)]^2S1z(j,0)"));

    mon_type m22(Cdag_dn, Cdag_dn, S1z_j, S1z_j);
    CHECK_THAT(m22, Prints<mon_type>("[C+(dn,0)]^2[S1z(j,0)]^2"));
  }
}