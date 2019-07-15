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
#include "utility.hpp"

#include <libcommute/expression/generator_fermion.hpp>
#include <libcommute/expression/generator_boson.hpp>
#include <libcommute/expression/expression.hpp>
#include <libcommute/expression/factories.hpp>

#include <string>

using namespace libcommute;

TEST_CASE("Compound assignment/subtraction", "[minus_assign]") {
  SECTION("double") {
    auto expr_r = real::c_dag(1, "up");
    using ref_t = decltype(expr_r);

    expr_r -= real::c(2, "dn");
    CHECK_THAT(expr_r, Prints<ref_t>("1*C+(1,up) + -1*C(2,dn)"));
    expr_r -= ref_t();
    CHECK_THAT(expr_r, Prints<ref_t>("1*C+(1,up) + -1*C(2,dn)"));
    expr_r -= real::c_dag(1, "up");
    CHECK_THAT(expr_r, Prints<ref_t>("-1*C(2,dn)"));
  }
  SECTION("complex from double") {
    auto expr_c = complex::c_dag(1, "up");
    using ref_t = decltype(expr_c);

    expr_c -= real::c(2, "dn");
    CHECK_THAT(expr_c, Prints<ref_t>("(1,0)*C+(1,up) + (-1,0)*C(2,dn)"));
    expr_c -= ref_t();
    CHECK_THAT(expr_c, Prints<ref_t>("(1,0)*C+(1,up) + (-1,0)*C(2,dn)"));
    expr_c -= real::c_dag(1, "up");
    CHECK_THAT(expr_c, Prints<ref_t>("(-1,0)*C(2,dn)"));
  }
  SECTION("int_complex") {
    auto expr = c_dag<int_complex>(1, "up");
    using ref_t = decltype(expr);

    expr -= c<int_complex>(2, "dn");
    CHECK_THAT(expr, Prints<ref_t>("{1,0}*C+(1,up) + {-1,0}*C(2,dn)"));
    expr -= ref_t();
    CHECK_THAT(expr, Prints<ref_t>("{1,0}*C+(1,up) + {-1,0}*C(2,dn)"));
    expr -= c_dag<int_complex>(1, "up");
    CHECK_THAT(expr, Prints<ref_t>("{-1,0}*C(2,dn)"));
  }
}

