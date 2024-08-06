#include "SkipList.h"

int main() {
  SkipList<int, int> list;

  std::cout << "## UPSERT ##\n";
  for (int i = 0; i < 100; i++) {
    list.upsert(i, i);
  }
  list.print();

  std::cout << "\n## ERASE ##\n";
  for (int i = 0; i < 50; i++) {
    list.erase(i * 2);
  }
  list.print();
  return 0;
}