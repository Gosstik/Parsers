#include <gtest/gtest.h>

#include <random>

#include "BasicLR1Parser.h"

// todo: add tests for non-LR grammar
// 1. shift-reduce conflict: palindromes
// 2. reduce-reduce conflict:

class LR1BBSConsts {
 protected:
  static constexpr size_t kRandomLowerBound = 5;
  static constexpr size_t kRandomUpperBound = 10;
  static constexpr size_t kRandomNumOfIters = 30;
  static constexpr size_t kStressLowerBound = 999000;
  static constexpr size_t kStressUpperBound = 1000000;
  static constexpr size_t kStressNumOfIters = 10;
};

class LR1BBS1 : public ::testing::Test, public LR1BBSConsts {
 protected:
  WLRParser<1> parser_;
  LR1BBS1() : parser_("../TestCases/BBS1") {}
};

class LR1BBS2 : public ::testing::Test, public LR1BBSConsts {
 protected:
  WLRParser<1> parser_;
  LR1BBS2() : parser_("../TestCases/BBS2") {}
};

TEST(LR1Test1, Test1) {
  WLRParser<1> parser("../TestCases/LR1/Test1");
  EXPECT_EQ(parser.Parse(L"bb"), true);
  EXPECT_EQ(parser.Parse(L"abab"), true);
  EXPECT_EQ(parser.Parse(L"aabaab"), true);
  EXPECT_EQ(parser.Parse(L"aaabaaab"), true);
  EXPECT_EQ(parser.Parse(L""), false);
  EXPECT_EQ(parser.Parse(L"a"), false);
  EXPECT_EQ(parser.Parse(L"aa"), false);
  EXPECT_EQ(parser.Parse(L"ba"), false);
  EXPECT_EQ(parser.Parse(L"aabaa"), false);
  EXPECT_EQ(parser.Parse(L"ac"), false) << "`c` does not belong to language\n";
  EXPECT_EQ(parser.Parse(L"A"), false) << "`A` is nonterminal\n";
  EXPECT_EQ(parser.Parse(L"S"), false) << "`S` is start nonterminal\n";
  EXPECT_EQ(parser.Parse(L"e"), false) << "`e` is alias for empty symbol\n";
  EXPECT_EQ(parser.Parse(L"abe"), false) << "`e` is alias for empty symbol\n";
}

TEST(LR1Test2, Test2) {
  WLRParser<1> parser("../TestCases/LR1/Test2");
  EXPECT_EQ(parser.Parse(L""), true);
  EXPECT_EQ(parser.Parse(L"b"), true);
  EXPECT_EQ(parser.Parse(L"ca"), true);
  EXPECT_EQ(parser.Parse(L"cba"), true);
  EXPECT_EQ(parser.Parse(L"cbbba"), true);
  EXPECT_EQ(parser.Parse(L"a"), false);
  EXPECT_EQ(parser.Parse(L"ba"), false);
  EXPECT_EQ(parser.Parse(L"ac"), false);
  EXPECT_EQ(parser.Parse(L"bc"), false);
  EXPECT_EQ(parser.Parse(L"bbbc"), false);
  EXPECT_EQ(parser.Parse(L"cbbac"), false);
  EXPECT_EQ(parser.Parse(L"e"), false) << "`e` is alias for empty symbol\n";
}

TEST(LR1Test3, Test3) {
  WLRParser<1> parser("../TestCases/LR1/Test3");
  EXPECT_EQ(parser.Parse(L"b"), true);
  EXPECT_EQ(parser.Parse(L"ca"), true);
  EXPECT_EQ(parser.Parse(L"cba"), true);
  EXPECT_EQ(parser.Parse(L"cbbba"), true);
  EXPECT_EQ(parser.Parse(L""), false);
  EXPECT_EQ(parser.Parse(L"ba"), false);
  EXPECT_EQ(parser.Parse(L"ac"), false);
  EXPECT_EQ(parser.Parse(L"bc"), false);
  EXPECT_EQ(parser.Parse(L"bbbc"), false);
  EXPECT_EQ(parser.Parse(L"cbbac"), false);
}

TEST(LRFinitGrammar, FinitGrammar1) {
  WLRParser<1> parser("../TestCases/FinitGrammar1");
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

TEST(LRFinitGrammar, FinitGrammar2) {
  WLRParser<1> parser("../TestCases/FinitGrammar2");
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

TEST_F(LR1BBS1, BBS1) {
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

TEST_F(LR1BBS1, BBS1Random) {
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

TEST_F(LR1BBS1, BBS1Stress) {
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
TEST_F(LR1BBS2, BBS2) {
  EXPECT_EQ(parser_.Parse(L"[]"), true);
  EXPECT_EQ(parser_.Parse(L"()[]{}"), true);
  EXPECT_EQ(parser_.Parse(L"({[]})"), true);
  EXPECT_EQ(parser_.Parse(L"[[]]{{}}[]()"), true);
  EXPECT_EQ(parser_.Parse(L"[}"), false);
  EXPECT_EQ(parser_.Parse(L"()[][}"), false);
  EXPECT_EQ(parser_.Parse(L"[{()})"), false);
  EXPECT_EQ(parser_.Parse(L"[[][]"), false);
}