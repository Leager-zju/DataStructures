#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

/**
 *
 * A suffix automaton satisfies the following properties
 *
 * 1. If a suffix automaton is represented by a tree structure, then each state
 * corresponds to a tree node.
 *
 * 2. Each state represents a set of strings with the same set of endpos.
 *
 * 3. If sorted by descending length, each string is a (proper) suffix of the
 * previous string, and the length decreases by 1.
 *
 * 4. For a state, all strings represented by all its ancestors in the tree
 * structure are (true) suffixes of the shortest string in the state.
 *
 * 5. If we can move from a state A to the next state B through a character `ch`
 * in `next`, then we can assume that all strings represented by A are suffixes
 * of all strings in B after adding the character ch.
 *
 */
class SuffixAutomaton {
  using stateIndex = int32_t;
  struct State {
    State(size_t len, stateIndex p)
        : length(len), cnt(1), firstTime(0), parent(p) {}
    size_t length;
    size_t cnt;
    size_t firstTime;
    stateIndex parent;
    std::unordered_map<char, int> next;
  };

 public:
  SuffixAutomaton();
  explicit SuffixAutomaton(const std::string& src);
  ~SuffixAutomaton() = default;

  /**
   * @brief Insert a character into automaton
   *
   * @param[in] ch
   */
  void insert(char ch);

  /**
   * @brief Insert a string into automaton
   *
   * @param[in] str
   */
  void insert(const std::string& str);

  /**
   * @brief Judge if pattern has appeared
   *
   * @param[in] pattern
   * @return true
   * @return false
   */
  bool match(const std::string& pattern);

  /**
   * @brief Get the number of all different substrings
   *
   * @return uint32_t
   */
  uint32_t differentSubstrings();

  /**
   * @brief Get the number of occurrences of pattern
   *
   * @param[in] str
   * @return uint32_t
   */
  uint32_t occurrences(const std::string& pattern);

  /**
   * @brief Get the index of the first occurrence of pattern (just like
   * `std::string::find`)
   *
   * @param[in] pattern
   * @return uint32_t
   */
  uint32_t find(const std::string& pattern);

  /**
   * @brief Get the longest common substring src and pattern
   *
   * @param[in] pattern
   * @return std::string
   */
  std::string logestCommonSubstring(const std::string& pattern);

 public:
  static const uint32_t npos = -1;

 private:
  /**
   * @brief Returns the indices of states that can be suffixed with pattern
   *
   * @param[in] pattern
   * @return stateIndex
   */
  stateIndex getStateIndex(const std::string& pattern);

 private:
  static constexpr stateIndex ROOT = 0;
  static constexpr stateIndex NIL = -1;

  size_t strLength_;
  stateIndex last_;
  std::vector<State> automaton;
};