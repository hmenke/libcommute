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
#ifndef LIBCOMMUTE_HILBERT_SPACE_HPP_
#define LIBCOMMUTE_HILBERT_SPACE_HPP_

#include "basis_space.hpp"
#include "../expression/expression.hpp"
#include "../metafunctions.hpp"
#include "../utility.hpp"

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace libcommute {

// Range of bits in a bit string, [start;end]
using bit_range_t = std::pair<int, int>;

//
// Hilbert space as the ordered product of base spaces
//

template<typename... IndexTypes>
class hilbert_space {

  using basis_space_t = basis_space<IndexTypes...>;
  using bs_ptr_type = std::unique_ptr<basis_space_t>;

public:

  // Compare two basis_space_t objects wrapped in std::unique_ptr<T>
  struct less {
    inline bool operator()(bs_ptr_type const& p1, bs_ptr_type const& p2) const {
      return *p1 < *p2;
    }
  };

  // Helper method for one of constructors
  template<typename BSType1, typename... BSTypesTail>
  inline void constructor_impl(BSType1&& bs, BSTypesTail&&... more_bs) {
    add(std::forward<BSType1>(bs));
    constructor_impl(std::forward<BSTypesTail>(more_bs)...);
  }
  void constructor_impl() {}

  // Check if all provided types are derived from generator<IndexTypes...>
  template<typename... Types>
  using all_are_basis_spaces = all_derived_from<basis_space_t, Types...>;

public:

  //
  // Exception types
  //

  struct basis_space_exists : public std::runtime_error {
    bs_ptr_type basis_space_ptr;
    basis_space_exists(basis_space_t const& bs) :
      std::runtime_error("Basis space already exists"),
      basis_space_ptr(bs.clone())
    {}
  };

  struct basis_space_not_found : public std::runtime_error {
    bs_ptr_type basis_space_ptr;
    basis_space_not_found(basis_space_t const& bs) :
      std::runtime_error("Basis space not found"),
      basis_space_ptr(bs.clone())
    {}
  };

  struct no_default_basis_space : public std::runtime_error {
    std::unique_ptr<generator<IndexTypes...>> generator_ptr;
    inline static std::string make_what(generator<IndexTypes...> const& g) {
      std::stringstream ss;
      ss << "Generator " << g << " has no default basis_space "
            "associated with it. You have to provide a basis_space manually";
      return ss.str();
    }
    no_default_basis_space(generator<IndexTypes...> const& g) :
      std::runtime_error(make_what(g)),
      generator_ptr(g.clone())
    {}
  };

  // Construct empty space
  hilbert_space() = default;

  // Construct from individual basis spaces
  template<typename... Args,
           typename = typename std::enable_if<
              all_are_basis_spaces<Args...>::value>::type
           >
  explicit hilbert_space(Args&&... args) {
    constructor_impl(std::forward<Args>(args)...);
  }

  // Copy all basis spaces from `hs` and append those associated with
  // all generators found in `expr` and missing from `hs`.
  template<typename ScalarType>
  hilbert_space(expression<ScalarType, IndexTypes...> const& expr,
                hilbert_space const& hs = {}) : hilbert_space(hs) {
    for(auto const& m : expr) {
      for(auto const& g : m.monomial) {
        bs_ptr_type bs = g.make_basis_space();
        int n_bits = bs->n_bits();
        auto r = basis_spaces_.emplace(std::move(bs), bit_range_t(0, 0));
        if(r.second) {
          if(n_bits == 0) throw no_default_basis_space(g);
        }
      }
    }
    recompute_bit_ranges();
  }

  // Value semantics
  hilbert_space(hilbert_space const& hs) : bit_range_end_(hs.bit_range_end_) {
    for(auto const& bs : hs.basis_spaces_) {
      basis_spaces_.emplace_hint(basis_spaces_.end(),
                                 bs.first->clone(),
                                 bs.second);
    }

  }
  hilbert_space(hilbert_space&&) noexcept = default;
  hilbert_space& operator=(hilbert_space const& hs) {
    bit_range_end_ = hs.bit_range_end_;
    basis_spaces_.clear();
    for(auto const& bs : hs.basis_spaces_) {
      basis_spaces_.emplace_hint(basis_spaces_.end(),
                                 bs.first->clone(),
                                 bs.second);
    }
    return *this;
  }
  hilbert_space& operator=(hilbert_space&&) noexcept = default;

  // Equality
  inline friend bool operator==(hilbert_space const& hs1,
                                hilbert_space const& hs2) {
    using value_type = typename decltype(basis_spaces_)::value_type;
    return hs1.size() == hs2.size() &&
           std::equal(hs1.basis_spaces_.begin(),
                      hs1.basis_spaces_.end(),
                      hs2.basis_spaces_.begin(),
                      [](value_type const& v1, value_type const& v2) {
                        return *v1.first == *v2.first && v1.second == v2.second;
                      }
          );
  }
  inline friend bool operator!=(hilbert_space const& hs1,
                                hilbert_space const& hs2) {
    return !operator==(hs1, hs2);
  }

  // Append a new basis space to the ordered product
  void add(basis_space_t const& bs) {
    auto r = basis_spaces_.emplace(bs.clone(), bit_range_t(0, 0));
    if(!r.second) throw basis_space_exists(bs);
    recompute_bit_ranges();
  }

  // Is a given basis space found in this Hilbert space
  bool has(basis_space_t const& bs) const {
    return basis_spaces_.count(bs.clone()) == 1;
  }

  // Bit range spanned by a basis space
  bit_range_t bit_range(basis_space_t const& bs) const {
    auto it = basis_spaces_.find(bs.clone());
    if(it == basis_spaces_.end())
      throw basis_space_not_found(bs);
    else
      return it->second;
  }

  // Number of basis spaces
  size_t size() const { return basis_spaces_.size(); }

  // The minimal number of binary digits needed to represent any state
  // in this Hilbert space
  int total_n_bits() const { return bit_range_end_ + 1; }

private:

  // Recompute bit ranges in basis_spaces_
  void recompute_bit_ranges() {
    bit_range_end_ = -1;
    for(auto & bs : basis_spaces_) {
      int n_bits = bs.first->n_bits();
      bs.second = bit_range_t(bit_range_end_ + 1, bit_range_end_ + n_bits);
      bit_range_end_ += n_bits;
    }
  }

  // List of base spaces in the product and their corresponding bit ranges
  std::map<bs_ptr_type, bit_range_t, less> basis_spaces_;

  // End of the bit range spanned by this Hilbert space
  int bit_range_end_ = -1;
};

} // namespace libcommute

#endif
