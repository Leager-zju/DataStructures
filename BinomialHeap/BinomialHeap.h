#pragma once

#include <iostream>

template <class T>
struct comp {
  int operator()(const T& lhs, const T& rhs) const {
    if (lhs == rhs) return 0;
    if (lhs < rhs) return -1;
    return 1;
  }
};

template <class Value, class Compare = comp<Value>>
class BinomialHeap {
  struct BinomialHeapNode {
    BinomialHeapNode()
        : degree_(0), parent_(nullptr), child_(nullptr), sibling_(nullptr) {}
    explicit BinomialHeapNode(const Value& value)
        : value_(value),
          degree_(0),
          parent_(nullptr),
          child_(nullptr),
          sibling_(nullptr) {}
    void setParent(BinomialHeapNode* newParent) {
      if (newParent == parent_) {
        return;
      }

      if (parent_) {
        --parent_->degree_;
      }
      parent_ = newParent;
      if (newParent) {
        ++newParent->degree_;
      }
    }
    void setSibling(BinomialHeapNode* newSibling) { sibling_ = newSibling; }

    Value value_;
    unsigned int degree_;
    BinomialHeapNode* parent_;
    BinomialHeapNode* child_;
    BinomialHeapNode* sibling_;
  };

 public:
  BinomialHeap() : head_(new BinomialHeapNode) {}
  explicit BinomialHeap(BinomialHeapNode* otherHead) : head_(otherHead) {}
  ~BinomialHeap() { destory(head_); }

  /**
   * @brief Return the size of the Binomial Heap
   *
   * @return size_t
   */
  inline size_t size() const {
    size_t res = 0;
    for (auto subHeap = head_->child_; subHeap; toSibling(subHeap)) {
      res += 1 << subHeap->degree_;
    }
    return res;
  }

  /**
   * @brief Return if the Binomial Heap is empty
   *
   * @return true
   * @return false
   */
  inline bool empty() const { return head_->child_ == nullptr; }

  /**
   * @brief Get the minimum element
   *
   * @return const Value&
   */
  inline const Value& front() const {
    if (empty()) {
      throw "Incorrect access to empty heap.";
    }
    return getMinmChild(head_)->value_;
  }

  /**
   * @brief Push a new value into the heap, with potential heap refactoring
   *
   * @param[in] value
   */
  void push(const Value& value) {
    BinomialHeapNode* newNode = new BinomialHeapNode(value);
    newNode->setParent(head_);
    newNode->setSibling(head_->child_);
    head_->child_ = mergeChildren(newNode);
  }

  /**
   * @brief Pop the minimum element, with potential heap refactoring
   *
   */
  void pop() {
    BinomialHeapNode* p = head_->child_;
    BinomialHeapNode* prev = p;

    BinomialHeapNode* minmNode = p;
    BinomialHeapNode* minmPrev = prev;

    toSibling(p);
    while (p) {
      if (compareFunc_(p->value_, minmNode->value_) < 0) {
        minmNode = p;
        minmPrev = prev;
      }
      toSibling(prev);
      toSibling(p);
    }

    if (minmNode == head_->child_) {
      toSibling(head_->child_);
    } else {
      minmPrev->setSibling(minmNode->sibling_);
    }
    minmNode->setParent(nullptr);
    minmNode->setSibling(nullptr);
    mergeHead(minmNode);
  }

  /**
   * @brief Update a node with oldValue to newValue
   *
   * @param[in] oldValue
   * @param[in] newValue
   * @return true
   * @return false
   */
  bool update(const Value& oldValue, const Value& newValue) {
    BinomialHeapNode* node = nullptr;
    for (auto root = head_->child_; root; toSibling(root)) {
      if (node = findNode(root, oldValue); node) {
        break;
      }
    }
    if (node == nullptr) {
      return false;
    }

    int compareResult = compareFunc_(oldValue, newValue);
    if (compareResult == 0) {
      return true;
    }

    node->value_ = newValue;
    if (compareResult < 0) {
      // Bottom-up
      while (node->parent_ &&
             compareFunc_(node->value_, node->parent_->value_) < 0) {
        std::swap(node->value_, node->parent_->value_);
        node = node->parent_;
      }
    } else {
      // Top-down
      while (node->child_) {
        BinomialHeapNode* child = getMinmChild(node);
        if (compareFunc_(node->value_, child->value_) <= 0) {
          break;
        }
        std::swap(node->value_, child->value_);
        node = child;
      }
    }
    return true;
  }

