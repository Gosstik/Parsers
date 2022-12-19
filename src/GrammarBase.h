#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "KMP.h"

enum ExitStatus { IncorrectGrammarInput = 11 };

// abstract class for reading grammar
template <typename CharT>
class GrammarBase {
 public:
  using String = utl::BasicString<CharT>;
  using IndexT = int64_t;
  template <typename T>
  using Vector = std::vector<T>;
  template <class Key, class Value, class Hash = std::hash<Key>>
  using UMap = std::unordered_map<Key, Value, Hash>;
  template <class Key, class Hash = std::hash<Key>>
  using USet = std::unordered_set<Key, Hash>;
  using RulesRightT = Vector<Vector<IndexT>>;
  using RulesT = UMap<IndexT, RulesRightT>;

  static constexpr IndexT kIncorrectSymbolInd =
      std::numeric_limits<IndexT>::max();
  static constexpr int64_t kEpsilonInd = 0;
  static constexpr int64_t kAuxiliaryStartSymbolInd = 1;
  static constexpr int64_t kStartSymbolInd = 2;

  GrammarBase() = default;

  void Read(std::basic_istream<CharT>& input);
  virtual void Print(std::basic_ostream<CharT>& out) const;

  IndexT ToInd(CharT symbol) const;
  IndexT NonterminalsCount() const;
  IndexT TerminalsCount() const;
  bool IsTerminal(IndexT symbol) const;
  bool IsNonterminal(IndexT symbol) const;
  bool IncorrectInput(IndexT ind) const;
  bool Empty() const;
  void Clear();

 protected:
  static const String kAuxiliaryStr;
  static constexpr CharT kSlash = L'\\';
  static constexpr std::basic_string_view<CharT> kSlashStr = L"\\";
  static constexpr std::basic_string_view<CharT> kSlashEscape = L"\\\\";
  static constexpr CharT kDelim = L'`';
  static const String kDelimSpecial;
  static const String kRulesDelim;
  static constexpr CharT kRulesDelimSymbol = L'|';
  static constexpr std::basic_string_view<CharT> kRulesDelimEscape = L"\\|";
  static constexpr std::basic_string_view<CharT> kArrowStr = L" -> ";

  // terminals are <= -1, nonterminals >= 1
  // epsilon is 0, auxiliary start symbol is 1, start symbol is 2
  UMap<IndexT, String> map_ind_str_;
  UMap<String, IndexT> map_str_ind_;
  IndexT terminals_count_;     // except for epsilon
  IndexT nonterminals_count_;  // except for auxiliary start symbol
  RulesT rules_;

  virtual void AfterRead() = 0;
  virtual void AfterClear() = 0;

 private:
  // printing
  void PrintRules(std::basic_ostream<CharT>& out) const;
  // reading
  void ReadFirstLine(std::basic_istream<CharT>& input);
  void ReadSymbols(std::basic_istream<CharT>& input);
  bool ReadEscapeTerminals(const Vector<String>& split_res, IndexT& ind_i,
                           IndexT ind_j);
  void ReadRules(std::basic_istream<CharT>& input);
  IndexT ReadLeftNonterminal(std::basic_istream<CharT>& input);
  void ReadRightPart(IndexT left, const String& right_part);
  void ReadNonterminalSequence(const Vector<String>& parts, size_t r_i,
                               IndexT left, size_t& err_offt,
                               Vector<IndexT>& right);
  void ReadRuleSymbol(IndexT left, size_t& err_offt,
                      const Vector<String>& symbols, Vector<IndexT>& right,
                      CharT symbol);
  void ReadEscapeSymbol(IndexT start_ind, size_t& err_offt,
                        const Vector<String>& symbols, Vector<IndexT>& right,
                        size_t& pos, CharT symbol);
  void ReadRightPartPrintError(IndexT start_ind, size_t offset_for_error,
                               const std::vector<String>& symbols);
  bool ContainArrow(const std::vector<String>& symbols) const;
  void PrintRightPartOfRule(std::basic_ostream<CharT>& out,
                            Vector<String> symbols);
};

