#pragma once

#include <string>
#include <vector>

class PrefixTrie {
  struct TrieNode {
    TrieNode() : isEndOfString_(false), numOfChilren_(0) {
      for (int i = 0; i < CAPITAL_CARACTER_END; i++) {
        children_[i] = nullptr;
      }
    }
    TrieNode(const std::string_view& str)
        : data_(std::move(str)), isEndOfString_(false), numOfChilren_(0) {
      for (int i = 0; i < CAPITAL_CARACTER_END; i++) {
        children_[i] = nullptr;
      }
    }

    inline bool isLeaf() { return numOfChilren_ == 0; }

    const static int CAPITAL_CARACTER_BEGIN = 26;
    const static int CAPITAL_CARACTER_END = 52;

    std::string data_;
    bool isEndOfString_;
    int numOfChilren_;
    TrieNode* children_[52];
  };

 public:
  PrefixTrie();
  ~PrefixTrie();

  /**
   * @brief Return true if the Trie is empty, false else
   *
   * @return true
   * @return false
   */
  inline bool empty() { return root->isLeaf(); }

  /**
   * @brief Insert a string into Trie
   *
   * @param[in] str
   */
  void insert(const std::string& str);

  /**
   * @brief Return true if the string exists in the Trie, false else
   *
   * @param[in] str
   * @return true
   * @return false
   */
  bool exist(const std::string& str);

  /**
   * @brief Return true if removing success, false else
   *
   * @param[in] str
   * @return true
   * @return false
   */
  bool remove(const std::string& str);

  /**
   * @brief Transform the Trie into a string vector
   *
   * @return std::vector<std::string>
   */
  std::vector<std::string> toVector();

 private:
  /**
   * @brief Get the index corresponding to the character
   *
   * @param[in] ch
   * @return int
   */
  inline static int getIndex(char ch) {
    return isupper(ch) ? (ch - 'A' + TrieNode::CAPITAL_CARACTER_BEGIN)
                       : ch - 'a';
  }

  /**
   * @brief Recursively insert a string
   *
   * @param[in] node
   * @param[in] str
   */
  void insertInternal(TrieNode*& node, const std::string_view& str);

  /**
   * @brief Recursively check if a string exists
   *
   * @param[in] node
   * @param[in] str
   * @return true
   * @return false
   */
  bool existInternal(TrieNode* const node, const std::string_view& str) const;

  /**
   * @brief
   *
   * @param[in] node
   * @param[in] str
   * @return true means that the child has been deleted, so the numOfChilren
   * should decrease
   * @return false else
   */
  bool removeInternal(TrieNode*& node, const std::string_view& str);

  /**
   * @brief Append all substrings to the result vector
   *
   * @param[in] node
   * @param[in] vec
   */
  void append(TrieNode* const node, std::vector<std::string>& vec) const;

  /**
   * @brief Recursively append all substrings to the result vector
   *
   * @param[in] node
   * @param[in] str
   * @param[in] vec
   */
  void recursivelyAppend(TrieNode* const node, std::string& str,
                         std::vector<std::string>& vec) const;

  /**
   * @brief Only called by destructor
   *
   * @param[in] node
   */
  void destory(TrieNode* node);

 private:
  TrieNode* root;
};
