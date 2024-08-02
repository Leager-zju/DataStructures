#include "Buddy.h"

int main() {
  Buddy<8> buddy;
  std::cout << "## ALLOC ##\n";
  {
    uint32_t off1 = buddy.alloc(1);
    std::cout << "alloc 1B at offset " << off1 << '\n';
    buddy.print();

    uint32_t off2 = buddy.alloc(2);
    std::cout << "alloc 2B at offset " << off2 << '\n';
    buddy.print();

    uint32_t off3 = buddy.alloc(3);
    std::cout << "alloc 3B at offset " << off3 << '\n';
    buddy.print();
  }

  std::cout << "\n## FREE ##\n";
  {
    buddy.free(0, 1);
    std::cout << "free 1B at offset 0" << '\n';
    buddy.print();

    buddy.free(2, 2);
    std::cout << "free 2B at offset 2" << '\n';
    buddy.print();

    buddy.free(4, 3);
    std::cout << "free 3B at offset 4" << '\n';
    buddy.print();
  }

  return 0;
}