#pragma once

#include <string>
#include <vector>

/// Knuth–Morris–Pratt algorithm

template <typename CharT>
using String = std::basic_string<CharT>;

template <typename CharT>
std::vector<String<CharT>> Split(const String<CharT>& src,
                                 const String<CharT>& delim);

namespace dev {
// LPS - longest proper prefix
template <typename CharT>
void CreateLPSArray(const String<CharT>& pattern, size_t pat_len,
                    std::vector<size_t>& lps) {
  size_t len = 0;  // length of the previous longest prefix suffix
  lps[0] = 0;      // lps[0] is always 0
  size_t i = 1;
  while (i < pat_len) {
    if (pattern[i] == pattern[len]) {
      lps[i++] = ++len;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i++] = 0;
      }
    }
  }
}

template <typename CharT>
void KMPSearch(const String<CharT>& pattern, const String<CharT>& string,
               std::vector<size_t>& match_indexes) {
  size_t pat_len = pattern.size();
  size_t str_len = string.size();

  std::vector<size_t> lps(pat_len);
  CreateLPSArray(pattern, pat_len, lps);

  size_t i = 0;  // index for string
  size_t j = 0;  // index for pattern
  while ((str_len - i) >= (pat_len - j)) {
    if (pattern[j] == string[i]) {
      j++;
      i++;
    }
    if (j == pat_len) {
      match_indexes.push_back(i - j);
      j = lps[j - 1];
    }
    // mismatch after j matches
    else if (i < str_len && pattern[j] != string[i]) {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        ++i;
      }
    }
  }
}
}  // namespace dev

template <typename CharT>
std::vector<String<CharT>> Split(const String<CharT>& src,
                                 const String<CharT>& delim) {
  if (src.empty() || src.size() < delim.size()) {
    return {src};
  }
  std::vector<size_t> match_indexes;
  dev::KMPSearch(delim, src, match_indexes);
  std::vector<String<CharT>> result;
  size_t prev_match_ind = 0;
  for (auto index : match_indexes) {
    size_t substr_size = index - prev_match_ind;
    result.emplace_back(src.substr(prev_match_ind, substr_size));
    prev_match_ind = index + delim.size();
  }
  result.emplace_back(src.substr(prev_match_ind));
  return result;
}