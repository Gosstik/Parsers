#pragma once

#include <string>
#include <vector>

/// Knuth–Morris–Pratt algorithm

namespace utl {
template <typename CharT>
using BasicString = std::basic_string<CharT>;

template <typename CharT>
std::vector<BasicString<CharT>> Split(const BasicString<CharT>& src,
                                      const BasicString<CharT>& delim);
// LPS - longest proper prefix
template <typename CharT>
void CreateLPSArray(const BasicString<CharT>& pattern, size_t pat_len,
                    std::vector<size_t>& lps) {
  size_t len = 0;  // length of the previous longest prefix suffix
  lps[0] = 0;      // lps[0] is always 0
  size_t pos = 1;
  while (pos < pat_len) {
    if (pattern[pos] == pattern[len]) {
      lps[pos++] = ++len;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[pos++] = 0;
      }
    }
  }
}

template <typename CharT>
void KMPSearch(const BasicString<CharT>& pattern, const BasicString<CharT>& string,
               std::vector<size_t>& match_indexes) {
  size_t pat_len = pattern.size();
  size_t str_len = string.size();

  std::vector<size_t> lps(pat_len);
  CreateLPSArray(pattern, pat_len, lps);

  size_t pos_s = 0;  // index for string
  size_t pos_p = 0;  // index for pattern
  while ((str_len - pos_s) >= (pat_len - pos_p)) {
    if (pattern[pos_p] == string[pos_s]) {
      pos_p++;
      pos_s++;
    }
    if (pos_p == pat_len) {
      match_indexes.push_back(pos_s - pos_p);
      pos_p = lps[pos_p - 1];
    }
    // mismatch after j matches
    else if (pos_s < str_len && pattern[pos_p] != string[pos_s]) {
      if (pos_p != 0) {
        pos_p = lps[pos_p - 1];
      } else {
        ++pos_s;
      }
    }
  }
}

template <typename CharT>
std::vector<BasicString<CharT>> Split(const BasicString<CharT>& src,
                                      const BasicString<CharT>& delim) {
  if (src.empty() || src.size() < delim.size()) {
    return {src};
  }
  std::vector<size_t> match_indexes;
  utl::KMPSearch(delim, src, match_indexes);
  std::vector<BasicString<CharT>> result;
  size_t prev_match_ind = 0;
  for (auto index : match_indexes) {
    size_t substr_size = index - prev_match_ind;
    result.emplace_back(src.substr(prev_match_ind, substr_size));
    prev_match_ind = index + delim.size();
  }
  result.emplace_back(src.substr(prev_match_ind));
  return result;
}
}  // namespace dev