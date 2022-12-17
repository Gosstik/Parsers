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

template <typename CharT>
class GrammarBase {
 public:
  using IndexT = int64_t;
  template <typename Key, typename Value>
  using UMap = std::unordered_map<Key, Value>;
  template <typename Key>
  using USet = std::unordered_set<Key>;
  using RulesT = UMap<IndexT, std::vector<std::vector<IndexT>>>;

  GrammarBase() = default;

  void Read(std::basic_istream<CharT>& input);
  void Print(std::basic_ostream<CharT>& out) const;

  const std::vector<std::vector<IndexT>>& GetRightPartRules(
      IndexT left_part) const {
    auto res = rules_.find(left_part);
    return res->second;
  }

  IndexT ToInd(CharT symbol) const {
    auto itr = symbol_to_num_.find(String<CharT>(1, symbol));
    return (itr == symbol_to_num_.end()) ? std::numeric_limits<IndexT>::max()
                                         : itr->second;
  }
  // returns 'true' if grammar generate epsilon
  [[nodiscard]] bool GenerateEpsilon() const {
    return GenerateEpsilon(kStartSymbolInd);
  }
  // returns 'true' if symbol generate epsilon
  [[nodiscard]] bool GenerateEpsilon(IndexT symbol) const {
    return proc_eps_generating_symbols_.contains(symbol);
  }
  [[nodiscard]] bool IsTerminal(IndexT symbol) const { return symbol < 0; }
  [[nodiscard]] bool IsNonterminal(IndexT symbol) const { return symbol > 0; }
  [[nodiscard]] bool Empty() const { return num_to_symbol_.empty(); }

 protected:
  static constexpr int64_t kEpsilonInd = 0;
  static constexpr int64_t kAuxiliaryStartSymbolInd = 1;
  static constexpr int64_t kStartSymbolInd = 2;
  static constexpr CharT kSlash = L'\\';
  static constexpr std::basic_string_view<CharT> kSlashStr = L"\\";
  static constexpr std::basic_string_view<CharT> kSlashEscape = L"\\\\";
  static constexpr CharT kDelim = L'`';
  static const std::basic_string<CharT> kDelimSpecial;
  static const std::basic_string<CharT> kRulesDelim;
  static constexpr CharT kRulesDelimSymbol = L'|';
  static constexpr std::basic_string_view<CharT> kRulesDelimEscape = L"\\|";
  static constexpr std::basic_string_view<CharT> kArrowStr = L" -> ";

  // terminals are <= -1, nonterminals >= 1
  // epsilon is 0, auxiliary start symbol is 1, start symbol is 2
  UMap<IndexT, String<CharT>> num_to_symbol_;
  UMap<String<CharT>, IndexT> symbol_to_num_;
  IndexT terminals_count_;
  IndexT nonterminals_count_;  // except for auxiliary start symbol
  RulesT rules_;               // does not contain epsilon rules
  USet<IndexT> start_eps_generating_symbols_;  // for printing source grammar
  USet<IndexT> proc_eps_generating_symbols_;

 private:
  // printing
  void PrintRules(std::basic_ostream<CharT>& out) const;
  // reading
  void ReadFirstLine(std::basic_istream<CharT>& input);
  void ReadSymbols(std::basic_istream<CharT>& input);
  void ReadRules(
      std::basic_istream<CharT>& input,
      UMap<IndexT, std::vector<USet<IndexT>>>& rules_for_eps_generating);
  IndexT ReadLeftNonterminal(std::basic_istream<CharT>& input);
  void ReadRightPart(
      IndexT start_ind, const String<CharT>& right_part,
      UMap<IndexT, std::vector<USet<IndexT>>>& rules_for_eps_generating);
  void ReadRightPartHandleSymbol(IndexT start_ind, size_t& offset_for_error,
                                 const std::vector<String<CharT>>& symbols,
                                 std::vector<IndexT>& right_part_inds,
                                 USet<IndexT>& right_part_eps_inds,
                                 CharT symbol);
  void ReadRightPartHandleSpecialSymbol(
      IndexT start_ind, size_t& offset_for_error,
      const std::vector<String<CharT>>& symbols,
      std::vector<IndexT>& right_part_inds, USet<IndexT>& right_part_eps_inds,
      size_t& i, CharT symbol);
  void ReadRightPartPrintError(IndexT start_ind, size_t offset_for_error,
                               const std::vector<String<CharT>>& symbols);
  bool ContainArrow(const std::vector<String<CharT>>& symbols) const;
  void PrintRightPartOfRule(std::basic_ostream<CharT>& out,
                            std::vector<String<CharT>> symbols);
  void ProcEpsGeneratingSymbols(
      UMap<IndexT, std::vector<USet<IndexT>>>& rules_for_eps_generating);

