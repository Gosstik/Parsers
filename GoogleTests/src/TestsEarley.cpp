#include <gtest/gtest.h>

#include <random>

#include "BasicEarleyParser.h"

class EarleyBBSConsts {
 protected:
  static constexpr size_t kRandomLowerBound = 5;
  static constexpr size_t kRandomUpperBound = 10;
  static constexpr size_t kRandomNumOfIters = 30;
  static constexpr size_t kStressLowerBound = 9000;
  static constexpr size_t kStressUpperBound = 9999;
  static constexpr size_t kStressNumOfIters = 20;
};

class EarleyBBS1 : public ::testing::Test, public EarleyBBSConsts {
 protected:
  WEarleyParser parser_;
  EarleyBBS1() : parser_("../TestCases/BBS1") {}
};

class EarleyBBS2 : public ::testing::Test, public EarleyBBSConsts {
 protected:
  WEarleyParser parser_;
  EarleyBBS2() : parser_("../TestCases/BBS2") {}
};

TEST(EarleyFinitGrammar, FinitGrammar1) {
  WEarleyParser parser("../TestCases/FinitGrammar1");
  EXPECT_EQ(parser.Parse(L""), true);
  EXPECT_EQ(parser.Parse(L"a"), true);
  EXPECT_EQ(parser.Parse(L"b"), true);
  EXPECT_EQ(parser.Parse(L"c"), true);
  EXPECT_EQ(parser.Parse(L"ab"), true);
  EXPECT_EQ(parser.Parse(L"ac"), true);
  EXPECT_EQ(parser.Parse(L"bc"), true);
  EXPECT_EQ(parser.Parse(L"abc"), true);
  EXPECT_EQ(parser.Parse(L"aabcc"), false);
  EXPECT_EQ(parser.Parse(L"aaa"), false);
  EXPECT_EQ(parser.Parse(L"ccc"), false);
  EXPECT_EQ(parser.Parse(L"accc"), false);
  EXPECT_EQ(parser.Parse(L"aaabbbccc"), false);
  EXPECT_EQ(parser.Parse(L"cba"), false);
  EXPECT_EQ(parser.Parse(L"aba"), false);
  EXPECT_EQ(parser.Parse(L"abba"), false);
  EXPECT_EQ(parser.Parse(L"ca"), false);
  EXPECT_EQ(parser.Parse(L"aabbcca"), false);
  EXPECT_EQ(parser.Parse(L"abcd"), false)
      << "`d` does not belong to language\n";
  EXPECT_EQ(parser.Parse(L"A"), false) << "`A` is nonterminal\n";
  EXPECT_EQ(parser.Parse(L"S"), false) << "`S` is start nonterminal\n";
  EXPECT_EQ(parser.Parse(L"e"), false) << "`e` is alias for empty symbol\n";
}

TEST(EarleyFinitGrammar, FinitGrammar2) {
  WEarleyParser parser("../TestCases/FinitGrammar2");
  EXPECT_EQ(parser.Parse(L""), true);
  EXPECT_EQ(parser.Parse(L"a"), true);
  EXPECT_EQ(parser.Parse(L"b"), true);
  EXPECT_EQ(parser.Parse(L"c"), true);
  EXPECT_EQ(parser.Parse(L"ab"), true);
  EXPECT_EQ(parser.Parse(L"ac"), true);
  EXPECT_EQ(parser.Parse(L"bc"), true);
  EXPECT_EQ(parser.Parse(L"abc"), true);
  EXPECT_EQ(parser.Parse(L"aabcc"), true);
  EXPECT_EQ(parser.Parse(L"aaa"), true);
  EXPECT_EQ(parser.Parse(L"ccc"), true);
  EXPECT_EQ(parser.Parse(L"accc"), true);
  EXPECT_EQ(parser.Parse(L"aaabbbccc"), true);
  EXPECT_EQ(parser.Parse(L"cba"), false);
  EXPECT_EQ(parser.Parse(L"aba"), false);
  EXPECT_EQ(parser.Parse(L"abba"), false);
  EXPECT_EQ(parser.Parse(L"ca"), false);
  EXPECT_EQ(parser.Parse(L"aabbcca"), false);
  EXPECT_EQ(parser.Parse(L"abcd"), false)
      << "`d` does not belong to language\n";
  EXPECT_EQ(parser.Parse(L"A"), false) << "`A` is nonterminal\n";
  EXPECT_EQ(parser.Parse(L"S"), false) << "`S` is start nonterminal\n";
  EXPECT_EQ(parser.Parse(L"e"), false) << "`e` is alias for empty symbol\n";
}

