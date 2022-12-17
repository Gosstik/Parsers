#include <iostream>
#include <fstream>

#include "BasicEarleyParser.h"

int main() {
  WEarleyParser earley_parser("../src/GrammarText/GrammarText");
  earley_parser.PrintGrammar(std::wcout);
  std::wcout << earley_parser.Parse(L"`\\``") << '\n'; // todo: fix
//  std::wcout << earley_parser.Parse(L"a`\\|`a") << '\n'; // todo: fix
//  std::wcout << earley_parser.Parse(L"a`\\``a") << '\n'; // todo: fix
//  std::wcout << earley_parser.Parse(L"a`\\\\`a") << '\n'; // todo: fix
  return 0;
}
