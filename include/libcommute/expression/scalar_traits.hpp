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
#ifndef LIBCOMMUTE_SCALAR_TRAITS_HPP_
#define LIBCOMMUTE_SCALAR_TRAITS_HPP_

#include <complex>
#include <limits>
#include <type_traits>

namespace libcommute {

// Metafunction to detect complex types
template<typename T> struct is_complex : std::false_type {};
template<typename T> struct is_complex<std::complex<T>> : std::true_type {};

// Traits of types that can be used as the ScalarType parameter of `expression`.
// User-defined scalar types need to spcialize this structure.
template<typename S, typename = void> struct scalar_traits {};

// Enable template instantiation if Trait<T>::value is true
template<template<typename> class Trait, typename T>
using with_trait = typename std::enable_if<Trait<T>::value>::type;

//
// Integral types
//
template<typename S>
struct scalar_traits<S, with_trait<std::is_integral, S>> {
  // Zero value test
  static bool is_zero(S const& x) { return x == 0; }
  // Real part of x
  static S real(S const& x) { return x; }
  // Imaginary part of x
  static S imag(S const& x) { return S{}; }
  // Complex conjugate of x
  static S conj(S const& x) { return x; }
};

//
// Floating point types
//
template<typename S>
struct scalar_traits<S, with_trait<std::is_floating_point, S>> {
  // Zero value test
  static bool is_zero(S const& x) {
    return std::abs(x) < 100 * std::numeric_limits<S>::epsilon();
  }
  // Real part of x
  static S real(S const& x) { return x; }
  // Imaginary part of x
  static S imag(S const& x) { return S{}; }
  // Complex conjugate of x
  static S conj(S const& x) { return x; }
};

//
// Complex types
//
template<typename S>
struct scalar_traits<S, with_trait<is_complex, S>> {
  // Zero value test
  static bool is_zero(S const& x) {
    return is_zero(std::real(x)) && is_zero(std::imag(x));
  }
  // Real part of x
  static S real(S const& x) { return std::real(x); }
  // Imaginary part of x
  static S imag(S const& x) { return std::imag(x); }
  // Complex conjugate of x
  static S conj(S const& x) { return std::conj(x); }
};

} // namespace libcommute

#endif
