#pragma once

#include <algorithm>
#include <iostream>
#include <string>

template <class T>
struct comp {
  int operator()(const T &lhs, const T &rhs) const {
    if (lhs == rhs) return 0;
    if (lhs < rhs) return -1;
    return 1;
  }
};

template <class Key, class Value, class Compare = comp<Key>>
class RBTree {
  enum NodeColor { RED, BLACK, VOID };
  struct RBTreeNode {
    friend class RBTree;
    explicit RBTreeNode(const Key &k, const Value &v)
        : kv(k, v),
          color(RED),
          parent(nullptr),
          left(nullptr),
          right(nullptr) {}

    void setColor(NodeColor c) { color = c; }
    void setParent(RBTreeNode *p) { parent = p; }

    std::pair<Key, Value> kv;

   private:
    NodeColor color;
    RBTreeNode *parent;
    RBTreeNode *left;
    RBTreeNode *right;
  };

  class RBTreeIterator {
   public:
    RBTreeIterator() : object(nullptr) {}

    explicit RBTreeIterator(RBTreeNode *ptr) : object(ptr) {}
    RBTreeIterator(const RBTreeIterator &other) : object(other.object) {}
    RBTreeIterator(RBTreeIterator &&other) : object(other.object) {
      other.object = nullptr;
    }

    RBTreeIterator &operator=(const RBTreeIterator &other) {
      object = other.object;
      return *this;
    }
    RBTreeIterator &operator=(RBTreeIterator &&other) {
      object = other.object;
      other.object = nullptr;
      return *this;
    }
    RBTreeIterator &operator=(RBTreeNode *ptr) {
      object = ptr;
      return *this;
    }

    bool operator==(const RBTreeIterator &other) {
      return object == other.object;
    }
    bool operator==(RBTreeNode *ptr) { return object == ptr; }

    std::pair<Key, Value> *operator->() { return &(object->kv); }
    std::pair<Key, Value> &operator*() { return object->kv; }

    RBTreeIterator &operator++() {
      object = getSuccessor(object);
      return *this;
    }
    RBTreeIterator operator++(int) {
      RBTreeIterator tmp = *this;
      object = getSuccessor(object);
      return tmp;
    }

    RBTreeIterator &operator--() {
      object = getPredecessor(object);
      return *this;
    }
    RBTreeIterator operator--(int) {
      RBTreeIterator tmp = *this;
      object = getPredecessor(object);
      return tmp;
    }

   private:
    RBTreeNode *object;
  };

 public:
  using iterator = RBTreeIterator;

 public:
  RBTree() : root_(nullptr), size_(0), beginIter(nullptr), endIter(nullptr) {}
  ~RBTree() { destory(root_); }

  inline size_t size() { return size_; }

  inline bool empty() { return size_ == 0; }

  const RBTreeIterator &begin() { return beginIter; }
  const RBTreeIterator &end() { return endIter; }

  /**
   * @brief Insert {key, value} if key doesn't exist, update the value else
   *
   * @param[in] key
   * @param[in] value
   */
  void upsert(const Key &key, const Value &value) {
    upsertInternal(root_, key, value);
    root_->setColor(BLACK);
  }

  /**
   * @brief Get the value by specified key, return defaultValue if the key
   * doesn't exist
   *
   * @param[in] key
   * @param[in] defaultValue
   * @return const Value&
   */
  const Value &get(const Key &key, const Value &defaultValue) const {
    RBTreeNode *node = getInternal(root_, key);
    return node ? node->kv.second : defaultValue;
  }

  /**
   * @brief Get the color of tree node by specified key, return VOID if the key
   * doesn't exist
   *
   * @param[in] key
   * @return NodeColor
   */
  inline NodeColor getColor(const Key &key) const {
    RBTreeNode *node = getInternal(root_, key);
    return node ? getColorByNode(node) : VOID;
  }

  /**
   * @brief Remove {key, value} by specified key
   *
   * @param[in] key
   */
  void remove(const Key &key) {
    removeInternal(root_, key);
    if (size_ == 0) {
      root_ = nullptr;
    }
  }

  /* Only for debug */
  void print() {
    std::cout << "size: " << size_ << (validate() ? ", Valid\n" : ", Invalid\n")
              << '\n';
    printTreeStructure(root_, 0, '-');
  }
  /* Only for debug */
  bool validate() {
    return getColorByNode(root_) == BLACK ? validateInternal(root_) : false;
  }