 public:
  // for debugging // todo: make static
  template <typename T>
  void PrintUSet(const USet<T>& u_set) const;
  template <typename T, typename U>
  void PrintUMap(const UMap<T, U>& u_map) const;
  void PrintUMap(
      const UMap<IndexT, std::vector<std::vector<IndexT>>>& u_map) const;
  void PrintUMap(const UMap<IndexT, std::vector<USet<IndexT>>>& u_map) const;
  template <typename T>
  void PrintVecMap(const std::vector<USet<T>>& u_map) const;
  template <typename T>
  void PrintVec2(const std::vector<std::vector<T>>& vec2) const;
  template <typename T>
  static void PrintVec(const std::vector<T>& vec) {
    for (const auto& elem : vec) {
      std::cout << elem << ' ';
    }
    std::cout << '\n';
  }
};

template <typename CharT>
const std::basic_string<CharT> GrammarBase<CharT>::kDelimSpecial = L"";
template <typename CharT>
const std::basic_string<CharT> GrammarBase<CharT>::kRulesDelim = L" | ";

template <typename CharT>
void GrammarBase<CharT>::Print(std::basic_ostream<CharT>& out) const {
  if (Empty()) {
    out << "Grammar is not set\n";
  }
  // start nonterminal and epsilon
  auto n_to_s_itr = num_to_symbol_.find(kStartSymbolInd);
  out << n_to_s_itr->second << kDelim;
  n_to_s_itr = num_to_symbol_.find(kEpsilonInd);
  out << n_to_s_itr->second << '\n';
  // nonterminals
  for (IndexT i = 3; i <= nonterminals_count_; ++i) {
    n_to_s_itr = num_to_symbol_.find(i);
    out << n_to_s_itr->second << kDelim;
  }
  n_to_s_itr = num_to_symbol_.find(nonterminals_count_ + 1);
  out << n_to_s_itr->second << '\n';
  // terminals
  for (IndexT i = 1; i < terminals_count_; ++i) {
    n_to_s_itr = num_to_symbol_.find(-i);
    if (n_to_s_itr->second == String<CharT>(1, kDelim)) {
      out << kSlash << kDelim << kDelim;
    } else if (n_to_s_itr->second == String<CharT>(1, kRulesDelimSymbol)) {
      out << kSlash << kRulesDelimSymbol << kDelim;
    } else {
      out << n_to_s_itr->second << kDelim;
    }
  }
  n_to_s_itr = num_to_symbol_.find(-terminals_count_);
  if (n_to_s_itr->second == String<CharT>(1, kDelim)) {
    out << kSlash << kDelim << '\n';
  } else if (n_to_s_itr->second == String<CharT>(1, kRulesDelimSymbol)) {
    out << kSlash << kRulesDelimSymbol << '\n';
  } else {
    out << n_to_s_itr->second << '\n';
  }
  PrintRules(out);
}

