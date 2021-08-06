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
#ifndef LIBCOMMUTE_LOPERATOR_ELEMENTARY_SPACE_BOSON_HPP_
#define LIBCOMMUTE_LOPERATOR_ELEMENTARY_SPACE_BOSON_HPP_

#include "../algebra_ids.hpp"
#include "../metafunctions.hpp"
#include "../utility.hpp"

#include <cassert>
#include "elementary_space.hpp"

namespace libcommute {

//
// 2^n-dimensional elementary space generated by one bosonic degree of freedom
//

template<typename... IndexTypes>
class elementary_space_boson : public elementary_space<IndexTypes...> {

  using base = elementary_space<IndexTypes...>;

public:

  // Value semantics
  elementary_space_boson() = delete;
  template<typename... Args>
  elementary_space_boson(int n_bits, Args&&... indices)
    : base(std::forward<Args>(indices)...), n_bits_(n_bits) {
  }
  elementary_space_boson(elementary_space_boson const&) = default;
  elementary_space_boson(elementary_space_boson&&) noexcept = default;
  elementary_space_boson& operator=(elementary_space_boson const&) = default;
  elementary_space_boson& operator=(elementary_space_boson&&) noexcept
    = default;
  ~elementary_space_boson() override = default;

  // Make a smart pointer that manages a copy of this elementary space
  std::unique_ptr<base> clone() const override {
    return make_unique<elementary_space_boson>(*this);
  }

  // ID of the algebra this elementary space is associated with
  int algebra_id() const override { return boson; }

  // The minimal number of binary digits needed to represent any state
  // in this elementary space
  int n_bits() const override { return n_bits_; }

private:

  // This space is spanned by bosonic states |0>, |1>, ..., |2^{n_bits}-1>
  int n_bits_;
};

namespace static_indices {

// Convenience factory function
template<typename... IndexTypes>
inline elementary_space_boson<c_str_to_string_t<IndexTypes>...>
make_space_boson(int n_bits, IndexTypes&&... indices) {
  return {n_bits, std::forward<IndexTypes>(indices)...};
}

} // namespace libcommute::static_indices
} // namespace libcommute

#if __cplusplus >= 201703L
#include "../expression/dyn_indices.hpp"

namespace libcommute {
namespace dynamic_indices {

// Convenience factory function for dynamic indices
template<typename... IndexTypes>
inline elementary_space_boson<dyn_indices>
make_space_boson(int n_bits, IndexTypes&&... indices) {
  assert(n_bits > 0);
  return {n_bits, dyn_indices(std::forward<IndexTypes>(indices)...)};
}

} // namespace libcommute::dynamic_indices
} // namespace libcommute
#endif

#endif