TEST_CASE("Subtraction", "[minus]") {
  SECTION("double") {
    auto expr_r = real::c_dag(1, "up");
    using ref_t = decltype(expr_r);

    // Result type
    CHECK(std::is_same<decltype(expr_r - real::c(2, "dn")), ref_t>::value);
    CHECK(std::is_same<decltype(real::c(2, "dn") - expr_r), ref_t>::value);

    CHECK_THAT((ref_t() - ref_t()), Prints<ref_t>("0"));

    CHECK_THAT((expr_r - ref_t()), Prints<ref_t>("1*C+(1,up)"));
    CHECK_THAT((ref_t() - expr_r), Prints<ref_t>("-1*C+(1,up)"));
    CHECK_THAT((expr_r - real::c(2, "dn")),
               Prints<ref_t>("1*C+(1,up) + -1*C(2,dn)"));
    CHECK_THAT((real::c(2, "dn") - expr_r),
               Prints<ref_t>("-1*C+(1,up) + 1*C(2,dn)"));

    expr_r -= real::c(2, "dn");

    CHECK_THAT((expr_r + ref_t()), Prints<ref_t>("1*C+(1,up) + -1*C(2,dn)"));
    CHECK_THAT((ref_t() - expr_r), Prints<ref_t>("-1*C+(1,up) + 1*C(2,dn)"));
    CHECK_THAT((expr_r - real::a(0, "x")),
               Prints<ref_t>("1*C+(1,up) + -1*C(2,dn) + -1*A(0,x)"));
    CHECK_THAT((real::a(0, "x") - expr_r),
               Prints<ref_t>("-1*C+(1,up) + 1*C(2,dn) + 1*A(0,x)"));
    CHECK_THAT((expr_r - real::c_dag(1, "up")), Prints<ref_t>("-1*C(2,dn)"));
    CHECK_THAT((real::c_dag(1, "up") - expr_r), Prints<ref_t>("1*C(2,dn)"));

    CHECK_THAT(((real::c_dag(1, "up") + real::c(2, "dn")) -
                (real::c(2, "dn") + 2.0)),
               Prints<ref_t>("-2 + 1*C+(1,up)"));
  }
  SECTION("complex and double") {
    auto expr1 = complex::c_dag(1, "up");
    auto expr2 = real::c(2, "dn");
    using ref1_t = decltype(expr1);
    using ref2_t = decltype(expr2);

    // Result type
    CHECK(std::is_same<decltype(expr1 - expr2), ref1_t>::value);
    CHECK(std::is_same<decltype(expr2 - expr1), ref1_t>::value);

    CHECK_THAT((ref1_t() - ref2_t()), Prints<ref1_t>("(0,0)"));
    CHECK_THAT((ref2_t() - ref1_t()), Prints<ref1_t>("(0,0)"));

    CHECK_THAT((expr1 - ref2_t()), Prints<ref1_t>("(1,0)*C+(1,up)"));
    CHECK_THAT((ref2_t() - expr1), Prints<ref1_t>("(-1,-0)*C+(1,up)"));
    CHECK_THAT((expr2 - ref1_t()), Prints<ref1_t>("(1,-0)*C(2,dn)"));
    CHECK_THAT((ref1_t() - expr2), Prints<ref1_t>("(-1,0)*C(2,dn)"));
    CHECK_THAT((expr1 - expr2),
               Prints<ref1_t>("(1,0)*C+(1,up) + (-1,0)*C(2,dn)"));
    CHECK_THAT((expr2 - expr1),
               Prints<ref1_t>("(-1,-0)*C+(1,up) + (1,-0)*C(2,dn)"));

    expr1 -= expr2;

    CHECK_THAT((expr1 - ref2_t()),
               Prints<ref1_t>("(1,0)*C+(1,up) + (-1,0)*C(2,dn)"));
    CHECK_THAT((ref2_t() - expr1),
               Prints<ref1_t>("(-1,-0)*C+(1,up) + (1,-0)*C(2,dn)"));
    CHECK_THAT((expr1 - real::a(0, "x")),
             Prints<ref1_t>("(1,0)*C+(1,up) + (-1,0)*C(2,dn) + (-1,0)*A(0,x)"));
    CHECK_THAT((real::a(0, "x") - expr1),
           Prints<ref1_t>("(-1,-0)*C+(1,up) + (1,-0)*C(2,dn) + (1,-0)*A(0,x)"));
    CHECK_THAT((expr1 - complex::c_dag(1, "up")),
               Prints<ref1_t>("(-1,0)*C(2,dn)"));
    CHECK_THAT((complex::c_dag(1, "up") - expr1),
               Prints<ref1_t>("(1,-0)*C(2,dn)"));

    CHECK_THAT(((complex::c_dag(1, "up") + complex::c(2, "dn")) -
               (real::c(2, "dn") + 2.0)),
               Prints<ref1_t>("(-2,0) + (1,0)*C+(1,up)"));
  }
  SECTION("int_complex") {
    auto expr = c_dag<int_complex>(1, "up");
    using ref_t = decltype(expr);

    // Result type
    CHECK(std::is_same<decltype(expr - c<int_complex>(2, "dn")), ref_t>::value);
    CHECK(std::is_same<decltype(c<int_complex>(2, "dn") - expr), ref_t>::value);

    CHECK_THAT((ref_t() - ref_t()), Prints<ref_t>("{0,0}"));

    CHECK_THAT((expr - ref_t()), Prints<ref_t>("{1,0}*C+(1,up)"));
    CHECK_THAT((ref_t() - expr), Prints<ref_t>("{-1,0}*C+(1,up)"));
    CHECK_THAT((expr - c<int_complex>(2, "dn")),
               Prints<ref_t>("{1,0}*C+(1,up) + {-1,0}*C(2,dn)"));
    CHECK_THAT((c<int_complex>(2, "dn") - expr),
               Prints<ref_t>("{-1,0}*C+(1,up) + {1,0}*C(2,dn)"));

    expr -= c<int_complex>(2, "dn");

    CHECK_THAT((expr - ref_t()),
               Prints<ref_t>("{1,0}*C+(1,up) + {-1,0}*C(2,dn)"));
    CHECK_THAT((ref_t() - expr),
               Prints<ref_t>("{-1,0}*C+(1,up) + {1,0}*C(2,dn)"));
    CHECK_THAT((expr - a<int_complex>(0, "x")),
              Prints<ref_t>("{1,0}*C+(1,up) + {-1,0}*C(2,dn) + {-1,0}*A(0,x)"));
    CHECK_THAT((a<int_complex>(0, "x") - expr),
               Prints<ref_t>("{-1,0}*C+(1,up) + {1,0}*C(2,dn) + {1,0}*A(0,x)"));
    CHECK_THAT((expr - c_dag<int_complex>(1, "up")),
               Prints<ref_t>("{-1,0}*C(2,dn)"));
    CHECK_THAT((c_dag<int_complex>(1, "up") - expr),
               Prints<ref_t>("{1,0}*C(2,dn)"));

    CHECK_THAT(((c_dag<int_complex>(1, "up") + c<int_complex>(2, "dn")) -
                (c<int_complex>(2, "dn") + int_complex{2,0})),
               Prints<ref_t>("{-2,0} + {1,0}*C+(1,up)"));
  }
}