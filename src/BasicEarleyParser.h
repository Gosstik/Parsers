#pragma once

#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "GrammarBase.h"

template <typename CharT>
class BasicEarleyParser {
 private:
  template <typename Key, typename Value>
  using UMap = std::unordered_map<Key, Value>;
  using IndexT = int64_t;

  struct Situation;
  struct SituationHasher;
  // SetD consist if situations where dot is before nonterminal
  using USetSits = std::unordered_set<Situation, SituationHasher>;
  using SetD = UMap<IndexT, USetSits>;

  struct Situation {
   private:
    template <typename T>
    using RefW = std::reference_wrapper<T>;

   public:
    RefW<const std::vector<IndexT>> right_part;
    RefW<const SetD> prev_d;
    IndexT left_symbol;
    size_t curr_pos;
    Situation(RefW<const std::vector<IndexT>> right_part,
              RefW<const SetD> prev_d, IndexT left_symbol, size_t curr_pos = 0)
        : right_part(right_part),
          prev_d(prev_d),
          left_symbol(left_symbol),
          curr_pos(curr_pos) {}
    [[nodiscard]] bool RoolEnded() const {
      return curr_pos == right_part.get().size();
    }
    [[nodiscard]] IndexT CurrSymbolInd() const {
      return right_part.get()[curr_pos];
    }
    bool operator==(const Situation& sit) const {
      return &right_part.get() == &sit.right_part.get() &&
             &prev_d.get() == &sit.prev_d.get() &&
             left_symbol == sit.left_symbol && curr_pos == sit.curr_pos;
    }
    Situation(const Situation& sit)
        : right_part(sit.right_part),
          prev_d(sit.prev_d),
          left_symbol(sit.left_symbol),
          curr_pos(sit.curr_pos) {}
    // for debug
    void Print() const {
      std::cout << "Print Situation:\n";
      GrammarBase<CharT>::PrintVec(right_part.get());
      std::cout << &prev_d.get() << ' ' << left_symbol << ' ' << curr_pos
                << std::endl;
    }
  };

  struct SituationHasher {
    static constexpr size_t kMult1 = 0x9e3779b9;
    static constexpr size_t kOff1 = 6;
    static constexpr size_t kOff2 = 2;
    struct VectorHasher {
      constexpr size_t operator()(const std::vector<IndexT>& vec) const {
        size_t seed = vec.size();
        for (IndexT elem : vec) {
          seed ^= elem + kMult1 + (seed << kOff1) + (seed >> kOff2);
        }
        return seed;
      }
    };
    constexpr size_t operator()(const Situation& sit) const {
      return VectorHasher()(sit.right_part.get()) + (sit.left_symbol << kOff1) +
             (sit.curr_pos >> kOff2);
    }
  };

  // todo: think how to make this function const
  struct Grammar : public GrammarBase<CharT> {
    [[nodiscard]] Situation GetStartSituation(SetD& start_set) {
      assert(("Grammar is not set", !this->Empty()));
      auto itr = this->rules_.find(this->kAuxiliaryStartSymbolInd);
      return {std::cref(itr->second[0]), std::cref(start_set),
              this->kAuxiliaryStartSymbolInd};
    }

    [[nodiscard]] bool IsFinalSituation(const Situation& sit) const {
      return sit.left_symbol == this->kAuxiliaryStartSymbolInd &&
             sit.curr_pos == 1;
    }
  };

  Grammar grammar_;

  void Complete(Situation&& curr_sit, std::stack<Situation>& stk_sits,
                USetSits& handled_sits) const {
    auto itr = curr_sit.prev_d.get().find(curr_sit.left_symbol);
    if (itr == curr_sit.prev_d.get().end()) {
      return;
    }
    for (const auto& prev_sit : itr->second) {
      Situation new_sit = {std::cref(prev_sit.right_part),
                           std::cref(prev_sit.prev_d), prev_sit.left_symbol,
                           prev_sit.curr_pos + 1};
      if (!handled_sits.contains(new_sit)) {
        handled_sits.insert(new_sit);
        stk_sits.push(std::move(new_sit));
      }
    }
  }

  void Predict(Situation&& curr_sit, SetD& set_d,
               std::stack<Situation>& stk_sits, USetSits& handled_sits) const {
    IndexT left_part = curr_sit.CurrSymbolInd();
    if (grammar_.GenerateEpsilon(left_part)) {
      ++curr_sit.curr_pos;
      if (!handled_sits.contains(curr_sit)) {
        handled_sits.insert(curr_sit);
        stk_sits.push(std::move(curr_sit));
      }
    }
    for (const auto& right_part : grammar_.GetRightPartRules(left_part)) {
      Situation new_sit = {std::cref(right_part), std::cref(set_d), left_part};
      if (!handled_sits.contains(new_sit)) {
        handled_sits.insert(new_sit);
        stk_sits.push(std::move(new_sit));
      }
    }
  }

 public:
  BasicEarleyParser() = default;
  BasicEarleyParser(std::basic_istream<CharT>& input) { EnterGrammar(input); }
  BasicEarleyParser(const std::string& filename) { EnterGrammar(filename); }

  void EnterGrammar(std::basic_istream<CharT>& input) { grammar_.Read(input); }
  void EnterGrammar(const std::string& filename) {
    std::wifstream file(filename);
    grammar_.Read(file);
  }
  void PrintGrammar(std::basic_ostream<CharT>& out) { grammar_.Print(out); }
  // GetStartSituation does not allow to make this function const
  bool Parse(const String<CharT>& word) {
    assert(("Grammar is not set for Earley parser", !grammar_.Empty()));
    if (word.empty()) {
      return grammar_.GenerateEpsilon();
    }
    std::vector<SetD> sets_vec(word.size() + 1);
    USetSits handled_sits;
    USetSits next_handle_sits;
    std::stack<Situation> stk_sits;
    std::stack<Situation> stk_next_d;
    stk_next_d.push(grammar_.GetStartSituation(sets_vec[0]));
    next_handle_sits.insert(stk_next_d.top());
    for (size_t i = 0; i <= sets_vec.size(); ++i) {
      handled_sits = std::move(next_handle_sits);
      stk_sits = std::move(stk_next_d);
      while (!stk_sits.empty()) {
        Situation curr_sit = stk_sits.top();
        stk_sits.pop();
        if (i == word.size() && grammar_.IsFinalSituation(curr_sit)) {
          return true;
        }
        if (curr_sit.RoolEnded()) {
          Complete(std::move(curr_sit), stk_sits, handled_sits);
        } else if (grammar_.IsNonterminal(curr_sit.CurrSymbolInd())) {
          sets_vec[i][curr_sit.CurrSymbolInd()].insert(curr_sit);
          Predict(std::move(curr_sit), sets_vec[i], stk_sits, handled_sits);
        } else if (i < word.size() &&
                   curr_sit.CurrSymbolInd() == grammar_.ToInd(word[i])) {
          // scan()
          ++curr_sit.curr_pos;
          stk_next_d.push(curr_sit);
          next_handle_sits.insert(std::move(curr_sit));
        }
      }
    }
    return false;
  }
};

using WEarleyParser = BasicEarleyParser<wchar_t>;
using EarleyParser = BasicEarleyParser<char>;