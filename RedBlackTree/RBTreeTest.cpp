#include <vector>

#include "RBTree.h"

int main() {
  RBTree<int, int> tree;
  std::vector nums = {10, 20, 30, 15, 12, 25, 28, 27};

  std::cout << "## UPSERT ##\n";
  for (int num : nums) {
    tree.upsert(num, num);
    tree.print();
    std::cout << '\n';
  }

  std::cout << "\n## GET ##\n";
  for (int num : nums) {
    std::cout << tree.get(num, -1) << '\n';
  }

  std::cout << "\n## ITERATOR ##\n";
  for (auto iter = tree.begin(); iter != tree.end(); iter++) {
    std::cout << iter->second << '\n';
  }

  std::cout << "\n## REMOVE ##\n";
  for (int num : nums) {
    tree.remove(num);
    tree.print();
    std::cout << '\n';
  }

  return 0;
}