template <typename CharT>
void GrammarBase<CharT>::PrintRules(std::basic_ostream<CharT>& out) const {
  for (IndexT i = 2; i <= nonterminals_count_ + 1; ++i) {
    auto n_to_s_itr = num_to_symbol_.find(i);
    out << n_to_s_itr->second << kArrowStr;
    auto map_itr = rules_.find(i);
    std::vector<std::vector<IndexT>> right_parts = map_itr->second;
    bool prev_is_nonterm;
    // r_i = right_index, s_i = symbol_index
    for (size_t r_i = 0; r_i < right_parts.size(); ++r_i) {
      n_to_s_itr = num_to_symbol_.find(right_parts[r_i][0]);
      if (n_to_s_itr->second == String<CharT>(1, kDelim)) {
        out << kSlash << kDelim;
      } else if (n_to_s_itr->second == String<CharT>(1, kSlash)) {
        out << kSlash << kSlash;
      } else if (n_to_s_itr->second == String<CharT>(1, kRulesDelimSymbol)) {
        out << kSlash << kRulesDelimSymbol;
      } else {
        out << n_to_s_itr->second;
      }
      prev_is_nonterm = IsNonterminal(n_to_s_itr->first);
      for (size_t s_i = 1; s_i < right_parts[r_i].size(); ++s_i) {
        n_to_s_itr = num_to_symbol_.find(right_parts[r_i][s_i]);
        if (IsNonterminal(n_to_s_itr->first) ||
            (IsTerminal(n_to_s_itr->first) && prev_is_nonterm) ||
            n_to_s_itr->second == String<CharT>(1, kDelim)) {
          out << kDelim;
        }
        prev_is_nonterm = IsNonterminal(n_to_s_itr->first);
        if (n_to_s_itr->second == String<CharT>(1, kDelim)) {
          prev_is_nonterm = true;
          out << kSlash << kDelim;
        } else if (n_to_s_itr->second == String<CharT>(1, kSlash)) {
          out << kSlash << kSlash;
        } else if (n_to_s_itr->second == String<CharT>(1, kRulesDelimSymbol)) {
          out << kSlash << kRulesDelimSymbol;
        } else {
          out << n_to_s_itr->second;
        }
      }
      if (r_i != right_parts.size() - 1) {
        out << kRulesDelim;
      }
    }
    if (start_eps_generating_symbols_.contains(i)) {
      n_to_s_itr = num_to_symbol_.find(kEpsilonInd);
      out << kRulesDelim << n_to_s_itr->second;
    }
    out << '\n';
  }
}

template <typename CharT>
void GrammarBase<CharT>::Read(std::basic_istream<CharT>& input) {
  ReadFirstLine(input);
  ReadSymbols(input);
  rules_.insert({kAuxiliaryStartSymbolInd, {{kStartSymbolInd}}});
  UMap<IndexT, std::vector<USet<IndexT>>> rules_for_eps_generating;
  ReadRules(input, rules_for_eps_generating);
  ProcEpsGeneratingSymbols(rules_for_eps_generating);
}

template <typename CharT>
void GrammarBase<CharT>::ReadFirstLine(std::basic_istream<CharT>& input) {
  String<CharT> line;
  std::vector<String<CharT>> split_res;
  std::getline<CharT>(input, line, '\n');
  split_res = Split(line, String<CharT>(1, kDelim));
  symbol_to_num_.insert({split_res[0], kStartSymbolInd});
  num_to_symbol_.insert({kStartSymbolInd, split_res[0]});
  symbol_to_num_.insert({split_res[1], kEpsilonInd});
  num_to_symbol_.insert({kEpsilonInd, split_res[1]});
  symbol_to_num_.insert({L"AUXILIARY", kAuxiliaryStartSymbolInd});
  num_to_symbol_.insert({kAuxiliaryStartSymbolInd, L"AUXILIARY"});
}

