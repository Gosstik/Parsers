#pragma once

#include <fstream>
#include <functional>
#include <iostream>

#include "GrammarBase.h"

template <typename CharT>
class BasicEarleyParser {
 public:
  BasicEarleyParser() = default;
  BasicEarleyParser(const std::string& filename);
  BasicEarleyParser(std::basic_istream<CharT>& input);

  void EnterGrammar(const std::string& filename);
  void EnterGrammar(std::basic_istream<CharT>& input);
  void PrintGrammar(std::basic_ostream<CharT>& out);
  bool Parse(const std::basic_string<CharT>& word) const;

 private:
  using String = utl::BasicString<CharT>;
  using IndexT = int64_t;
  template <typename T>
  using Vector = std::vector<T>;
  template <class Key, class Value, class Hash = std::hash<Key>>
  using UMap = std::unordered_map<Key, Value, Hash>;
  template <class Key, class Hash = std::hash<Key>>
  using USet = std::unordered_set<Key, Hash>;
  template <typename T>
  using RefW = std::reference_wrapper<T>;

  struct Situation;
  class Grammar;

  using SituationHasher = Situation::SituationHasher;
  using USetSits = USet<Situation, SituationHasher>;
  // SetD consist if situations where dot is before nonterminal
  using SetD = UMap<IndexT, USetSits>;

  Grammar grammar_;

  void Complete(const Situation& curr_sit, std::stack<Situation>& stk_sits,
                USetSits& handled_sits) const;
  void Predict(Situation curr_sit, SetD& set_d, std::stack<Situation>& stk_sits,
               USetSits& handled_sits) const;
  void Clear();
};

template <typename CharT>
struct BasicEarleyParser<CharT>::Situation {
 public:
  struct SituationHasher {
    static constexpr size_t kMult1 = 0x9e3779b9;
    static constexpr size_t kOff1 = 6;
    static constexpr size_t kOff2 = 2;
    struct VectorHasher {
      constexpr size_t operator()(const Vector<IndexT>& vec) const {
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

  RefW<const Vector<IndexT>> right_part;
  RefW<const SetD> prev_d;
  IndexT left_symbol;
  size_t curr_pos;
  Situation(RefW<const Vector<IndexT>> right_part, RefW<const SetD> prev_d,
            IndexT left_symbol, size_t curr_pos = 0)
      : right_part(right_part),
        prev_d(prev_d),
        left_symbol(left_symbol),
        curr_pos(curr_pos) {}
  bool RoolEnded() const { return curr_pos == right_part.get().size(); }
  IndexT CurrSymbolInd() const { return right_part.get()[curr_pos]; }
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
};

template <typename CharT>
class BasicEarleyParser<CharT>::Grammar : public GrammarBase<CharT> {
 public:
  using RulesRightT = GrammarBase<CharT>::RulesRightT;
  using RulesT = GrammarBase<CharT>::RulesT;
  // todo: think how to make this function const
  [[nodiscard]] Situation GetStartSituation(SetD& start_set) const {
    assert(("Grammar is not set", !this->Empty()));
    auto itr = this->rules_.find(this->kAuxiliaryStartSymbolInd);
    return {std::cref(itr->second[0]), std::cref(start_set),
            this->kAuxiliaryStartSymbolInd};
  }
  [[nodiscard]] bool IsFinalSituation(const Situation& sit) const {
    return sit.left_symbol == this->kAuxiliaryStartSymbolInd &&
           sit.curr_pos == 1;
  }
  // returns 'true' if grammar generate epsilon
  [[nodiscard]] bool GenerateEpsilon() const {
    return GenerateEpsilon(this->kStartSymbolInd);
  }
  // returns 'true' if symbol generate epsilon
  [[nodiscard]] bool GenerateEpsilon(IndexT symbol) const {
    return proc_eps_generating_symbols_.contains(symbol);
  }
  const RulesRightT& RightPart(IndexT left) const {
    return rules_without_eps_.find(left)->second;
  }

 protected:
  void AfterRead() override {
    // process epsilon generating symbols
    UMap<IndexT, std::vector<USet<IndexT>>> rules_for_eps_generating;
    RulesRightT right_parts_vec;
    RulesRightT tmp;
    for (auto& map : this->rules_) {
      IndexT left = map.first;
      std::vector<USet<IndexT>> right_parts_for_eps;
      for (const auto& right : map.second) {
        if (right[0] == this->kEpsilonInd) {
          start_eps_generating_symbols_.insert(left);
          proc_eps_generating_symbols_.insert(left);
        } else {
          right_parts_vec.push_back(right);
          USet<IndexT> right_part;
          for (auto symbol : (right)) {
            right_part.insert(symbol);
          }
          right_parts_for_eps.push_back(std::move(right_part));
        }
      }
      rules_without_eps_.insert({left, std::move(right_parts_vec)});
      rules_for_eps_generating.insert({left, std::move(right_parts_for_eps)});
    }
    ProcEpsGeneratingSymbols(rules_for_eps_generating);
  }

