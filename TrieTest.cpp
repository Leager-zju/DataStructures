#include <assert.h>

#include <iostream>

#include "Trie.h"

void printTree(PrefixTrie& tree) {
  auto&& vec = tree.toVector();
  for (auto&& str : vec) {
    std::cout << str << '\n';
  }
}

int main() {
  PrefixTrie tree;
  tree.insert("abc");
  tree.insert("abcbc");
  std::cout << "==#1==\n";
  printTree(tree);

  if (!tree.remove("abcbc")) {
    std::cerr << "remove 'abcbc' failed" << std::endl;
    exit(-1);
  };
  std::cout << "==#2==\n";
  printTree(tree);

  if (!tree.remove("abc")) {
    std::cerr << "remove 'abc' failed" << std::endl;
    exit(-1);
  };
  std::cout << "==#3==\n";
  printTree(tree);

  assert(tree.empty());

  std::cout << "test success" << std::endl;
  return 0;
}