template <typename CharT>
const GrammarBase<CharT>::String GrammarBase<CharT>::kAuxiliaryStr =
    L"AUXILIARY";
template <typename CharT>
const GrammarBase<CharT>::String GrammarBase<CharT>::kDelimSpecial = L"";
template <typename CharT>
const GrammarBase<CharT>::String GrammarBase<CharT>::kRulesDelim = L" | ";

template <typename CharT>
GrammarBase<CharT>::IndexT GrammarBase<CharT>::ToInd(CharT symbol) const {
  auto itr = map_str_ind_.find(String(1, symbol));
  return (itr == map_str_ind_.end()) ? kIncorrectSymbolInd : itr->second;
}
template <typename CharT>
GrammarBase<CharT>::IndexT GrammarBase<CharT>::NonterminalsCount() const {
  return nonterminals_count_;
}

template <typename CharT>
GrammarBase<CharT>::IndexT GrammarBase<CharT>::TerminalsCount() const {
  return terminals_count_;
}

template <typename CharT>
bool GrammarBase<CharT>::IsTerminal(IndexT symbol) const {
  return symbol < 0;
}

template <typename CharT>
bool GrammarBase<CharT>::IsNonterminal(IndexT symbol) const {
  return symbol > 0;
}

template <typename CharT>
bool GrammarBase<CharT>::IncorrectInput(IndexT ind) const {
  return ind >= 0;
}

template <typename CharT>
bool GrammarBase<CharT>::Empty() const {
  return map_ind_str_.empty();
}

template <typename CharT>
void GrammarBase<CharT>::Clear() {
  map_ind_str_.clear();
  map_str_ind_.clear();
  nonterminals_count_ = terminals_count_ = 0;
  rules_.clear();
}

template <typename CharT>
void GrammarBase<CharT>::Print(std::basic_ostream<CharT>& out) const {
  if (Empty()) {
    out << "Grammar is not set\n";
    return;
  }
  // start nonterminal and epsilon
  out << map_ind_str_.find(kStartSymbolInd)->second << kDelim;
  out << map_ind_str_.find(kEpsilonInd)->second << '\n';
  // nonterminals
  for (IndexT i = 3; i <= nonterminals_count_; ++i) {
    out << map_ind_str_.find(i)->second << kDelim;
  }
  out << map_ind_str_.find(nonterminals_count_ + 1)->second << '\n';
  // terminals
  for (IndexT i = 1; i <= terminals_count_; ++i) {
    CharT end = (i == terminals_count_) ? '\n' : kDelim;
    String symbol = map_ind_str_.find(-i)->second;
    if (symbol == String(1, kDelim)) {
      out << kSlash << kDelim << end;
    } else if (symbol == kSlashStr) {
      out << kSlash << kSlash << end;
    } else if (symbol == String(1, kRulesDelimSymbol)) {
      out << kSlash << kRulesDelimSymbol << end;
    } else {
      out << symbol << end;
    }
  }
  PrintRules(out);
}

