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
 public:
  RBTree() : root_(nullptr), size_(0) {}
  ~RBTree() { destory(root_); }

  enum NodeColor { RED, BLACK, VOID };

  inline size_t size() { return size_; }

  inline bool empty() { return size_ == 0; }

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
    return node ? node->value : defaultValue;
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
    std::cout << "size: " << size_ << '\n';
    printTreeStructure(root_, 0, '-');
  }
  /* Only for debug */
  bool validate() {
    return getColorByNode(root_) == BLACK ? validateInternal(root_) : false;
  }

 private:
  struct RBTreeNode {
    explicit RBTreeNode(const Key &k, const Value &v)
        : color(RED),
          key(k),
          value(v),
          parent(nullptr),
          left(nullptr),
          right(nullptr) {}

    void setColor(NodeColor c) { color = c; }
    void setParent(RBTreeNode *p) { parent = p; }

    NodeColor color;
    Key key;
    Value value;
    RBTreeNode *parent;
    RBTreeNode *left;
    RBTreeNode *right;
  };

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
  void leftRotate(RBTreeNode *&node) {
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
  void rightRotate(RBTreeNode *&node) {
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
   * @brief Internal function for upsert node
   *
   * @param[in] node
   * @param[in] key
   * @param[in] value
   */
  void upsertInternal(RBTreeNode *&node, const Key &key, const Value &value) {
    if (node == nullptr) {
      node = new RBTreeNode(key, value);
      size_++;
      return;
    }

    int compareResult = compareFunc_(key, node->key);
    if (compareResult == 0) {
      node->value = value;
    } else if (compareResult < 0) {
      RBTreeNode *&child = node->left;
      RBTreeNode *&sibling = node->right;

      upsertInternal(child, key, value);
      child->setParent(node);

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

    int compareResult = compareFunc_(key, node->key);
    if (compareResult == 0) {
      return node;
    }
    if (compareResult < 0) {
      return getInternal(node->left, key);
    }
    return getInternal(node->right, key);
  }

  /**
   * @brief Find the leftmost node
   *
   * @param[in] node
   * @return RBTreeNode*&
   */
  RBTreeNode *&leftMost(RBTreeNode *&node) {
    return node->left ? leftMost(node->left) : node;
  }

  /**
   * @brief Find the rightmost node
   *
   * @param[in] node
   * @return RBTreeNode*&
   */
  RBTreeNode *&rightMost(RBTreeNode *&node) {
    return node->right ? rightMost(node->right) : node;
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
    int compareResult = compareFunc_(key, node->key);
    if (compareResult == 0) {
      RBTreeNode *&swapNode =
          node->left ? rightMost(node->left)
                     : (node->right ? leftMost(node->right) : node);
      if (node->left == nullptr && node->right == nullptr) {
        // now the node must be leaf node
        NodeColor color = node->color;
        delete node;
        node = nullptr;
        --size_;
        return color;
      }

      std::swap(node->key, swapNode->key);
      std::swap(node->value, swapNode->value);
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
              << colorToString(node->color) << '(' << node->key << ", "
              << node->value << ')' << endOfColor() << std::endl;
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
};