TEST(EarleyPalindromes, Palindromes) {
  WEarleyParser parser("../TestCases/Palindromes");
  EXPECT_EQ(parser.Parse(L""), true);
  EXPECT_EQ(parser.Parse(L"a"), true);
  EXPECT_EQ(parser.Parse(L"bb"), true);
  EXPECT_EQ(parser.Parse(L"aba"), true);
  EXPECT_EQ(parser.Parse(L"aaaaaa"), true);
  EXPECT_EQ(parser.Parse(L"bbbaaabbb"), true);
  EXPECT_EQ(parser.Parse(L"babababababababab"), true);
  EXPECT_EQ(parser.Parse(L"ba"), false);
  EXPECT_EQ(parser.Parse(L"abab"), false);
  EXPECT_EQ(parser.Parse(L"aaaab"), false);
  EXPECT_EQ(parser.Parse(L"aababababa"), false);
  EXPECT_EQ(parser.Parse(L"babaaaabb"), false);
}

TEST(EarleyPalindromes, NonPalindromes) {
  WEarleyParser parser("../TestCases/NotPalindromes");
  EXPECT_EQ(parser.Parse(L"ba"), true);
  EXPECT_EQ(parser.Parse(L"abab"), true);
  EXPECT_EQ(parser.Parse(L"aaaab"), true);
  EXPECT_EQ(parser.Parse(L"aababababa"), true);
  EXPECT_EQ(parser.Parse(L"babaaaabb"), true);
  EXPECT_EQ(parser.Parse(L""), false);
  EXPECT_EQ(parser.Parse(L"a"), false);
  EXPECT_EQ(parser.Parse(L"bb"), false);
  EXPECT_EQ(parser.Parse(L"aba"), false);
  EXPECT_EQ(parser.Parse(L"aaaaaa"), false);
  EXPECT_EQ(parser.Parse(L"bbbaaabbb"), false);
  EXPECT_EQ(parser.Parse(L"babababababababab"), false);
}

TEST_F(EarleyBBS1, BBS1) {
  EXPECT_EQ(parser_.Parse(L""), true);
  EXPECT_EQ(parser_.Parse(L"()"), true);
  EXPECT_EQ(parser_.Parse(L"()()"), true);
  EXPECT_EQ(parser_.Parse(L"(())"), true);
  EXPECT_EQ(parser_.Parse(L"(()())"), true);
  EXPECT_EQ(parser_.Parse(L"(())(())"), true);
  EXPECT_EQ(parser_.Parse(L"("), false);
  EXPECT_EQ(parser_.Parse(L")("), false);
  EXPECT_EQ(parser_.Parse(L"())("), false);
  EXPECT_EQ(parser_.Parse(L"(((("), false);
  EXPECT_EQ(parser_.Parse(L"(()))"), false);
  EXPECT_EQ(parser_.Parse(L"((()(())"), false);
}

