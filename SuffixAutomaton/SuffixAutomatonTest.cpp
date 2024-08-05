#include <iostream>

#include "SuffixAutomaton.h"

int main() {
  SuffixAutomaton SAM;
  SAM.insert("aabab");
  std::cout << SAM.match("bab") << std::endl;
  std::cout << SAM.differentSubstrings() << std::endl;
  std::cout << SAM.occurrences("ab") << std::endl;
  std::cout << SAM.find("b") << std::endl;
  std::cout << SAM.logestCommonSubstring("cdbab") << std::endl;
  return 0;
}