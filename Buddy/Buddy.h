#pragma once

#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>

template <int Size>
class Buddy {
 public:
  Buddy() {
    uint32_t index = ROOT;
    uint32_t levelNodes = 1;
    for (uint32_t nodeSize = Size; nodeSize >= 1; nodeSize >>= 1) {
      for (uint32_t n = 0; n < levelNodes; n++) {
        longest_[index++] = binaryDigits(nodeSize);
      }
      levelNodes <<= 1;
    }
  }
  ~Buddy() = default;

  Buddy(const Buddy&) = delete;
  Buddy& operator=(const Buddy&) = delete;
  Buddy(Buddy&&) = delete;
  Buddy& operator=(Buddy&&) = delete;

  uint32_t alloc(uint32_t size) {
    size = nextPow2(size);
    if (getNodeLongestSize(ROOT) < size) {
      return -1;
    }

    uint32_t curNode = ROOT;
    while (getNodeSize(curNode) > size) {
      if (getNodeLongestSize(leftChild(curNode)) >= size) {
        curNode = leftChild(curNode);
      } else {
        curNode = rightChild(curNode);
      }
    }

    uint32_t offset = getNodeOffset(curNode);
    longest_[curNode] = 0;
    while (curNode != ROOT) {
      curNode = parent(curNode);
      longest_[curNode] =
          std::max(longest_[leftChild(curNode)], longest_[rightChild(curNode)]);
    }
    return offset;
  }

  void free(uint32_t offset, uint32_t size) {
    size = nextPow2(size);

    uint32_t curNode = ROOT;
    while (true) {
      uint32_t nodeOffset = getNodeOffset(curNode);
      uint32_t nodeSize = getNodeSize(curNode);

      if (offset == nodeOffset) {
        if (size > nodeSize) {
          throw "Invalid size free at offset" + std::to_string(offset);
        }
        if (size == nodeSize) {
          if (longest_[curNode] > 0) {
            throw "Double Free at offset " + std::to_string(offset);
          }
          longest_[curNode] = binaryDigits(nodeSize);
          break;
        }
      }

      uint32_t mid = std::midpoint(nodeOffset, nodeSize);
      if (offset < mid) {
        curNode = leftChild(curNode);
      } else {
        curNode = rightChild(curNode);
      }
    }

    while (curNode != ROOT) {
      curNode = parent(curNode);
      if (getNodeLongestSize(leftChild(curNode)) !=
              getNodeSize(leftChild(curNode)) ||
          getNodeLongestSize(rightChild(curNode)) !=
              getNodeSize(rightChild(curNode))) {
        break;
      }
      longest_[curNode] = binaryDigits(getNodeSize(curNode));
    }
  }

  void print() {
    char str[Size + 1];
    toString(ROOT, str);
    str[Size] = '\0';
    std::cout << "Utilization: " << str << '\n';
  }

 private:
  inline bool isPow2(uint32_t x) { return (x & (-x)) == x; }
  inline uint32_t nextPow2(uint32_t x) {
    if (isPow2(x)) return x;

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
  }
  inline uint32_t binaryDigits(uint32_t x) {
    uint32_t d = 0;
    while (x) {
      ++d;
      x >>= 1;
    }
    return d;
  }

  inline uint32_t leftChild(uint32_t index) { return index << 1; }

  inline uint32_t parent(uint32_t index) { return index >> 1; }

  inline uint32_t rightChild(uint32_t index) { return (index << 1) + 1; }

  inline uint32_t getNodeOffset(uint32_t index) {
    return (index ^ (1 << (binaryDigits(index) - 1))) * getNodeSize(index);
  }

  inline uint32_t getNodeSize(uint32_t index) {
    return Size >> (binaryDigits(index) - 1);
  }

  inline uint32_t getNodeLongestSize(uint32_t index) {
    return longest_[index] == 0 ? 0 : 1 << (longest_[index] - 1);
  }

  void toString(uint32_t index, char* out) {
    if (index > 2 * Size) return;
    uint32_t start = getNodeOffset(index);
    uint32_t longest = getNodeLongestSize(index);
    uint32_t size = getNodeSize(index);
    if (longest == 0) {
      while (size--) {
        out[start++] = '*';
      }
    } else if (longest == size) {
      while (size--) {
        out[start++] = '_';
      }
    } else {
      toString(leftChild(index), out);
      toString(rightChild(index), out);
    }
  }

 private:
  static const uint32_t ROOT = 1;

  uint8_t longest_[2 * Size];
};