TEST_F(EarleyBBS1, BBS1Random) {
  std::mt19937_64 gen(std::random_device().operator()());
  std::uniform_int_distribution<size_t> distrib(kRandomLowerBound,
                                                kRandomUpperBound);
  for (size_t iter_i = 0; iter_i < kRandomNumOfIters; ++iter_i) {
    size_t brackets_count = distrib(gen);
    brackets_count += brackets_count % 2;  // to make 'brackets_count' even
    std::wstring sequence;
    size_t left_count = 0;
    size_t left_balance = 0;
    while (brackets_count - left_count != 0) {
      if (left_balance == 0) {
        sequence += L'(';
        ++left_count;
        ++left_balance;
        continue;
      }
      if (gen() % 2 == 0) {
        sequence += L'(';
        ++left_count;
        ++left_balance;
      } else {
        sequence += L')';
        --left_balance;
      }
    }
    while (left_balance > 0) {
      sequence += L')';
      --left_balance;
    }
    EXPECT_EQ(parser_.Parse(sequence), true)
        << "Fail sequence: " << sequence << '\n';
  }
}

TEST_F(EarleyBBS1, BBS1Stress) {
  std::mt19937_64 gen(std::random_device().operator()());
  std::uniform_int_distribution<size_t> distrib(kStressLowerBound,
                                                kStressUpperBound);
  for (size_t iter_i = 0; iter_i < kStressNumOfIters; ++iter_i) {
    size_t brackets_count = distrib(gen);
    brackets_count += brackets_count % 2;  // to make 'brackets_count' even
    std::wstring sequence;
    size_t left_count = 0;
    size_t left_balance = 0;
    while (brackets_count - left_count != 0) {
      if (left_balance == 0) {
        sequence += L'(';
        ++left_count;
        ++left_balance;
        continue;
      }
      if (gen() % 2 == 0) {
        sequence += L'(';
        ++left_count;
        ++left_balance;
      } else {
        sequence += L')';
        --left_balance;
      }
    }
    while (left_balance > 0) {
      sequence += L')';
      --left_balance;
    }
    EXPECT_EQ(parser_.Parse(sequence), true)
        << "Fail sequence: " << sequence << '\n';
  }
}

// todo: make random and stress version
TEST_F(EarleyBBS2, BBS2) {
  EXPECT_EQ(parser_.Parse(L"[]"), true);
  EXPECT_EQ(parser_.Parse(L"()[]{}"), true);
  EXPECT_EQ(parser_.Parse(L"({[]})"), true);
  EXPECT_EQ(parser_.Parse(L"[[]]{{}}[]()"), true);
  EXPECT_EQ(parser_.Parse(L"[}"), false);
  EXPECT_EQ(parser_.Parse(L"()[][}"), false);
  EXPECT_EQ(parser_.Parse(L"[{()})"), false);
  EXPECT_EQ(parser_.Parse(L"[[][]"), false);
}

TEST(EarleyAmbiguous, Ambiguous1) {
  WEarleyParser parser("../TestCases/Ambiguous1");
  EXPECT_EQ(parser.Parse(L"abb"), true);
  EXPECT_EQ(parser.Parse(L"aabbbb"), true);
  EXPECT_EQ(parser.Parse(L"aaabbb"), true);
  EXPECT_EQ(parser.Parse(L"ab"), false);
  EXPECT_EQ(parser.Parse(L"aaa"), false);
  EXPECT_EQ(parser.Parse(L"aaaabbb"), false);
  EXPECT_EQ(parser.Parse(L"abbb"), false);
}

TEST(EarleyAmbiguous, Ambiguous2) {
  WEarleyParser parser("../TestCases/Ambiguous2");
  EXPECT_EQ(parser.Parse(L"acb"), true);
  EXPECT_EQ(parser.Parse(L"aaaacbbbb"), true);
  EXPECT_EQ(parser.Parse(L"acbaacbb"), true);
  EXPECT_EQ(parser.Parse(L"aaacbbbacbaacbb"), true);
  EXPECT_EQ(parser.Parse(L""), false);
  EXPECT_EQ(parser.Parse(L"ab"), false);
  EXPECT_EQ(parser.Parse(L"aaccbb"), false);
  EXPECT_EQ(parser.Parse(L"aaacbbacb"), false);
  EXPECT_EQ(parser.Parse(L"acbaacbbaacbbb"), false);
}