template <typename CharT>
void GrammarBase<CharT>::PrintRules(std::basic_ostream<CharT>& out) const {
  for (IndexT i = 2; i <= nonterminals_count_ + 1; ++i) {
    out << map_ind_str_.find(i)->second << kArrowStr;
    RulesRightT right_parts = rules_.find(i)->second;
    bool prev_was_nonterminal;
    auto pred = [&prev_was_nonterminal, this](IndexT ind, size_t s_i) -> bool {
      return (IsNonterminal(ind) && s_i != 0) ||
             (IsTerminal(ind) && prev_was_nonterminal);
    };
    auto end_pred = [&right_parts, this](size_t r_i, size_t s_i) -> bool {
      if (s_i == right_parts[r_i].size() - 1) {
        return true;
      }
      String next_symbol = map_ind_str_.find(right_parts[r_i][s_i + 1])->second;
      return next_symbol == String(1, kDelim);
    };
    // r_i = right_index, s_i = symbol_index
    for (size_t r_i = 0; r_i < right_parts.size(); ++r_i) {
      prev_was_nonterminal = false;
      for (size_t s_i = 0; s_i < right_parts[r_i].size(); ++s_i) {
        auto iter_ind_str = map_ind_str_.find(right_parts[r_i][s_i]);
        String delim = pred(iter_ind_str->first, s_i) ? String(1, kDelim) : L"";
        if (iter_ind_str->second == String(1, kDelim)) {
          String end = end_pred(r_i, s_i) ? String(1, kDelim) : L"";
          out << kDelim << kSlash << kDelim << end;
          prev_was_nonterminal = true;
          continue;  // to avoid changing `prev_was_nonterminal`
        }
        if (iter_ind_str->second == String(1, kSlash)) {
          out << delim << kSlash << kSlash;
        } else if (iter_ind_str->second == String(1, kRulesDelimSymbol)) {
          out << delim << kSlash << kRulesDelimSymbol;
        } else {
          out << delim << iter_ind_str->second;
        }
        prev_was_nonterminal = IsNonterminal(iter_ind_str->first);
      }
      if (r_i != right_parts.size() - 1) {
        out << kRulesDelim;
      }
    }
    out << '\n';
  }
}

template <typename CharT>
void GrammarBase<CharT>::Read(std::basic_istream<CharT>& input) {
  ReadFirstLine(input);
  ReadSymbols(input);
  rules_.insert({kAuxiliaryStartSymbolInd, {{kStartSymbolInd}}});
  ReadRules(input);
  AfterRead();
}

template <typename CharT>
void GrammarBase<CharT>::ReadFirstLine(std::basic_istream<CharT>& input) {
  String line;
  Vector<String> split_res;
  std::getline<CharT>(input, line, '\n');
  split_res = utl::Split(line, String(1, kDelim));
  map_str_ind_.insert({split_res[0], kStartSymbolInd});
  map_ind_str_.insert({kStartSymbolInd, split_res[0]});
  map_str_ind_.insert({split_res[1], kEpsilonInd});
  map_ind_str_.insert({kEpsilonInd, split_res[1]});
  map_str_ind_.insert({kAuxiliaryStr, kAuxiliaryStartSymbolInd});
  map_ind_str_.insert({kAuxiliaryStartSymbolInd, kAuxiliaryStr});
}

template <typename CharT>
void GrammarBase<CharT>::ReadSymbols(std::basic_istream<CharT>& input) {
  // nonterminals
  String line;
  std::getline(input, line, L'\n');
  Vector<String> split_res = utl::Split(line, String(1, kDelim));
  if (split_res.size() == 1 && split_res[0].empty()) {
    // case when S is the only nonterminal
    nonterminals_count_ = 1;
  } else {
    nonterminals_count_ = static_cast<IndexT>(split_res.size() + 1);
  }
  for (IndexT i = 0; i < nonterminals_count_ - 1; ++i) {
    map_str_ind_.insert({split_res[i], i + 3});
    map_ind_str_.insert({i + 3, split_res[i]});
  }
  // terminals
  std::getline(input, line, L'\n');
  split_res = utl::Split(line, String(1, kDelim));
  terminals_count_ = static_cast<IndexT>(split_res.size());
  for (IndexT i = 0, j = 0; i < split_res.size(); ++i, ++j) {
    if (ReadEscapeTerminals(split_res, i, j)) {
      continue;
    }
    map_str_ind_.insert({split_res[i], -j - 1});
    map_ind_str_.insert({-j - 1, split_res[i]});
  }
}

