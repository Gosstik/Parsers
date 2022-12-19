#pragma once

#include <fstream>
#include <stack>
#include <boost/dynamic_bitset.hpp>

#include "GrammarBase.h"

template <typename CharT, size_t K>
class BasicLRParser {
 public:
  BasicLRParser() {
    std::wcerr << "LR(" << K << ") is not implemented\n";
    exit(NotImplemented);
  }

 private:
  enum LRExitStatus {
    NotImplemented = 12
  };
};

template <typename CharT>
class BasicLRParser<CharT, 1> {
 public:
  BasicLRParser() = default;
  BasicLRParser(const std::string& filename);
  BasicLRParser(std::basic_istream<CharT>& input);

  void SetGrammar(const std::string& filename);
  void SetGrammar(std::basic_istream<CharT>& input);
  void PrintGrammar(std::basic_ostream<CharT>& out) const;
  bool Parse(const String<CharT>& word) const;

 private:
  using IndexT = GrammarBase<CharT>::IndexT;
  template <class Key, class Value, class Hash = std::hash<Key>>
  using UMap = std::unordered_map<Key, Value, Hash>;
  template <class Key, class Hash = std::hash<Key>>
  using USet = std::unordered_set<Key, Hash>;
  template <typename T>
  using Vector = std::vector<T>;
  template <typename T>
  using RefW = std::reference_wrapper<T>;
  using Bitset = boost::dynamic_bitset<>;

  enum ActionT {
    Shift = 0,
    Reduce,
    Accept
  };
  struct Action;
  struct Situation;
  struct Bucket;
  class Grammar;
  class ParseStack;

  using TableT = Vector<UMap<IndexT, Action>>;
  using ActionHasher = Action::ActionHasher;
  using SitsHasher = Situation::SituationHasher;
  using BucketHasher = Bucket::BucketHasher;
  using USetSits = USet<Situation, SitsHasher>;
  using USetBuckets = USet<Bucket, BucketHasher>;

  Grammar grammar_;
  TableT table_;

  size_t Goto(USetBuckets& buckets, Vector<RefW<const Bucket>>& buckets_vec,
              size_t bucket_id, std::stack<Situation>& unhandled_sits);
  void CreateTable();
  Bitset First(const Situation& sit) const;
  void Closure(Bucket& bucket, std::stack<Situation>& unhandled_sits);
  void Clear();
};

template <typename CharT>
struct BasicLRParser<CharT, 1>::Action {
  ActionT type;
  // if type == Shift, id contains id of bucket to shift to
  // if type == Reduce, length and left are length of right part of rule
  //            and index of left nonterminal of rule to make reduce by
  // if type == Accept, id = 0
  union {
    size_t id;
    struct {
      size_t length;
      IndexT left;
    };
  };
  Action() : type(Accept), id(0) {}
  Action(size_t bucket_id) : type(Shift), id(bucket_id) {}
  Action(size_t length, IndexT left)
      : type(Reduce), length(length), left(left) {}
  struct ActionHasher {
    static constexpr size_t kMult1 = 0x9e3779b9;
    static constexpr size_t kOff1 = 4;
    constexpr size_t operator()(const Action& act) const {
      if (act.type == Reduce) {
        return size_t(act.type) * kMult1 + act.length << kOff1 + act.left;
      }
      return size_t(act.type) * kMult1 + act.id << kOff1;
    }
  };
  bool operator==(const Action& act) const {
    if (type != act.type) {
      return false;
    }
    return (type == Reduce) ?
            length == act.length && left == act.left :
            id == act.id;
  }
};