template <typename CharT>
void GrammarBase<CharT>::ReadSymbols(std::basic_istream<CharT>& input) {
  // nonterminals
  String<CharT> line;
  std::vector<String<CharT>> split_res;
  std::getline(input, line, L'\n');
  split_res = Split(line, String<CharT>(1, kDelim));
  nonterminals_count_ = static_cast<IndexT>(split_res.size() + 1);
  for (IndexT i = 0; i < split_res.size(); ++i) {
    symbol_to_num_.insert({split_res[i], i + 3});
    num_to_symbol_.insert({i + 3, split_res[i]});
  }
  // terminals
  std::getline(input, line, L'\n');
  split_res = Split(line, String<CharT>(1, kDelim));
  terminals_count_ = static_cast<IndexT>(split_res.size());
  for (IndexT i = 0, j = 0; i < split_res.size(); ++i, ++j) {
    if (split_res[i].empty()) {
      std::wcerr
          << L"Empty symbol in terminals\n";  // todo: make more informative
      exit(ExitStatus::IncorrectGrammarInput);
    } else if (split_res[i] == kRulesDelimEscape) {
      symbol_to_num_.insert({String<CharT>(1, kRulesDelimSymbol), -j - 1});
      num_to_symbol_.insert({-j - 1, String<CharT>(1, kRulesDelimSymbol)});
      continue;
    } else if (split_res[i] == kSlashEscape) {
      symbol_to_num_.insert({String<CharT>(1, kSlash), -j - 1});
      num_to_symbol_.insert({-j - 1, String<CharT>(1, kSlash)});
      continue;
    } else if (split_res[i] == kSlashStr && i != split_res.size() - 1 &&
               split_res[i + 1] == kDelimSpecial) {
      symbol_to_num_.insert({String<CharT>(1, kDelim), -j - 1});
      num_to_symbol_.insert({-j - 1, String<CharT>(1, kDelim)});
      --terminals_count_;
      ++i;
      continue;
    } else if (split_res[i].size() > 1) {
      std::wcerr
          << L"Symbol in terminals consists of more then one character\n";
      exit(ExitStatus::IncorrectGrammarInput);
    }
    symbol_to_num_.insert({split_res[i], -j - 1});
    num_to_symbol_.insert({-j - 1, split_res[i]});
  }
}

template <typename CharT>
void GrammarBase<CharT>::ReadRules(
    std::basic_istream<CharT>& input,
    UMap<IndexT, std::vector<USet<IndexT>>>& rules_for_eps_generating) {
  String<CharT> line;
  std::vector<String<CharT>> split_res;
  for (size_t i = 0; i < nonterminals_count_;
       ++i) {  // todo: add case bad reading
    IndexT start_ind = ReadLeftNonterminal(input);
    std::getline(input, line, L' ');   // reading ->
    std::getline(input, line, L'\n');  // reading all right parts
    split_res = Split(line, kRulesDelim);
    for (const auto& right_part : split_res) {
      ReadRightPart(start_ind, right_part, rules_for_eps_generating);
    }
  }
}

