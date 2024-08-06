#pragma once

#include <cstdint>
#include <iostream>
#include <random>

template <class T>
struct comp {
  int operator()(const T& lhs, const T& rhs) const {
    if (lhs == rhs) return 0;
    if (lhs < rhs) return -1;
    return 1;
  }
};

template <class Key, class Value, class Compare = comp<Value>>
class SkipList {
  using level_t = int32_t;
  static constexpr level_t MAXLEVEL = 16;

  struct SkipListNode {
    SkipListNode() = default;
    explicit SkipListNode(const Key& k, const Value& v) : key(k), value(v) {}

    Key key;
    Value value;
    SkipListNode* next[MAXLEVEL + 1] = {0};
  };

 public:
  SkipList() : globalMaxLevel_(0), head_(new SkipListNode) {}
  ~SkipList() {
    SkipListNode* p = head_->next[0];
    while (head_) {
      delete head_;
      head_ = p;
      if (p) p = p->next[0];
    }
  }

  /**
   * @brief Return true if the skiplist is empty, false else
   *
   * @return true
   * @return false
   */
  inline bool empty() const { return head_->next[0] != nullptr; }

  /**
   * @brief Returns the corresponding Value according to the Key. If the node
   * does not exist, returns the default Value.
   *
   * @param[in] key
   * @param[in] defaultValue
   * @return const Value&
   */
  const Value& find(const Key& key, const Value& defaultValue) const {
    SkipListNode* node = head_;
    for (level_t curLevel = MAXLEVEL; curLevel >= 0; curLevel--) {
      while (node->next[curLevel] &&
             compareFunc_(node->next[curLevel]->key, key) < 0) {
        node = node->next[curLevel];
      }

      if (node->next[curLevel] &&
          compareFunc_(node->next[curLevel]->key, key) == 0) {
        return node->next[0]->value;
      }
    }

    return defaultValue;
  }

  /**
   * @brief If the Key exists in the skiplist, update the Value, otherwise add a
   * new entry
   *
   * @param[in] key
   * @param[in] value
   */
  void upsert(const Key& key, const Value& value) {
    SkipListNode* node = head_;
    const level_t localMaxLevel = randomLevel(globalMaxLevel_);

    SkipListNode* newNode = nullptr;
    for (level_t curLevel = localMaxLevel; curLevel >= 0; curLevel--) {
      while (node->next[curLevel] &&
             compareFunc_(node->next[curLevel]->key, key) < 0) {
        node = node->next[curLevel];
      }

      if (node->next[curLevel] &&
          compareFunc_(node->next[curLevel]->key, key) == 0) {
        std::swap(node->next[curLevel]->value, const_cast<Value&>(value));
        return;
      }

      if (newNode == nullptr) {
        newNode = new SkipListNode(key, value);
      }

      newNode->next[curLevel] = node->next[curLevel];
      node->next[curLevel] = newNode;
    }

    if (localMaxLevel > globalMaxLevel_) {
      globalMaxLevel_ = localMaxLevel;
    }
  }

  /**
   * @brief Reduce the cnt of the corresponding node according to the Key, and
   * delete it if it reaches zero
   *
   * @param[in] key
   */
  void erase(const Key& key) {
    SkipListNode* node = head_;
    SkipListNode* deleteNode = nullptr;

    for (level_t curLevel = MAXLEVEL; curLevel >= 0; curLevel--) {
      while (node->next[curLevel] &&
             compareFunc_(node->next[curLevel]->key, key) < 0) {
        node = node->next[curLevel];
      }

      if (node->next[curLevel] &&
          compareFunc_(node->next[curLevel]->key, key) == 0) {
        deleteNode = node->next[curLevel];
        node->next[curLevel] = deleteNode->next[curLevel];
      }
    }

    if (deleteNode) {
      delete deleteNode;
    }
  }

  void print() {
    for (level_t curLevel = globalMaxLevel_; curLevel >= 0; curLevel--) {
      std::cout << "[Level " << curLevel << "]: ";
      for (SkipListNode* p = head_; p->next[curLevel]; p = p->next[curLevel]) {
        std::cout << "{" << p->key << ", " << p->value << "} -> ";
      }
      std::cout << "(x)\n";
    }
  }

 private:
  /**
   * @brief Randomly generate a level
   *
   * @return level_t
   */
  static inline level_t randomLevel(level_t maxLevel) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 10);

    level_t level = 0;
    // Increase level with 30% probability
    while (level <= maxLevel && dis(gen) <= 2) {
      level++;
    }
    return level;
  }

 private:
  Compare compareFunc_;
  level_t globalMaxLevel_;
  SkipListNode* head_;
};