template <typename CharT>
struct BasicLRParser<CharT, 1>::Situation {
 private:
  RefW<const Vector<IndexT>> right_;
 public:
  IndexT left;
  IndexT context;
  size_t pos;
  Situation(RefW<const std::vector<IndexT>> right, IndexT left,
            IndexT context, size_t pos)
      : right_(right), left(left), context(context), pos(pos) {}
  [[nodiscard]] bool RoolEnded() const { return pos == RightSize(); }
  [[nodiscard]] IndexT CurrSymbol() const { return right_.get()[pos]; }
  bool IsAccepted() const {
    return left == Grammar::kAuxiliaryStartSymbolInd && pos == 1;
  }
  size_t RightSize() const {
    return right_.get().size() - size_t(right_.get().back() == Grammar::kEpsilonInd);
  }
  IndexT RightSymbol(size_t ind) const { return right_.get()[ind]; }
  struct SituationHasher {
    static constexpr size_t kMult1 = 0x45d9f3b;
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
      return VectorHasher()(sit.right_.get()) + (sit.left << kOff1) +
             (sit.pos >> kOff2);
    }
  };
  bool operator==(const Situation& sit) const {
    return &right_.get() == &sit.right_.get() && left == sit.left &&
            context == sit.context && pos == sit.pos;
  }
};
// todo: add USetSits
template <typename CharT>
struct BasicLRParser<CharT, 1>::Bucket {
  UMap<IndexT, USet<Situation, SitsHasher>> shift_sits; // index is curr symbol
  UMap<IndexT, Action> reduce_sits; // index is context
  size_t id; /// mustn't be used in hashing
  bool contains_accept = false; // isn't used in hashing
  struct BucketHasher {
    static constexpr size_t kOff1 = 5;
    static constexpr size_t kOff2 = 3;
    struct USetHasher {
      constexpr size_t operator()(const USetSits& u_set) const {
        size_t seed = u_set.size();
        for (const auto& elem : u_set) {
          seed ^= SitsHasher()(elem);
        }
        return seed;
      }
    };
    constexpr size_t operator()(const Bucket& bucket) const {
      size_t seed = bucket.shift_sits.size() + bucket.reduce_sits.size();
      for (const auto& elem : bucket.shift_sits) {
        seed ^= elem.first + (seed << kOff1) + USetHasher()(elem.second);
      }
      for (const auto& elem : bucket.reduce_sits) {
        seed ^= elem.first + (seed << kOff2) + ActionHasher()(elem.second);
      }
      return seed;
    }
  };
  bool operator==(const Bucket& bucket) const {
    return shift_sits == bucket.shift_sits && reduce_sits == bucket.reduce_sits;
  }
//  bool operator!=(const Bucket& bucket) const {
//    return !(*this == bucket);
//  }
//  bool operator<(const Bucket& bucket) const {
//    return *this == bucket || id < bucket.id;
//  }
};

template <typename CharT>
class BasicLRParser<CharT, 1>::Grammar : public GrammarBase<CharT>{
 public:
  using RulesRightT = GrammarBase<CharT>::RulesRightT;
  using GrammarBase<CharT>::kEpsilonInd;
  using GrammarBase<CharT>::kStartSymbolInd;
  using GrammarBase<CharT>::kAuxiliaryStartSymbolInd;

  const Bitset& First(IndexT symbol) const {
    return first_.find(symbol)->second;
  }
  IndexT FromBitsetInd(size_t ind) const {
    return IndexT(ind) - terminals_count_;
  }
  bool ProduceEpsilon(IndexT ind) const {
    return first_.find(ind)->second.test(ToBitsetInd(this->kEpsilonInd));
  }
  IndexT MinIndex() const { return -terminals_count_; }
  IndexT MaxIndex() const { return nonterminals_count_ + 1; }
  Situation StartSituation() const {
    assert(("Grammar is not set", !this->Empty()));
    auto iter = this->rules_.find(this->kAuxiliaryStartSymbolInd);
    return {iter->second[0], iter->first, this->kEpsilonInd, 0};
  }
  const RulesRightT& RightPart(IndexT ind) const {
    return rules_.find(ind)->second;
  }
  size_t BitsetSize() const { return bitset_size_; }
  static bool IsEpsilon(IndexT ind) { return ind == kEpsilonInd; }

 protected:
  using GrammarBase<CharT>::num_to_symbol_;
  using GrammarBase<CharT>::symbol_to_num_;
  using GrammarBase<CharT>::terminals_count_;
  using GrammarBase<CharT>::nonterminals_count_;
  using GrammarBase<CharT>::rules_;