 private:
  /**
   * @brief Get the color of tree node, the NIL node will be seemed as BLACK
   *
   * @param[in] node
   * @return NodeColor
   */
  inline NodeColor getColorByNode(RBTreeNode *node) const {
    return node ? node->color : BLACK;
  }

  /**
   * @brief Left rotate the node, and the parent's pointer will be modified
   * synchronously
   *
   * @param[in] node
   */
  static void leftRotate(RBTreeNode *&node) {
    RBTreeNode *child = node->right;
    child->setParent(node->parent);

    node->right = child->left;
    if (child->left) {
      child->left->setParent(node);
    }

    child->left = node;
    node->setParent(child);

    node = child;
  }

  /**
   * @brief Right rotate the node, and the parent's pointer will be modified
   * synchronously
   *
   * @param[in] node
   */
  static void rightRotate(RBTreeNode *&node) {
    RBTreeNode *child = node->left;
    child->setParent(node->parent);

    node->left = child->right;
    if (child->right) {
      child->right->setParent(node);
    }

    child->right = node;
    node->setParent(child);

    node = child;
  }

  /**
   * @brief Get the first node with a smaller key
   *
   * @param[in] node
   * @return RBTreeNode*
   */
  static RBTreeNode *getPredecessor(RBTreeNode *node) {
    if (node->left) {
      return rightMost(node->left);
    }
    while (node->parent && node != node->parent->right) {
      node = node->parent;
    }
    return node->parent;
  }

  /**
   * @brief Get the first node with a larger key
   *
   * @param[in] node
   * @return RBTreeNode*
   */
  static RBTreeNode *getSuccessor(RBTreeNode *node) {
    if (node->right) {
      return leftMost(node->right);
    }
    while (node->parent && node != node->parent->left) {
      node = node->parent;
    }
    return node->parent;
  }

  /**
   * @brief Find the leftmost node
   *
   * @param[in] node
   * @return RBTreeNode*&
   */
  static RBTreeNode *&leftMost(RBTreeNode *&node) {
    return node->left ? leftMost(node->left) : node;
  }

  /**
   * @brief Find the rightmost node
   *
   * @param[in] node
   * @return RBTreeNode*&
   */
  static RBTreeNode *&rightMost(RBTreeNode *&node) {
    return node->right ? rightMost(node->right) : node;
  }

  /**
   * @brief Internal function for upsert node
   *
   * @param[in] node
   * @param[in] key
   * @param[in] value
   */
  void upsertInternal(RBTreeNode *&node, const Key &key, const Value &value) {
    if (node == nullptr) {
      node = new RBTreeNode(key, value);
      if (++size_ == 1) {
        beginIter = node;
      }
      return;
    }

    int compareResult = compareFunc_(key, node->kv.first);
    if (compareResult == 0) {
      node->kv.second = value;
    } else if (compareResult < 0) {
      RBTreeNode *&child = node->left;
      RBTreeNode *&sibling = node->right;

      upsertInternal(child, key, value);
      child->setParent(node);

      if (beginIter == node) {
        --beginIter;
      }

      if (getColorByNode(child) == RED) {
        if (getColorByNode(sibling) == BLACK) {
          if (getColorByNode(child->left) == RED) {
            /*
             * CASE 1
             *   node -->    B
             *              / \
             *  child -->  R   B
             *            /
             *           R
             */
            child->setColor(BLACK);
            node->setColor(RED);
            rightRotate(node);
          } else if (getColorByNode(child->right) == RED) {
            /*
             * CASE 2
             *   node -->    B
             *              / \
             *  child -->  R   B
             *              \
             *               R
             */
            leftRotate(child);
            child->setColor(BLACK);
            node->setColor(RED);
            rightRotate(node);
          }
        } else {  // getColorByNode(sibling) == RED
          if (getColorByNode(child->left) == RED) {
            /*
             * CASE 3
             *   node -->    B
             *              / \
             *  child -->  R   R
             *            /
             *           R
             */
            child->setColor(BLACK);
            sibling->setColor(BLACK);
            node->setColor(RED);
          } else if (getColorByNode(child->right) == RED) {
            /*
             * CASE 4
             *   node -->    B
             *              / \
             *  child -->  R   R
             *              \
             *               R
             */
            leftRotate(child);
            child->setColor(BLACK);
            sibling->setColor(BLACK);
            node->setColor(RED);
          }
        }
      }
    } else {
      RBTreeNode *&child = node->right;
      RBTreeNode *&sibling = node->left;

      upsertInternal(child, key, value);
      child->setParent(node);

      if (getColorByNode(child) == RED) {
        if (getColorByNode(sibling) == BLACK) {
          if (getColorByNode(child->right) == RED) {
            /*
             * CASE 1
             *     B   <-- node
             *    / \
             *   B   R <-- child
             *        \
             *        R
             */
            child->setColor(BLACK);
            node->setColor(RED);
            leftRotate(node);
          } else if (getColorByNode(child->left) == RED) {
            /*
             * CASE 2
             *     B   <-- node
             *    / \
             *   B   R <-- child
             *      /
             *     R
             */
            rightRotate(child);
            child->setColor(BLACK);
            node->setColor(RED);
            leftRotate(node);
          }
        } else {  // getColorByNode(sibling) == RED
          if (getColorByNode(child->right) == RED) {
            /*
             * CASE 3
             *     B   <-- node
             *    / \
             *   R   R <-- child
             *        \
             *        R
             */
            child->setColor(BLACK);
            sibling->setColor(BLACK);
            node->setColor(RED);
          } else if (getColorByNode(child->left) == RED) {
            /*
             * CASE 4
             *     B   <-- node
             *    / \
             *   R   R <-- child
             *      /
             *     R
             */
            rightRotate(child);
            child->setColor(BLACK);
            sibling->setColor(BLACK);
            node->setColor(RED);
          }
        }
      }
    }
  }

