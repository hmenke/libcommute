/*******************************************************************************
 *
 * This file is part of libcommute, a quantum operator algebra DSL and
 * exact diagonalization toolkit for C++11/14/17.
 *
 * Copyright (C) 2016-2021 Igor Krivenko <igor.s.krivenko@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 ******************************************************************************/
#ifndef LIBCOMMUTE_LOPERATOR_ELEMENTARY_SPACE_FERMION_HPP_
#define LIBCOMMUTE_LOPERATOR_ELEMENTARY_SPACE_FERMION_HPP_

#include "../algebra_ids.hpp"
#include "../metafunctions.hpp"
#include "../utility.hpp"

#include "elementary_space.hpp"

namespace libcommute {

//
// 2-dimensional elementary space generated by one fermionic degree of freedom
//

template <typename... IndexTypes>
class elementary_space_fermion : public elementary_space<IndexTypes...> {

  using base = elementary_space<IndexTypes...>;

public:
  // Value semantics
  elementary_space_fermion() = delete;
  template <typename... Args>
  explicit elementary_space_fermion(Args&&... indices)
    : base(std::forward<Args>(indices)...) {}
  elementary_space_fermion(elementary_space_fermion const&) = default;
  elementary_space_fermion(elementary_space_fermion&&) noexcept = default;
  elementary_space_fermion&
  operator=(elementary_space_fermion const&) = default;
  elementary_space_fermion&
  operator=(elementary_space_fermion&&) noexcept = default;
  ~elementary_space_fermion() override = default;

  // Make a smart pointer that manages a copy of this elementary space
  std::unique_ptr<base> clone() const override {
    return make_unique<elementary_space_fermion>(*this);
  }

  // ID of the algebra this elementary space is associated with
  int algebra_id() const override { return fermion; }

  // The minimal number of binary digits needed to represent any state
  // in this elementary space
  int n_bits() const override { return 1; }
};

namespace static_indices {

// Convenience factory function
template <typename... IndexTypes>
inline elementary_space_fermion<c_str_to_string_t<IndexTypes>...>
make_space_fermion(IndexTypes&&... indices) {
  return elementary_space_fermion<c_str_to_string_t<IndexTypes>...>(
      std::forward<IndexTypes>(indices)...);
}

} // namespace static_indices
} // namespace libcommute

#if __cplusplus >= 201703L
#include "../expression/dyn_indices.hpp"

namespace libcommute::dynamic_indices {

// Convenience factory function for dynamic indices
template <typename... IndexTypes>
inline elementary_space_fermion<dyn_indices>
make_space_fermion(IndexTypes&&... indices) {
  return elementary_space_fermion<dyn_indices>(
      dyn_indices(std::forward<IndexTypes>(indices)...));
}

} // namespace libcommute::dynamic_indices
#endif

#endif
