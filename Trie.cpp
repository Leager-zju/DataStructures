#include "Trie.h"

#include <string.h>

static size_t longestPrefixMatchLength(const char* a, const char* b) {
  size_t len = std::min(strlen(a), strlen(b));
  size_t res = 0;
  while (res < len && a[res] == b[res]) {
    res++;
  }
  return res;
}

PrefixTrie::PrefixTrie() : root(new TrieNode) {}

PrefixTrie::~PrefixTrie() { destory(root); }

void PrefixTrie::insert(const std::string& str) {
  if (str.empty()) return;

  int childIndex = getIndex(str.at(0));
  insertInternal(root->children_[childIndex], str);
  root->numOfChilren_++;
}

bool PrefixTrie::exist(const std::string& str) {
  if (str.empty()) return true;

  int childIndex = getIndex(str.at(0));
  return existInternal(root->children_[childIndex], str);
}

bool PrefixTrie::remove(const std::string& str) {
  if (str.empty()) return true;

  int childIndex = getIndex(str.at(0));
  if (removeInternal(root->children_[childIndex], str)) {
    root->numOfChilren_--;
    return true;
  }
  return false;
}

/*********************************************************************
 ****************************** Private ******************************
 *********************************************************************/

void PrefixTrie::insertInternal(TrieNode*& node, const std::string_view& str) {
  if (node == nullptr) {
    node = new TrieNode(str);
    node->isEndOfString_ = true;
    return;
  }

  size_t matchLength = longestPrefixMatchLength(node->data_.data(), str.data());
  if (matchLength < node->data_.length()) {
    TrieNode* cur = node;
    cur->data_.erase(0, matchLength);

    node = new TrieNode(str.substr(0, matchLength));
    node->children_[getIndex(cur->data_.at(0))] = cur;
    node->numOfChilren_ = 1;
  }

  if (matchLength == str.length()) {
    node->isEndOfString_ = true;
    return;
  }

  insertInternal(node->children_[getIndex(str.at(matchLength))],
                 str.substr(matchLength, str.length() - matchLength));
  node->numOfChilren_++;
}

bool PrefixTrie::existInternal(TrieNode* const node,
                               const std::string_view& str) const {
  if (node == nullptr) {
    return false;
  }

  size_t matchLength = longestPrefixMatchLength(node->data_.data(), str.data());
  if (matchLength < node->data_.length()) {
    return false;
  }
  if (matchLength == str.length()) {
    return node->isEndOfString_;
  }

  return node->isLeaf()
             ? false
             : existInternal(
                   node->children_[getIndex(str.at(matchLength))],
                   str.substr(matchLength, str.length() - matchLength));
}

bool PrefixTrie::removeInternal(TrieNode*& node, const std::string_view& str) {
  if (node == nullptr) {
    return false;
  }

  size_t matchLength = longestPrefixMatchLength(node->data_.data(), str.data());
  if (matchLength < node->data_.length()) {
    return false;
  }
  if (matchLength == str.length()) {
    if (!node->isEndOfString_) {
      return false;
    }
    node->isEndOfString_ = false;
    if (node->isLeaf()) {
      delete node;
      node = nullptr;
    }
    return true;
  }

  bool res =
      removeInternal(node->children_[getIndex(str.at(matchLength))],
                     str.substr(matchLength, str.length() - matchLength));
  if (!res) return false;
  node->numOfChilren_--;
  if (node->isLeaf() && !node->isEndOfString_) {
    delete node;
    node = nullptr;
  }
  return true;
}

void PrefixTrie::destory(TrieNode* node) {
  if (!node->isLeaf()) {
    for (int i = 0; i < TrieNode::CAPITAL_CARACTER_END; i++) {
      if (auto child = node->children_[i]; child != nullptr) {
        destory(child);
      }
    }
  }

  delete node;
}

/*********************************************************************
 ******************************* Debug *******************************
 *********************************************************************/

std::vector<std::string> PrefixTrie::toVector() {
  std::vector<std::string> res;
  append(root, res);
  return res;
}

void PrefixTrie::append(TrieNode* node, std::vector<std::string>& vec) const {
  std::string tmp;
  recursivelyAppend(node, tmp, vec);
}

void PrefixTrie::recursivelyAppend(TrieNode* const node, std::string& str,
                                   std::vector<std::string>& vec) const {
  size_t oldLength = str.length();
  str.append(node->data_);
  if (node->isEndOfString_) {
    vec.emplace_back(str);
  }
  if (!node->isLeaf()) {
    for (int i = 0; i < TrieNode::CAPITAL_CARACTER_END; i++) {
      if (node->children_[i]) {
        recursivelyAppend(node->children_[i], str, vec);
      }
    }
  }
  str = str.substr(0, oldLength);
}