template <typename CharT>
typename GrammarBase<CharT>::IndexT GrammarBase<CharT>::ReadLeftNonterminal(
    std::basic_istream<CharT>& input) {
  String<CharT> line;
  std::getline(input, line, L' ');  // reading left nonterminal
  auto iter_symbol_to_num = symbol_to_num_.find(line);
  if (iter_symbol_to_num == symbol_to_num_.end() ||
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
void GrammarBase<CharT>::ReadRightPart(
    IndexT start_ind, const String<CharT>& right_part,
    UMap<IndexT, std::vector<USet<IndexT>>>& rules_for_eps_generating) {
  if (right_part.empty()) {
    std::wcerr << L"Incorrect right part of the rule with left nonterminal ";
    std::wcerr << L'`' << num_to_symbol_[start_ind] << L"`\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  if (right_part == num_to_symbol_[kEpsilonInd]) {
    start_eps_generating_symbols_.insert(start_ind);
    proc_eps_generating_symbols_.insert(start_ind);
    return;
  }
  std::vector<String<CharT>> symbols =
      Split(right_part, String<CharT>(1, kDelim));
  std::vector<IndexT> right_part_inds;
  USet<IndexT> right_part_eps_inds;
  size_t offset_for_error = num_to_symbol_[start_ind].size() + kArrowStr.size();
  size_t j = 0;
  if (symbols[0].empty()) {
    // handle case when `\`` is in the start of the rule
    if (symbols.size() < 3 || symbols[1] != kSlashStr || !symbols[2].empty()) {
      ReadRightPartPrintError(start_ind, offset_for_error, symbols);
      exit(ExitStatus::IncorrectGrammarInput);
    }
    j++;
  }
  for (; j < symbols.size(); ++j) {
    if (symbols[j] == kSlashStr) {
      if (j != symbols.size() - 1 && symbols[j + 1] == kDelimSpecial) {
        ReadRightPartHandleSpecialSymbol(start_ind, offset_for_error, symbols,
                                         right_part_inds, right_part_eps_inds,
                                         j, kDelim);
        continue;
      }
      ReadRightPartPrintError(start_ind, offset_for_error, symbols);
      exit(ExitStatus::IncorrectGrammarInput);
    }
    auto iter_symbol_to_num = symbol_to_num_.find(symbols[j]);
    if (iter_symbol_to_num == symbol_to_num_.end()) {
      for (size_t i = 0; i < symbols[j].size(); ++i) {
        if (symbols[j][i] == kSlash) {
          if (i == symbols[j].size() - 1) {
            ReadRightPartPrintError(start_ind, offset_for_error, symbols);
            exit(ExitStatus::IncorrectGrammarInput);
          }
          if (symbols[j][i + 1] == kSlash) {
            ReadRightPartHandleSpecialSymbol(start_ind, offset_for_error,
                                             symbols, right_part_inds,
                                             right_part_eps_inds, i, kSlash);
            continue;
          }
          if (symbols[j][i + 1] == kRulesDelimSymbol) {
            ReadRightPartHandleSpecialSymbol(
                start_ind, offset_for_error, symbols, right_part_inds,
                right_part_eps_inds, i, kRulesDelimSymbol);
            continue;
          }
        }
        ReadRightPartHandleSymbol(start_ind, offset_for_error, symbols,
                                  right_part_inds, right_part_eps_inds,
                                  symbols[j][i]);
      }
      continue;
    }
    offset_for_error += symbols[j].size() + 1;
    right_part_inds.push_back(iter_symbol_to_num->second);
    right_part_eps_inds.insert(iter_symbol_to_num->second);
  }
  rules_[start_ind].push_back(std::move(right_part_inds));
  rules_for_eps_generating[start_ind].push_back(std::move(right_part_eps_inds));
}

template <typename CharT>
void GrammarBase<CharT>::ReadRightPartHandleSymbol(
    IndexT start_ind, size_t& offset_for_error,
    const std::vector<String<CharT>>& symbols,
    std::vector<IndexT>& right_part_inds, USet<IndexT>& right_part_eps_inds,
    CharT symbol) {
  auto iter_symbol_to_num = symbol_to_num_.find(String<CharT>(1, symbol));
  if (iter_symbol_to_num == symbol_to_num_.end() ||
      IsNonterminal(iter_symbol_to_num->second)) {
    ReadRightPartPrintError(start_ind, offset_for_error, symbols);
    exit(ExitStatus::IncorrectGrammarInput);
  }
  ++offset_for_error;
  right_part_inds.push_back(iter_symbol_to_num->second);
  right_part_eps_inds.insert(iter_symbol_to_num->second);
}

template <typename CharT>
void GrammarBase<CharT>::ReadRightPartHandleSpecialSymbol(
    IndexT start_ind, size_t& offset_for_error,
    const std::vector<String<CharT>>& symbols,
    std::vector<IndexT>& right_part_inds, USet<IndexT>& right_part_eps_inds,
    size_t& i, CharT symbol) {
  auto iter_symbol_to_num = symbol_to_num_.find(String<CharT>(1, symbol));
  if (iter_symbol_to_num == symbol_to_num_.end()) {
    ReadRightPartPrintError(start_ind, offset_for_error, symbols);
    std::wcerr << symbol << " is not a terminal\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  right_part_inds.push_back(iter_symbol_to_num->second);
  right_part_eps_inds.insert(iter_symbol_to_num->second);
  ++i;
}

template <typename CharT>
void GrammarBase<CharT>::ReadRightPartPrintError(
    IndexT start_ind, size_t offset_for_error,
    const std::vector<String<CharT>>& symbols) {
  if (ContainArrow(symbols)) {
    std::wcerr << L"Incorrect number of spaces between "
               << num_to_symbol_[start_ind] << L" and rules_\n";
    exit(ExitStatus::IncorrectGrammarInput);
  }
  std::wcerr << L"Incorrect nonterminal, extra space or missing '`' "
             << L"in right part of the rule:\n";
  std::wcerr << num_to_symbol_[start_ind] << L" -> ";
  PrintRightPartOfRule(std::wcerr, symbols);
  std::wcerr << L'\n';
  for (size_t i = 0; i < offset_for_error; ++i) {
    std::wcerr << L' ';
  }
  std::wcerr << L"^\n";
}

template <typename CharT>
bool GrammarBase<CharT>::ContainArrow(
    const std::vector<String<CharT>>& symbols) const {
  return !symbols.empty() && symbols[0].find(L"->") != symbols[0].npos;
}

template <typename CharT>
void GrammarBase<CharT>::PrintRightPartOfRule(
    std::basic_ostream<CharT>& out, std::vector<String<CharT>> symbols) {
  auto smb_iter = symbols.begin();
  while (true) {
    out << *smb_iter;
    ++smb_iter;
    if (smb_iter == symbols.end()) {
      break;
    }
    out << L'`';
  }
}

template <typename CharT>
void GrammarBase<CharT>::ProcEpsGeneratingSymbols(
    UMap<IndexT, std::vector<USet<IndexT>>>& rules_for_eps_generating) {
  if (proc_eps_generating_symbols_.empty()) {
    return;
  }
  USet<IndexT> unhandled_smb;
  for (IndexT i = 2; i <= nonterminals_count_ + 1; ++i) {
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

template <typename CharT>
template <typename T>
void GrammarBase<CharT>::PrintUSet(const USet<T>& u_set) const {
  for (const auto& elem : u_set) {
    std::cout << elem << " ";
  }
  std::cout << "\n";
}

template <typename CharT>
template <typename T, typename U>
void GrammarBase<CharT>::PrintUMap(const UMap<T, U>& u_map) const {
  for (const auto& elem : u_map) {
    std::cout << '(' << elem.first << ", " << elem.second << ") ";
  }
  std::cout << "\n\n";
}

template <typename CharT>
void GrammarBase<CharT>::PrintUMap(
    const UMap<IndexT, std::vector<std::vector<IndexT>>>& u_map) const {
  for (const auto& elem : u_map) {
    std::cout << elem.first << '\n';
    PrintVec2(elem.second);
  }
  std::cout << "\n";
}

template <typename CharT>
void GrammarBase<CharT>::PrintUMap(
    const UMap<IndexT, std::vector<USet<IndexT>>>& u_map) const {
  for (const auto& elem : u_map) {
    std::cout << elem.first << '\n';
    PrintVecMap(elem.second);
  }
  std::cout << "\n";
}

template <typename CharT>
template <typename T>
void GrammarBase<CharT>::PrintVecMap(const std::vector<USet<T>>& u_map) const {
  for (const auto& line : u_map) {
    std::cout << "(";
    if (line.empty()) {
      std::cout << ") ";
    } else {
      auto iter = line.begin();
      while (iter != line.end()) {
        std::cout << *iter;
        ++iter;
        if (iter == line.end()) {
          std::cout << ")";
        } else {
          std::cout << " ";
        }
      }
    }
  }
  std::cout << "\n";
}

template <typename CharT>
template <typename T>
void GrammarBase<CharT>::PrintVec2(
    const std::vector<std::vector<T>>& vec2) const {
  for (const auto& line : vec2) {
    std::cout << "(";
    for (size_t i = 0; i < line.size() - 1; ++i) {
      std::cout << line[i] << " ";
    }
    if (line.empty()) {
      std::cout << ") ";
    } else {
      std::cout << line.back() << ") ";
    }
  }
  std::cout << "\n";
}