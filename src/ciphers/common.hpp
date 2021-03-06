#pragma once

#include "ciphey/ciphers.hpp"

#include <atomic>
#include <thread>
#include <future>

namespace ciphey::detail {
  /// Used for vigenere-style extensions on monosubstitution ciphers
  template <typename Key, typename BaseKey, auto* CrackOne, typename... CrackArgs>
  struct reducer {
    // We use a shared_future so that a copy is unnecessary
    using intermediate_res_t = std::vector<std::vector<crack_result<BaseKey>>>;

    inline static void reduce(std::vector<crack_result<Key>>& v, intermediate_res_t& imdt, prob_t p_value,
                              std::vector<crack_result<BaseKey> const*>& prev_pos, size_t pos = 0) {
      auto& target = imdt[pos];

      // At the bottom layer of recursion, we fill in all the values
      if (pos == prev_pos.size()) {
         crack_result<Key> base_candidate{.key = Key(imdt.size()), .p_value = 1};
        for (size_t i = 0; i < pos; ++i) {
          auto& part = *prev_pos[i];
          base_candidate.key[i] = part.key;
          base_candidate.p_value *= part.p_value;
        }
        for (auto& final_key_part : target) {
          auto candidate_p_value = base_candidate.p_value * final_key_part.p_value;
          if (candidate_p_value < p_value)
            continue;
          auto& back = v.emplace_back(crack_result<Key>{ .key = base_candidate.key });
          back.key.back() = final_key_part.key;
          back.p_value = candidate_p_value;
        }
      }
      else {
        for (auto& i : target) {
          prev_pos[pos] = &i;
          reduce(v, imdt, p_value, prev_pos, pos + 1);
        }
      }
    }

    static inline std::vector<crack_result<Key>> crack(windowed_prob_table observed, prob_table const& expected,
                                                       freq_t count, prob_t p_value, CrackArgs const&&... args) {
      if (count > 2000 && observed.size() == 29)
        printf("");
      std::vector<crack_result<Key>> ret;
      intermediate_res_t imdt(observed.size());
      // Solve as distinct substitution cyphers, in parallel
      double n_candidates = 1;
      for (size_t i = 0; i < observed.size(); ++i) {
        n_candidates *= (imdt[i] = CrackOne(observed[i]/*std::move(observed[i])*/, expected,
                           std::forward<CrackArgs>(args)..., count / observed.size(), p_value)).size();
        if (n_candidates == 0)
          return {};
      }
      // This is the maximum reasonable amount before we tell someone to piss off
      //
      // This catches BS like passing in plaintext
      if (n_candidates > 1e4)
        return {};

      // Now we reduce the lists, and kick out any which fail our p value
      std::vector<crack_result<BaseKey> const*> indexes(observed.size() - 1);
      reduce(ret, imdt, p_value, indexes);
      // Now we can sort by p-value
      sort_crack_result(ret);
      return ret;
    }
  };
}
