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
#ifndef LIBCOMMUTE_MONOMIAL_ACTION_SPIN_HPP_
#define LIBCOMMUTE_MONOMIAL_ACTION_SPIN_HPP_

#include "basis_space_spin.hpp"
#include "hilbert_space.hpp"
#include "monomial_action.hpp"
#include "state_vector.hpp"
#include "../expression/generator_spin.hpp"

#include <cmath>
#include <cstdint>
#include <vector>

//
// Action of a monomial comprised of spin algebra generators
//

#ifndef LIBCOMMUTE_SPIN_MAX_NUM_PRECOMPUTED_SQRT
// Maximum allowed size of `sqr_roots` in monomial_action<spin>
#define LIBCOMMUTE_SPIN_MAX_NUM_PRECOMPUTED_SQRT 128
#endif

namespace libcommute {

template<> class monomial_action<spin> {

  //
  // Calculations in this class are perfomed using the shifted magnetic quantum
  // number n = m + s, n = 0,...,2*s
  //

  // Update of a single spin mode
  struct single_spin_update_t {
    // Spin times 2
    sv_index_type s2;

    // The bit range corresponding to one spin mode is selected as
    // (in_index >> shift) & mask
    int shift;
    sv_index_type mask;

    // Generator: +, - or z
    spin_component c;

    // Power of generator
    sv_index_type power;
  };

  // List of single-boson updates
  std::vector<single_spin_update_t> updates_;

  // Precomputed square roots of integers
  std::vector<double> sqr_roots_;

  inline double sqr_root(sv_index_type n) const {
    if(n < LIBCOMMUTE_SPIN_MAX_NUM_PRECOMPUTED_SQRT)
      return sqr_roots_[n];
    else
      return std::sqrt(n);
  }

public:

  template<typename... IndexTypes>
  monomial_action(detail::monomial_range_t<IndexTypes...> const& m_range,
                  hilbert_space<IndexTypes...> const& hs) {
    sv_index_type sqr_roots_size = 0;

    auto it = m_range.first;
    auto next_it = it; ++next_it;
    auto end_it = m_range.second;

    sv_index_type power = 1;
    for(;it != end_it; ++it, ++next_it) {
      if(!is_spin(*it))
        throw unknown_generator<IndexTypes...>(*it);

      if(next_it == end_it || *next_it != *it) {
        auto const& g = dynamic_cast<generator_spin<IndexTypes...> const&>(*it);
        double spin = g.spin();

        basis_space_spin<IndexTypes...> bs(spin, g.indices());
        if(!hs.has(bs))
          throw unknown_generator<IndexTypes...>(g);

        bit_range_t const& bit_range = hs.bit_range(bs);
        int shift = bit_range.first;
        int n_bits = bit_range.second - bit_range.first + 1;

        sv_index_type ss = g.multiplicity() % 2 == 0 ?
                           (spin+0.5)*(spin+0.5) :
                           spin*(spin+1);
        sqr_roots_size = std::max(sqr_roots_size, ss + 1);

        updates_.emplace_back(single_spin_update_t{
          sv_index_type(2*spin),
          shift,
          (sv_index_type(1) << n_bits) - 1,
          g.component(),
          power
        });

        power = 1;
      } else
        ++power;
    }

    sqr_roots_size = std::min(
      sqr_roots_size,
      sv_index_type(LIBCOMMUTE_SPIN_MAX_NUM_PRECOMPUTED_SQRT)
    );
    sqr_roots_.resize(sqr_roots_size);
    for(int n = 0; n < sqr_roots_size; ++n)
      sqr_roots_[n] = std::sqrt(double(n));
  }

  inline bool act(sv_index_type in_index,
                  sv_index_type & out_index,
                  double & coeff) const {

    out_index = in_index;
    for(int b = updates_.size() - 1; b >= 0; --b) {
      auto const& update = updates_[b];
      std::int64_t n = (out_index >> update.shift) & update.mask;
      switch(update.c) {
        case plus: {
          if(n + update.power > update.s2) return false;
          for(int d = 0; d < update.power; ++d)
            coeff *= sqr_root((update.s2 - (n+d))*(n+d + 1));
          out_index += update.power << update.shift;
        }
        break;
        case minus: {
          if(n - std::int64_t(update.power) < 0) return false;
          for(int d = 0; d < update.power; ++d)
            coeff *= sqr_root((update.s2 - (n-d) + 1)*(n-d));
          out_index -= update.power << update.shift;
        }
        break;
        case z: {
          if((update.s2 % 2 == 0) && n == update.s2 / 2) return false;
          coeff *= std::pow(double(n) - double(update.s2)/2, update.power);
        }
        break;
      }
    }
    return true;
  }
};

} // namespace libcommute

#endif
