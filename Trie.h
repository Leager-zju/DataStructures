#pragma once

#include <string>
#include <vector>

class PrefixTrie {
 public:
  PrefixTrie();
  ~PrefixTrie();

  void insert(const std::string& str);

  bool exist(const std::string& str);

  bool remove(const std::string& str);

  inline bool empty() { return root->isLeaf(); }

  std::vector<std::string> toVector();

 private:
  struct TrieNode {
    TrieNode() : endOfString_(false) {
      for (int i = 0; i < CAPITAL_CARACTER_END; i++) {
        children_[i] = nullptr;
      }
    }
    TrieNode(const std::string_view& str)
        : endOfString_(false), data_(std::move(str)) {
      for (int i = 0; i < CAPITAL_CARACTER_END; i++) {
        children_[i] = nullptr;
      }
    }

    inline bool isLeaf() {
      for (int i = 0; i < CAPITAL_CARACTER_END; i++) {
        if (children_[i]) return false;
      }
      return true;
    }

    const static int CAPITAL_CARACTER_BEGIN = 26;
    const static int CAPITAL_CARACTER_END = 52;

    bool endOfString_;
    std::string data_;
    TrieNode* children_[52];
  };

  inline static int getIndex(char ch) {
    return isupper(ch) ? (ch - 'A' + TrieNode::CAPITAL_CARACTER_BEGIN)
                       : ch - 'a';
  }

  void insertInternal(TrieNode*& node, const std::string_view& str);

  bool existInternal(TrieNode* const node, const std::string_view& str) const;

  bool removeInternal(TrieNode*& node, const std::string_view& str);

  void append(TrieNode* const node, std::vector<std::string>& vec) const;

  void recursivelyAppend(TrieNode* const node, std::string& str, std::vector<std::string>& vec) const;

  void destory(TrieNode* node);

 private:
  TrieNode* root;
};
