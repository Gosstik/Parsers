#include <iostream>

#include "BasicEarleyParser.h"
#include "BasicLR1Parser.h"

int main() {
  std::wstring word;
  size_t lines_count;
  std::wcout << "Enter word for checking: ";
  std::wcin >> word;

  WEarleyParser earley_parser("../GrammarText/GrammarText.txt");
  std::wcout << L"Source grammar:\n";
  earley_parser.PrintGrammar(std::wcout);
  std::wcout << "\nResult: " << std::endl;
  std::wcout << "Earley: " << std::boolalpha << earley_parser.Parse(word) << '\n';

  WLRParser<1> lr1_parser("../GrammarText/GrammarText.txt");
  std::wcout << "LR(1): " << std::boolalpha << lr1_parser.Parse(word) << '\n';
  return 0;
}