  /**
   * @brief Merge with another Binomial Heap and return the merged result
   *
   * @param[in] other
   * @return BinomialHeap&
   */
  BinomialHeap& merge(BinomialHeap&& other) {
    if (this->empty()) {
      std::swap(head_, other.head_);
      return *this;
    }
    if (other.empty()) {
      return *this;
    }

    BinomialHeapNode* thisCur = head_->child_;
    BinomialHeapNode* otherCur = other.head_->child_;
    BinomialHeapNode* tail = head_;

    if (thisCur->degree_ <= otherCur->degree_) {
      tail->child_ = thisCur;
      toSibling(thisCur);
    } else {
      tail->child_ = otherCur;
      toSibling(otherCur);
    }
    tail = tail->child_;

    while (thisCur && otherCur) {
      if (thisCur->degree_ <= otherCur->degree_) {
        tail->setSibling(thisCur);
        toSibling(thisCur);
      } else {
        tail->setSibling(otherCur);
        otherCur->setParent(head_);
        toSibling(otherCur);
      }
      toSibling(tail);
    }
    if (thisCur) {
      tail->setSibling(thisCur);
    }
    if (otherCur) {
      tail->setSibling(otherCur);
    }

    head_->child_ = mergeChildren(head_->child_);
    other.head_->child_ = nullptr;
    return *this;
  }

  /* Only for debug */
  void print() const {
    BinomialHeapNode* node = head_->child_;
    size_t heapIdx = 1;
    while (node) {
      std::cout << "heap#" << heapIdx++ << ": size " << (1 << node->degree_)
                << ", min value " << node->value_ << '\n';
      toSibling(node);
    }
  }

 private:
  /**
   * @brief Move a node to its sibling
   *
   * @param[in] node
   */
  inline void toSibling(BinomialHeapNode*& node) const {
    node = node->sibling_;
  }

  /**
   * @brief Return the minimum child in one level
   *
   * @param[in] head
   * @return BinomialHeapNode*
   */
  BinomialHeapNode* getMinmChild(BinomialHeapNode* head) const {
    BinomialHeapNode* p = head->child_;
    BinomialHeapNode* minmNode = p;

    toSibling(p);
    while (p) {
      if (compareFunc_(p->value_, minmNode->value_) < 0) {
        minmNode = p;
      }
      toSibling(p);
    }
    return minmNode;
  }

  /**
   * @brief Finds a node with a specified value in the tree, and return
   *
   * @param[in] value
   * @return BinomialHeapNode*
   */
  BinomialHeapNode* findNode(BinomialHeapNode* head, const Value& value) const {
    if (head == nullptr) {
      return nullptr;
    }
    if (compareFunc_(head->value_, value) == 0) {
      return head;
    }
    for (auto child = head->child_; child; toSibling(child)) {
      if (compareFunc_(child->value_, value) < 0) {
        if (auto res = findNode(child, value); res) {
          return res;
        }
      }
    }
    return nullptr;
  }

  /**
   * @brief Merge the given node
   *
   * @param[in] head
   * @return BinomialHeap&
   */
  BinomialHeap& mergeHead(BinomialHeapNode* head) {
    return merge(BinomialHeap(head));
  }

  /**
   * @brief Reset the parent-child relationship of the given nodes
   *
   * @param[in] parent
   * @param[in] child
   */
  inline void mergeNode(BinomialHeapNode* parent, BinomialHeapNode* child) {
    child->setSibling(nullptr);

    if (parent->child_ == nullptr) {
      parent->child_ = child;
    } else {
      BinomialHeapNode* prev = parent->child_;
      while (prev->sibling_) {
        toSibling(prev);
      }
      prev->setSibling(child);
    }
    child->setParent(parent);
  }

  /**
   * @brief Merge all children by degree
   *
   * @param[in] node
   * @return BinomialHeapNode*
   */
  BinomialHeapNode* mergeChildren(BinomialHeapNode* node) {
    BinomialHeapNode* first = node;
    BinomialHeapNode* second = first->sibling_;
    if (second == nullptr) {
      return first;
    }

    if (first->degree_ == second->degree_) {
      if (compareFunc_(first->value_, second->value_) <= 0) {
        first->setSibling(second->sibling_);
        mergeNode(first, second);
      } else {
        mergeNode(second, first);
        first = second;
      }
    }

    if (first->sibling_) {
      first->sibling_ = mergeChildren(first->sibling_);
    }
    return first;
  }

  /**
   * @brief Only called by destructor
   *
   * @param[in] head
   */
  void destory(BinomialHeapNode* head) {
    BinomialHeapNode* ptr = head->child_;
    while (ptr) {
      BinomialHeapNode* tmp = ptr;
      toSibling(ptr);
      destory(tmp);
    }
    delete head;
  }

 private:
  Compare compareFunc_;
  BinomialHeapNode* head_;
};