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

  tree.remove("abcbc");
  std::cout << "==#2==\n";
  printTree(tree);

  tree.remove("abc");
  std::cout << "==#3==\n";
  printTree(tree);

  assert(tree.empty());

  std::cout << "test success" << std::endl;
  return 0;
}