TEST(EarleyEscapeSymbols, EscapeSymbols) {
  WEarleyParser parser("../TestCases/EscapeSymbols");
  EXPECT_EQ(parser.Parse(L"A"), true);
  EXPECT_EQ(parser.Parse(L"\\|"), true) << "right part is |\n";
  EXPECT_EQ(parser.Parse(L"A`\\|`A"), true) << "result right part is A`\\|`A\n";
  EXPECT_EQ(parser.Parse(L"`\\``"), true) << "right part is `\n";
  EXPECT_EQ(parser.Parse(L"A`\\``A"), true) << "result right part is A\\`A\n";
  EXPECT_EQ(parser.Parse(L"\\\\"), true) << "right part is \\\n";
  EXPECT_EQ(parser.Parse(L"A`\\\\`A"), true)
      << "Result right part is A`\\\\`A\n";
  EXPECT_EQ(parser.Parse(L"A`\\|`A`\\``A`\\\\`A"), true);
  EXPECT_EQ(parser.Parse(L"A | A`A"), true);
  EXPECT_EQ(parser.Parse(L"A`\\|`A | A`\\\\`A | A`\\\\`A"), true);
  EXPECT_EQ(parser.Parse(L""), false);
  EXPECT_EQ(parser.Parse(L"`"), false);
  EXPECT_EQ(parser.Parse(L"A`A`"), false);
  EXPECT_EQ(parser.Parse(L"|"), false);
  EXPECT_EQ(parser.Parse(L"A|A"), false);
  EXPECT_EQ(parser.Parse(L"\\"), false);
  EXPECT_EQ(parser.Parse(L"A`\\`A"), false);
}

TEST(EarleyLongSymbols, LongEmptySymbol) {
  WEarleyParser parser("../TestCases/LongEmptySymbol");
  EXPECT_EQ(parser.Parse(L""), true);
  EXPECT_EQ(parser.Parse(L"ab"), true);
  EXPECT_EQ(parser.Parse(L"aaabbb"), true);
  EXPECT_EQ(parser.Parse(L"b"), true);
  EXPECT_EQ(parser.Parse(L"ba"), false);
  EXPECT_EQ(parser.Parse(L"aaabbba"), false);
  EXPECT_EQ(parser.Parse(L"aababb"), false);
}

TEST(EarleyLongSymbols, LongNonterminals1) {
  WEarleyParser parser("../TestCases/LongNonterminals1");
  EXPECT_EQ(parser.Parse(L""), true);
  EXPECT_EQ(parser.Parse(L"ab"), true);
  EXPECT_EQ(parser.Parse(L"abbcca"), true);
  EXPECT_EQ(parser.Parse(L"aaabbbbbccca"), true);
  EXPECT_EQ(parser.Parse(L"bbcccccaaa"), true);
  EXPECT_EQ(parser.Parse(L"abc"), false);
  EXPECT_EQ(parser.Parse(L"bb"), false);
  EXPECT_EQ(parser.Parse(L"abba"), false);
  EXPECT_EQ(parser.Parse(L"caa"), false);
}

TEST(EarleyLongSymbols, LongNonterminals2) {
  WEarleyParser parser("../TestCases/LongNonterminals2");
  EXPECT_EQ(parser.Parse(L"Max is an immaculate student."), true);
  EXPECT_EQ(parser.Parse(L"Dan is an impartial teacher."), true);
  EXPECT_EQ(parser.Parse(L"Jack is a brilliant programmer."), true);
  EXPECT_EQ(parser.Parse(L"Thomas is a carefree designer."), true);
  EXPECT_EQ(parser.Parse(L"Max is a immaculate student."), false);
  EXPECT_EQ(parser.Parse(L"Dan is impartial teacher."), false);
  EXPECT_EQ(parser.Parse(L"Jack is a brilliant programmer"), false);
  EXPECT_EQ(parser.Parse(L"thomas is a carefree designer."), false);
}