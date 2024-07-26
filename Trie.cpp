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
}

bool PrefixTrie::exist(const std::string& str) {
  if (str.empty()) return true;

  int childIndex = getIndex(str.at(0));
  return existInternal(root->children_[childIndex], str);
}

bool PrefixTrie::remove(const std::string& str) {
  if (str.empty()) return true;

  int childIndex = getIndex(str.at(0));
  return removeInternal(root->children_[childIndex], str);
}

std::vector<std::string> PrefixTrie::toVector() {
  std::vector<std::string> res;
  append(root, res);
  return res;
}

/*********************************************************************
 ****************************** Private ******************************
 *********************************************************************/
void PrefixTrie::insertInternal(TrieNode*& node, const std::string_view& str) {
  if (node == nullptr) {
    node = new TrieNode(str);
    node->endOfString_ = true;
    return;
  }

  size_t matchLength = longestPrefixMatchLength(node->data_.data(), str.data());
  if (matchLength == node->data_.length()) {
    if (matchLength < str.length()) {
      insertInternal(node->children_[getIndex(str.at(matchLength))],
                     str.substr(matchLength, str.length() - matchLength));
    } else {
      node->endOfString_ = true;
    }
    return;
  }

  TrieNode* cur = node;
  cur->data_.erase(0, matchLength);

  node = new TrieNode(str.substr(0, matchLength));
  node->children_[getIndex(cur->data_.at(0))] = cur;
  insertInternal(node->children_[getIndex(str.at(matchLength))],
                 str.substr(matchLength, str.length() - matchLength));
}

bool PrefixTrie::existInternal(TrieNode* const node,
                               const std::string_view& str) const {
  if (node == nullptr) {
    return false;
  }

  size_t matchLength = longestPrefixMatchLength(node->data_.data(), str.data());
  if (matchLength != node->data_.length()) {
    return false;
  }
  if (matchLength == str.length()) {
    return node->endOfString_;
  }

  return existInternal(node->children_[getIndex(str.at(matchLength))],
                       str.substr(matchLength, str.length() - matchLength));
}

bool PrefixTrie::removeInternal(TrieNode*& node, const std::string_view& str) {
  if (node == nullptr) {
    return false;
  }

  size_t matchLength = longestPrefixMatchLength(node->data_.data(), str.data());
  if (matchLength != node->data_.length()) {
    return false;
  }
  if (matchLength == str.length()) {
    if (!node->endOfString_) {
      return false;
    }
    node->endOfString_ = false;
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

  if (node->isLeaf() && !node->endOfString_) {
    delete node;
    node = nullptr;
  }
  return true;
}

void PrefixTrie::append(TrieNode* node, std::vector<std::string>& vec) const {
  std::string tmp;
  recursivelyAppend(node, tmp, vec);
}

void PrefixTrie::recursivelyAppend(TrieNode* const node, std::string& str, std::vector<std::string>& vec) const {
  size_t oldLength = str.length();
  str.append(node->data_);
  if (node->endOfString_) {
    vec.emplace_back(str);
  }
  for (int i = 0; i < TrieNode::CAPITAL_CARACTER_END; i++) {
    if (node->children_[i]) {
      recursivelyAppend(node->children_[i], str, vec);
    }
  }
  str = str.substr(0, oldLength);
}

void PrefixTrie::destory(TrieNode* node) {
  for (int i = 0; i < TrieNode::CAPITAL_CARACTER_END; i++) {
    if (auto child = node->children_[i]; child != nullptr) {
      destory(child);
    }
  }

  delete node;
}