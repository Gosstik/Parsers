#include <iostream>

#include "BasicEarleyParser.h"
#include "BasicLR1Parser.h"

int main() {
  WLRParser<1> lr_parser("../src/GrammarText/GrammarText");
  lr_parser.PrintGrammar(std::wcout);
  std::wcout << lr_parser.Parse(L"") << '\n';

  WEarleyParser earley_parser("../src/GrammarText/GrammarText");
//  earley_parser.PrintGrammar(std::wcout);
  std::wcout << earley_parser.Parse(L"") << '\n';

  return 0;
}

//EXPECT_EQ(parser.Parse(L""), true);
//EXPECT_EQ(parser.Parse(L"a"), true);
//EXPECT_EQ(parser.Parse(L"b"), true);
//EXPECT_EQ(parser.Parse(L"c"), true);
//EXPECT_EQ(parser.Parse(L"ac"), true);
//EXPECT_EQ(parser.Parse(L"abc"), true);
//EXPECT_EQ(parser.Parse(L"aabcc"), true);
//EXPECT_EQ(parser.Parse(L"aaa"), true);
//EXPECT_EQ(parser.Parse(L"ccc"), true);
//EXPECT_EQ(parser.Parse(L"accc"), true);
//EXPECT_EQ(parser.Parse(L"aaabbbccc"), true);
//EXPECT_EQ(parser.Parse(L"cba"), false);
//EXPECT_EQ(parser.Parse(L"aba"), false);
//EXPECT_EQ(parser.Parse(L"abba"), false);
//EXPECT_EQ(parser.Parse(L"ca"), false);
//EXPECT_EQ(parser.Parse(L"aabbcca"), false);
//EXPECT_EQ(parser.Parse(L"abcd"), false);
//EXPECT_EQ(parser.Parse(L"A"), false) << "A is nonterminal";
//EXPECT_EQ(parser.Parse(L"S"), false) << "S is start nonterminal";
//EXPECT_EQ(parser.Parse(L"e"), false) << "e is name for empty symbol";