  void AfterRead() override {
    /// create first_
    // terminals
    bitset_size_ = terminals_count_ + nonterminals_count_ + 2;
    for (IndexT i = MinIndex(); i <= kEpsilonInd; ++i) {
      Bitset bitset(bitset_size_);
      bitset.set(ToBitsetInd(i));
      first_[i] = std::move(bitset);
    }
    // nonterminals
    for (IndexT i = kStartSymbolInd; i <= MaxIndex(); ++i) {
      first_[i] = Bitset(bitset_size_);
    }
    bool change = true;
    while (change) {
      change = false;
      for (IndexT i = kStartSymbolInd; i <= MaxIndex(); ++i) {
        Bitset prev_bitset = first_[i];
        for (const auto& right : rules_[i]) {
          size_t curr = 0;
          do  {
            first_[i] |= First(right[curr]);
          } while (curr < right.size() - 1 && ProduceEpsilon(right[curr++]));
        }
        change |= first_[i] != prev_bitset;
      }
    }
  }

  void AfterClear() override {
    first_.clear();
    bitset_size_ = 0;
  }

 private:
  UMap<IndexT, Bitset> first_;
  size_t bitset_size_;
  size_t ToBitsetInd(IndexT ind) const { return ind + terminals_count_; }
};

template <typename CharT>
class BasicLRParser<CharT, 1>::ParseStack {
 private:
  size_t size_ = 0;
  size_t capacity_;
  Vector<IndexT> vec_;
 public:
  ParseStack(size_t capacity) : capacity_(capacity + 1), vec_(capacity_, 0) {}
  void Push(IndexT ind) {
    if (size_ == capacity_) {
      capacity_ *= 2;
      vec_.resize(capacity_);
    }
    vec_[size_++] = ind;
  }
  void Pop(size_t count) {
    size_ -= count;
  }
  IndexT Top() const { return vec_[size_ - 1]; }
};

template <typename CharT>
BasicLRParser<CharT, 1>::BasicLRParser(const std::string& filename) {
  SetGrammar(filename);
}

template <typename CharT>
BasicLRParser<CharT, 1>::BasicLRParser(std::basic_istream<CharT>& input) {
  SetGrammar(input);
}

template <typename CharT>
void BasicLRParser<CharT, 1>::SetGrammar(const std::string& filename) {
  std::wifstream file(filename);
  SetGrammar(file);
}

template <typename CharT>
void BasicLRParser<CharT, 1>::SetGrammar(std::basic_istream<CharT>& input) {
  Clear();
  grammar_.Read(input);
  CreateTable();
}

template <typename CharT>
void BasicLRParser<CharT, 1>::PrintGrammar(std::basic_ostream<CharT>& out) const {
  grammar_.Print(out);
}

template <typename CharT>
bool BasicLRParser<CharT, 1>::Parse(const String<CharT>& word) const {
  ParseStack stack(word.size());
  stack.Push(0);
  size_t curr_pos = 0;
  IndexT left; // index of left nonterminal in rule for reduce
  IndexT curr_ind;
  while (true) {
    if (curr_pos < word.size()) {
      curr_ind = grammar_.ToInd(word[curr_pos]);
      if (grammar_.IncorrectInput(curr_ind)) {
        return false;
      }
    } else {
      curr_ind = Grammar::kEpsilonInd;
    }
    auto iter = table_[stack.Top()].find(curr_ind);
    if (iter == table_[stack.Top()].end()) {
      return false;
    }
    const Action& act = iter->second;
    switch(act.type) {
      case Shift:
        stack.Push(act.id);
        ++curr_pos;
        break;
      case Reduce:
        stack.Pop(act.length);
        left = act.left;
        iter = table_[stack.Top()].find(left);
        if (iter == table_[stack.Top()].end()) {
          return false;
        }
        stack.Push(iter->second.id);
        break;
      case Accept:
        return true;
    }
  }
}

template <typename CharT>
size_t BasicLRParser<CharT, 1>::Goto(
    USetBuckets& buckets, Vector<RefW<const Bucket>>& buckets_vec,
    size_t bucket_id, std::stack<Situation>& unhandled_sits) {
  Bucket bucket;
  bucket.id = bucket_id;
  Closure(bucket, unhandled_sits);
  auto res = buckets.insert(std::move(bucket));
  if (res.second) {
    buckets_vec.push_back(std::cref(*res.first));
  }
  return res.first->id;
}