  void ProcEpsGeneratingSymbols(
      UMap<IndexT, std::vector<USet<IndexT>>>& rules_for_eps_generating) {
    if (proc_eps_generating_symbols_.empty()) {
      return;
    }
    USet<IndexT> unhandled_smb;
    for (IndexT i = 2; i <= this->nonterminals_count_ + 1; ++i) {
      unhandled_smb.insert(i);
    }
    std::stack<IndexT> stk;
    for (IndexT ind : proc_eps_generating_symbols_) {
      stk.push(ind);
      unhandled_smb.erase(ind);
    }
    while (!stk.empty()) {
      IndexT curr_ind = stk.top();
      stk.pop();
      auto it_curr = unhandled_smb.begin();
      while (it_curr != unhandled_smb.end()) {
        auto it_copy = it_curr;
        auto iter = rules_for_eps_generating.find(*it_curr);
        for (auto& right : iter->second) {
          auto it_ind = right.begin();
          while (it_ind != right.end()) {
            if (*it_ind == curr_ind) {
              it_ind = right.erase(it_ind);
            } else {
              ++it_ind;
            }
            if (right.empty()) {
              proc_eps_generating_symbols_.insert(*it_curr);
              stk.push(*it_curr);
              it_curr = unhandled_smb.erase(it_curr);
              break;
            }
          }
        }
        if (it_curr == it_copy) {  // if there were no deletion
          ++it_curr;
        }
      }
    }
  }

  void AfterClear() override {
    rules_without_eps_.clear();
    start_eps_generating_symbols_.clear();
    proc_eps_generating_symbols_.clear();
  }

 private:
  RulesT rules_without_eps_;
  USet<IndexT> start_eps_generating_symbols_;  // for printing source grammar
  USet<IndexT> proc_eps_generating_symbols_;
};

template <typename CharT>
BasicEarleyParser<CharT>::BasicEarleyParser(const std::string& filename) {
  EnterGrammar(filename);
}

template <typename CharT>
BasicEarleyParser<CharT>::BasicEarleyParser(std::basic_istream<CharT>& input) {
  EnterGrammar(input);
}

template <typename CharT>
void BasicEarleyParser<CharT>::EnterGrammar(const std::string& filename) {
  std::wifstream file(filename);
  EnterGrammar(file);
}

template <typename CharT>
void BasicEarleyParser<CharT>::EnterGrammar(std::basic_istream<CharT>& input) {
  Clear();
  grammar_.Read(input);
}

template <typename CharT>
void BasicEarleyParser<CharT>::PrintGrammar(std::basic_ostream<CharT>& out) {
  grammar_.Print(out);
}

template <typename CharT>
bool BasicEarleyParser<CharT>::Parse(
    const std::basic_string<CharT>& word) const {
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

template <typename CharT>
void BasicEarleyParser<CharT>::Complete(const Situation& curr_sit,
                                        std::stack<Situation>& stk_sits,
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

template <typename CharT>
void BasicEarleyParser<CharT>::Predict(Situation curr_sit, SetD& set_d,
                                       std::stack<Situation>& stk_sits,
                                       USetSits& handled_sits) const {
  IndexT left_part = curr_sit.CurrSymbolInd();
  if (grammar_.GenerateEpsilon(left_part)) {
    ++curr_sit.curr_pos;
    if (!handled_sits.contains(curr_sit)) {
      handled_sits.insert(curr_sit);
      stk_sits.push(curr_sit);
    }
  }
  for (const auto& right_part : grammar_.RightPart(left_part)) {
    Situation new_sit = {std::cref(right_part), std::cref(set_d), left_part};
    if (!handled_sits.contains(new_sit)) {
      handled_sits.insert(new_sit);
      stk_sits.push(std::move(new_sit));
    }
  }
}

template <typename CharT>
void BasicEarleyParser<CharT>::Clear() {
  grammar_.Clear();
}

using WEarleyParser = BasicEarleyParser<wchar_t>;
using EarleyParser = BasicEarleyParser<char>;