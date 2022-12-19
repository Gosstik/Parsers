#include <iostream>

#include "BasicEarleyParser.h"
#include "BasicLR1Parser.h"

int main() {
  WLRParser<1> lr_parser("../src/GrammarText/GrammarText");
  lr_parser.PrintGrammar(std::wcout);
  std::wcout << lr_parser.Parse(L"ca") << '\n';

  WEarleyParser earley_parser("../src/GrammarText/GrammarText");
  std::wcout << earley_parser.Parse(L"ca") << '\n';
  return 0;
}