template <typename CharT>
void BasicLRParser<CharT, 1>::CreateTable() {
  USetBuckets buckets;
  Vector<RefW<const Bucket>> buckets_vec;
  std::stack<Situation> unhandled_sits; // for closure
  unhandled_sits.push(grammar_.StartSituation());
  Goto(buckets, buckets_vec, 0, unhandled_sits);
  size_t curr = 0;
  while (curr < buckets_vec.size()) {
    UMap<IndexT, Action> table_cell;
    const Bucket& bucket = buckets_vec[curr].get();
    // handle reduce situations
    for (const auto& map : bucket.reduce_sits) {
      table_cell[map.first] = map.second;
    }
    // handle shift situations
    for (const auto& map : bucket.shift_sits) {
      if (table_cell.contains(map.first)) {
        std::wcerr << "shift-reduce conflict, grammar is not LR(1)\n";
        Clear(); // to reuse parser
        exit(2);
        // todo: make exception
      }
      for (const auto& sit : map.second) {
        Situation new_sit(sit);
        ++new_sit.pos;
        unhandled_sits.push(new_sit);
      }
      size_t bucket_id = Goto(buckets, buckets_vec, buckets.size(), unhandled_sits);
      table_cell[map.first] = Action(bucket_id);
    }
    // handle accept situations
    if (bucket.contains_accept) {
      bool was_inserted = table_cell.insert({Grammar::kEpsilonInd, Action()}).second;
      if (!was_inserted) {
        std::wcerr << "reduce-reduce conflict, grammar is not LR(1)\n";
        Clear();
        exit(2);
        // todo: make exception
      }
    }
    table_.push_back(table_cell);
    ++curr;
  }
}

template <typename CharT>
BasicLRParser<CharT, 1>::Bitset BasicLRParser<CharT, 1>::First(const Situation& sit) const {
  /// this function may be called ONLY for sits that have not ended
  Bitset res(grammar_.BitsetSize());
  size_t curr_pos = sit.pos + 1;
  if (curr_pos == sit.RightSize()) {
    res = grammar_.First(sit.context);
    return res;
  }
  res |= grammar_.First(sit.RightSymbol(curr_pos));
  while (grammar_.ProduceEpsilon(sit.RightSymbol(curr_pos++))) {
    if (curr_pos == sit.RightSize()) {
      res |= grammar_.First(sit.context);
      break;
    }
    res |= grammar_.First(sit.RightSymbol(curr_pos));
  }
  return res;
}

template <typename CharT>
void BasicLRParser<CharT, 1>::Closure(Bucket& bucket,
                                      std::stack<Situation>& unhandled_sits) {
  size_t count = unhandled_sits.size();
  USetSits prev_sits;
  while (!unhandled_sits.empty()) {
    Situation sit = unhandled_sits.top();
    unhandled_sits.pop();
    if (count > 0) {
      prev_sits.insert(sit);
      --count;
    }
    if (sit.RoolEnded()) {
      if (sit.IsAccepted()) {
        bucket.contains_accept = true;
      } else {
        Action action(sit.RightSize(), sit.left);
        bool inserted = bucket.reduce_sits.insert({sit.context, action}).second;
        if (!inserted) {
          std::wcerr << "reduce-reduce conflict, grammar is not LR(1)\n";
          Clear();
          exit(2);
          // todo: make exception
        }
      }
    } else {
      bool inserted = bucket.shift_sits[sit.CurrSymbol()].insert(sit).second;
      if (inserted && grammar_.IsNonterminal(sit.CurrSymbol())) {
        IndexT left = sit.CurrSymbol();
        Bitset bitset = First(sit);
        for (size_t i = 0; i < bitset.size(); ++i) {
          if (bitset.test(i)) {
            for (const auto& right : grammar_.RightPart(left)) {
              Situation new_sit(right, left, grammar_.FromBitsetInd(i), 0);
              if (prev_sits.insert(new_sit).second) {
                unhandled_sits.push(new_sit);
              }
            }
          }
        }
      }
    }
  }
}

template <typename CharT>
void BasicLRParser<CharT, 1>::Clear() {
  grammar_.Clear();
  table_.clear();
}

template <size_t K>
using LRParser = BasicLRParser<char, K>;
template <size_t K>
using WLRParser = BasicLRParser<wchar_t, K>;