  /**
   * @brief Internal function for get node
   *
   * @param[in] node
   * @param[in] key
   * @return RBTreeNode*
   */
  RBTreeNode *getInternal(RBTreeNode *node, const Key &key) const {
    if (node == nullptr) {
      return nullptr;
    }

    int compareResult = compareFunc_(key, node->kv.first);
    if (compareResult == 0) {
      return node;
    }
    if (compareResult < 0) {
      return getInternal(node->left, key);
    }
    return getInternal(node->right, key);
  }

  /**
   * @brief Swap two nodes' {key, value} and recursively remove.
   *
   * @param[in] node
   * @param[in] key
   * @return NodeColor means which color of nodes are removed in the subtree
   */
  NodeColor removeInternal(RBTreeNode *&node, const Key &key) {
    if (node == nullptr) {
      return VOID;
    }

    bool toLeft = true;
    int compareResult = compareFunc_(key, node->kv.first);
    if (compareResult == 0) {
      RBTreeNode *&swapNode =
          node->left ? rightMost(node->left)
                     : (node->right ? leftMost(node->right) : node);
      if (node->left == nullptr && node->right == nullptr) {
        // now the node must be leaf node
        NodeColor color = node->color;
        if (beginIter == node) {
          ++beginIter;
        }
        delete node;
        node = nullptr;
        --size_;
        return color;
      }

      std::swap(node->kv.first, swapNode->kv.first);
      std::swap(node->kv.second, swapNode->kv.second);
      if (node->left == nullptr) {
        toLeft = false;
      }
    } else if (compareResult > 0) {
      toLeft = false;
    }

    if (toLeft) {
      if (removeInternal(node->left, key) == BLACK) {
        RBTreeNode *&sibling = node->right;
        if (getColorByNode(node) == BLACK) {
          if (getColorByNode(sibling) == BLACK) {
            /*
             * CASE 1
             *     B   <-- node
             *    / \
             *  (x)  B <-- sibling
             */
            sibling->setColor(RED);
            return BLACK;
          } else {
            /*
             * CASE 2
             *     B   <-- node
             *    / \
             *  (x)  R <-- sibling
             *      / \
             *     B   B
             */
            sibling->setColor(BLACK);
            sibling->left->setColor(RED);
            leftRotate(node);
          }
        } else {
          if (getColorByNode(sibling->left) == BLACK &&
              getColorByNode(sibling->right) == BLACK) {
            /*
             * CASE 3
             *     R   <-- node
             *    / \
             *  (x)  B <-- sibling
             */
            node->setColor(BLACK);
            sibling->setColor(RED);
          }
        }

        if (getColorByNode(sibling->left) == RED) {
          /*
           * CASE 4
           *     ?   <-- node
           *    / \
           *  (x)  B <-- sibling
           *      /
           *     R
           */
          sibling->setColor(RED);
          sibling->left->setColor(BLACK);
          rightRotate(sibling);
        }
        if (getColorByNode(sibling->right) == RED) {
          /*
           * CASE 5
           *     ?   <-- node
           *    / \
           *  (x)  B <-- sibling
           *        \
           *         R
           */
          sibling->setColor(node->color);
          node->setColor(BLACK);
          sibling->right->setColor(BLACK);
          leftRotate(node);
        }
      }
    } else {
      if (removeInternal(node->right, key) == BLACK) {
        RBTreeNode *&sibling = node->left;
        if (getColorByNode(node) == BLACK) {
          if (getColorByNode(sibling) == BLACK) {
            /*
             * CASE 1
             *     node -->   B
             *               / \
             *  sibling --> B  (x)
             */
            sibling->setColor(RED);
            return BLACK;
          } else {
            /*
             * CASE 2
             *     node -->   B
             *               / \
             *  sibling --> R  (x)
             *             / \
             *            B   B
             */
            sibling->setColor(BLACK);
            sibling->right->setColor(RED);
            rightRotate(node);
          }
        } else {
          if (getColorByNode(sibling->left) == BLACK &&
              getColorByNode(sibling->right) == BLACK) {
            /*
             * CASE 3
             *     node -->   R
             *               / \
             *  sibling --> B  (x)
             */
            node->setColor(BLACK);
            sibling->setColor(RED);
          }
        }

        if (getColorByNode(sibling->right) == RED) {
          /*
           * CASE 4
           *     node -->   ?
           *               / \
           *  sibling --> B  (x)
           *               \
           *                R
           */
          sibling->setColor(RED);
          sibling->right->setColor(BLACK);
          leftRotate(sibling);
        }
        if (getColorByNode(sibling->left) == RED) {
          /*
           * CASE 5
           *     node -->   ?
           *               / \
           *  sibling --> B  (x)
           *             /
           *            R
           */
          sibling->setColor(node->color);
          node->setColor(BLACK);
          sibling->left->setColor(BLACK);
          rightRotate(node);
        }
      }
    }

    return VOID;
  }