template <typename CharT>
bool GrammarBase<CharT>::ReadEscapeTerminals(const Vector<String>& split_res,
                                             IndexT& ind_i, IndexT ind_j) {
  if (split_res[ind_i].empty()) {
    // todo: make more informative
    std::wcerr << L"Empty symbol in terminals\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  if (split_res[ind_i] == kRulesDelimEscape) {
    map_str_ind_.insert({String(1, kRulesDelimSymbol), -ind_j - 1});
    map_ind_str_.insert({-ind_j - 1, String(1, kRulesDelimSymbol)});
    return true;
  }
  if (split_res[ind_i] == kSlashEscape) {
    map_str_ind_.insert({String(1, kSlash), -ind_j - 1});
    map_ind_str_.insert({-ind_j - 1, String(1, kSlash)});
    return true;
  }
  if (split_res[ind_i] == kSlashStr && ind_i != split_res.size() - 1 &&
      split_res[ind_i + 1] == kDelimSpecial) {
    map_str_ind_.insert({String(1, kDelim), -ind_j - 1});
    map_ind_str_.insert({-ind_j - 1, String(1, kDelim)});
    --terminals_count_;
    ++ind_i;
    return true;
  }
  if (split_res[ind_i].size() > 1) {
    std::wcerr << L"Symbol in terminals consists of more then one character\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  return false;
}

template <typename CharT>
void GrammarBase<CharT>::ReadRules(std::basic_istream<CharT>& input) {
  String line;
  Vector<String> split_res;
  // todo: add case bad reading
  for (size_t i = 0; i < nonterminals_count_; ++i) {
    IndexT left = ReadLeftNonterminal(input);
    std::getline(input, line, L' ');   // reading ->
    std::getline(input, line, L'\n');  // reading all right parts
    split_res = utl::Split(line, kRulesDelim);
    for (const auto& right_part : split_res) {
      ReadRightPart(left, right_part);
    }
  }
}

template <typename CharT>
typename GrammarBase<CharT>::IndexT GrammarBase<CharT>::ReadLeftNonterminal(
    std::basic_istream<CharT>& input) {
  String line;
  std::getline(input, line, L' ');  // reading left nonterminal
  auto iter_symbol_to_num = map_str_ind_.find(line);
  if (iter_symbol_to_num == map_str_ind_.end() ||
      iter_symbol_to_num->second <= 0) {
    if (ContainArrow({line})) {
      line = line.substr(0, line.find(L'\n'));
      std::wcerr << L"No spaces around the arrow: " << line;
      exit(ExitStatus::IncorrectGrammarInput);
    } else if (line.empty()) {
      std::wcerr
          << L"No left nonterminal in rule or extra nonterminals listed\n";
      exit(ExitStatus::IncorrectGrammarInput);
    }
    std::wcerr << L"Incorrect left nonterminal `" << line << L"` in rule";
    std::wcerr << L" (maybe double spaces)\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  return iter_symbol_to_num->second;
}

template <typename CharT>
void GrammarBase<CharT>::ReadRightPart(IndexT left, const String& right_part) {
  if (right_part.empty()) {
    std::wcerr << L"Incorrect right part of the rule with left nonterminal ";
    std::wcerr << L'`' << map_ind_str_[left] << L"`\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  if (right_part == map_ind_str_[kEpsilonInd]) {
    rules_[left].push_back(Vector<IndexT>(1, kEpsilonInd));
    return;
  }
  Vector<String> parts = utl::Split(right_part, String(1, kDelim));
  Vector<IndexT> right;
  size_t err_offt = map_ind_str_[left].size() + kArrowStr.size();
  size_t r_i = 0;
  if (parts[0].empty()) {
    // handle case when `\`` is in the start of the rule
    if (parts.size() < 3 || parts[1] != kSlashStr || !parts[2].empty()) {
      ReadRightPartPrintError(left, err_offt, parts);
      exit(ExitStatus::IncorrectGrammarInput);
    }
    r_i++;
  }
  for (; r_i < parts.size(); ++r_i) {
    if (parts[r_i] == kSlashStr) {
      if (r_i != parts.size() - 1 && parts[r_i + 1] == kDelimSpecial) {
        ReadEscapeSymbol(left, err_offt, parts, right, r_i, kDelim);
        continue;
      }
      ReadRightPartPrintError(left, err_offt, parts);
      exit(ExitStatus::IncorrectGrammarInput);
    }
    auto iter_str_ind = map_str_ind_.find(parts[r_i]);
    if (iter_str_ind == map_str_ind_.end()) {
      ReadNonterminalSequence(parts, r_i, left, err_offt, right);
      continue;
    }
    err_offt += parts[r_i].size() + 1;
    right.push_back(iter_str_ind->second);
  }
  rules_[left].push_back(std::move(right));
}

template <typename CharT>
void GrammarBase<CharT>::ReadNonterminalSequence(const Vector<String>& parts,
                                                 size_t r_i, IndexT left,
                                                 size_t& err_offt,
                                                 Vector<IndexT>& right) {
  for (size_t s_i = 0; s_i < parts[r_i].size(); ++s_i) {
    if (parts[r_i][s_i] == kSlash) {
      if (s_i == parts[r_i].size() - 1) {
        ReadRightPartPrintError(left, err_offt, parts);
        exit(ExitStatus::IncorrectGrammarInput);
      }
      if (parts[r_i][s_i + 1] == kSlash) {
        ReadEscapeSymbol(left, err_offt, parts, right, s_i, kSlash);
        continue;
      }
      if (parts[r_i][s_i + 1] == kRulesDelimSymbol) {
        ReadEscapeSymbol(left, err_offt, parts, right, s_i, kRulesDelimSymbol);
        continue;
      }
    }
    ReadRuleSymbol(left, err_offt, parts, right, parts[r_i][s_i]);
  }
}

template <typename CharT>
void GrammarBase<CharT>::ReadRuleSymbol(IndexT left, size_t& err_offt,
                                        const Vector<String>& symbols,
                                        Vector<IndexT>& right, CharT symbol) {
  auto iter_str_ind = map_str_ind_.find(String(1, symbol));
  if (iter_str_ind == map_str_ind_.end() ||
      IsNonterminal(iter_str_ind->second)) {
    ReadRightPartPrintError(left, err_offt, symbols);
    exit(ExitStatus::IncorrectGrammarInput);
  }
  ++err_offt;
  right.push_back(iter_str_ind->second);
}

template <typename CharT>
void GrammarBase<CharT>::ReadEscapeSymbol(IndexT start_ind, size_t& err_offt,
                                          const Vector<String>& symbols,
                                          Vector<IndexT>& right, size_t& pos,
                                          CharT symbol) {
  auto iter_str_ind = map_str_ind_.find(String(1, symbol));
  if (iter_str_ind == map_str_ind_.end()) {
    ReadRightPartPrintError(start_ind, err_offt, symbols);
    std::wcerr << symbol << " is not a terminal\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  right.push_back(iter_str_ind->second);
  ++pos;
}

template <typename CharT>
void GrammarBase<CharT>::ReadRightPartPrintError(
    IndexT start_ind, size_t offset_for_error, const Vector<String>& symbols) {
  if (ContainArrow(symbols)) {
    std::wcerr << L"Incorrect number of spaces between "
               << map_ind_str_[start_ind] << L" and rules_\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  std::wcerr << L"Incorrect nonterminal, extra space or missing '`' "
             << L"in right part of the rule:\n";
  std::wcerr << map_ind_str_[start_ind] << L" -> ";
  PrintRightPartOfRule(std::wcerr, symbols);
  std::wcerr << L'\n';
  for (size_t i = 0; i < offset_for_error; ++i) {
    std::wcerr << L' ';
  }
  std::wcerr << L"^\n";
}

template <typename CharT>
bool GrammarBase<CharT>::ContainArrow(const Vector<String>& symbols) const {
  return !symbols.empty() && symbols[0].find(L"->") != symbols[0].npos;
}

template <typename CharT>
void GrammarBase<CharT>::PrintRightPartOfRule(std::basic_ostream<CharT>& out,
                                              std::vector<String> symbols) {
  auto smb_iter = symbols.begin();
  while (true) {
    out << *smb_iter;
    ++smb_iter;
    if (smb_iter == symbols.end()) {
      break;
    }
    out << kDelim;
  }
}