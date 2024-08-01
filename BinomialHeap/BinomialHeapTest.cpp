#include "BinomialHeap.h"

/**
 *      H
 *     /
 *    4 ——— 1
 *   /     /
 *  6     3 ——— 2
 *             /
 *            5
 */

int main() {
  std::cout << "## PUSH ##\n";
  BinomialHeap<int> heap;
  {
    heap.push(1);
    heap.push(3);
    heap.push(5);
    heap.push(2);
    heap.push(4);

    heap.print();
  }

  std::cout << "\n## MERGE ##\n";
  {
    BinomialHeap<int> heap2;
    heap2.push(6);

    heap.merge(std::move(heap2));
    heap.print();
  }

  std::cout << "\n## POP ##\n";
  {
    heap.pop();
    heap.pop();
    heap.pop();
    heap.print();
  }
  return 0;
}