  /**
   * @brief Only called by destructor
   *
   * @param[in] node
   */
  void destory(RBTreeNode *node) {
    if (node == nullptr) {
      return;
    }
    if (node->left) {
      destory(node->left);
    }
    if (node->right) {
      destory(node->right);
    }
    delete node;
  }

  /* Only for debug */
  inline const std::string &colorToString(NodeColor color) {
    static const std::string RED = "\033[1;31m";
    static const std::string BLACK = "\033[1;30m";
    return color == 0 ? RED : BLACK;
  }
  /* Only for debug */
  inline const std::string &endOfColor() {
    static const std::string str = "\033[0m";
    return str;
  }
  /* Only for debug -- print the structure by inorder */
  void printTreeStructure(RBTreeNode *node, int depth, char prefix) {
    if (node == nullptr) {
      return;
    }

    printTreeStructure(node->right, depth + 1, '/');
    std::cout << std::string(depth * 4, ' ') << prefix << "--"
              << colorToString(node->color) << '(' << node->kv.first << ", "
              << node->kv.second << ')' << endOfColor() << std::endl;
    printTreeStructure(node->left, depth + 1, '\\');
  }
  /* Only for debug -- return the number of black nodes to the nil node, -1 for
   * invalid */
  int validateInternal(RBTreeNode *node) {
    if (node == nullptr) {
      return 1;
    }

    NodeColor color = getColorByNode(node);
    if (color == RED && (getColorByNode(node->left) == RED ||
                         getColorByNode(node->right) == RED)) {
      return -1;
    }
    int left = validateInternal(node->left);
    int right = validateInternal(node->right);
    if (left == -1 || right == -1 || left != right) {
      return -1;
    }
    return color == BLACK ? left + 1 : left;
  }

 private:
  Compare compareFunc_;
  RBTreeNode *root_;

  size_t size_;

  RBTreeIterator beginIter;
  RBTreeIterator endIter;
};