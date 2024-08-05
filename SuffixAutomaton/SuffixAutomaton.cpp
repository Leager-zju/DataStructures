#include "SuffixAutomaton.h"

SuffixAutomaton::SuffixAutomaton() : strLength_(0), last_(0) {
  automaton.emplace_back(0, NIL);
}

SuffixAutomaton::SuffixAutomaton(const std::string& src) : SuffixAutomaton() {
  insert(src);
}

void SuffixAutomaton::insert(char ch) {
  automaton.emplace_back(++strLength_, 0);
  State& newState = automaton.back();
  stateIndex newStateIdx = automaton.size() - 1;

  stateIndex p;
  for (p = last_; p != NIL && !automaton[p].next.count(ch);
       p = automaton[p].parent) {
    automaton[p].next[ch] = newStateIdx;
  }

  if (p != NIL) {  //  {p} + c is a suffix of {q}
    stateIndex q = automaton[p].next[ch];
    if (automaton[p].length + 1 == automaton[q].length) {
      // endpoint({p} + c) change in sync with endpoint({q})
      newState.parent = q;
    } else {
      // size(endpoint({p} + c)) > size(endpoint({q})), so we should
      // create a new intermidiate state "cloneState" to present {p} + c
      automaton.emplace_back(automaton[q]);
      State& cloneState = automaton.back();
      stateIndex cloneStateIdx = automaton.size() - 1;
      cloneState.length = automaton[p].length + 1;
      cloneState.firstTime =
          automaton[q].firstTime + automaton[q].length - cloneState.length;

      for (; p != NIL; p = automaton[p].parent) {
        auto iter = automaton[p].next.find(ch);
        if (iter == automaton[p].next.end() || iter->second != q) {
          break;
        }
        iter->second = cloneStateIdx;
      }

      automaton[q].parent = cloneStateIdx;
      newState.parent = cloneStateIdx;
    }
  }

  for (p = automaton[newStateIdx].parent; p != NIL; p = automaton[p].parent) {
    automaton[p].cnt++;
  }

  last_ = newStateIdx;
}

void SuffixAutomaton::insert(const std::string& str) {
  for (char ch : str) {
    insert(ch);
  }
}

bool SuffixAutomaton::match(const std::string& pattern) {
  return getStateIndex(pattern) != NIL;
}

uint32_t SuffixAutomaton::differentSubstrings() {
  // Just sum the size of the string set corresponding to all states
  uint32_t res = 0;
  for (const State& state : automaton) {
    const State& parentState = automaton[state.parent];
    if (state.parent != NIL) {
      res += state.length - parentState.length;
    }
  }
  return res;
}

uint32_t SuffixAutomaton::occurrences(const std::string& pattern) {
  stateIndex index = getStateIndex(pattern);
  return index == NIL ? 0 : automaton[index].cnt;
}

uint32_t SuffixAutomaton::find(const std::string& pattern) {
  stateIndex index = getStateIndex(pattern);
  return index == NIL ? npos
                      : automaton[index].firstTime + automaton[index].length -
                            pattern.length();
}

std::string SuffixAutomaton::logestCommonSubstring(const std::string& pattern) {
  stateIndex cur = ROOT;
  size_t len = 0;

  // Implicitly maintain a temporary longest common substring by the following
  // two variables —— pattern.substr(maxLengthEndpos - maxLen + 1, maxLen)
  size_t maxLengthEndpos = 0;
  size_t maxLen = 0;

  for (size_t i = 0; i < pattern.size(); i++) {
    char ch = pattern[i];

    auto iter = automaton[cur].next.find(ch);
    while (cur != ROOT && iter == automaton[cur].next.end()) {
      // This means that the current common substring can't to be the
      // suffix of any substring of src after adding the new character ch.
      // So the common substring needs be modified by compressing the suffix,
      // that is, going to the parent state
      cur = automaton[cur].parent;
      len = std::min(len, automaton[cur].length);
      iter = automaton[cur].next.find(ch);
    }

    if (iter == automaton[cur].next.end()) {
      // This means that the character ch has not appeared in src.
      // At this time, the current common substring has been cleared,
      // and the construction starts again.
      continue;
    }

    // By compressing the suffix, we found a state where the common substring
    // can become its suffix, so go for it!
    cur = iter->second;
    len++;
    if (len > maxLen) {
      maxLen = len;
      maxLengthEndpos = i;
    }
  }
  return pattern.substr(maxLengthEndpos - maxLen + 1, maxLen);
}

SuffixAutomaton::stateIndex SuffixAutomaton::getStateIndex(
    const std::string& pattern) {
  stateIndex index = ROOT;
  for (char ch : pattern) {
    auto next = automaton[index].next.find(ch);
    if (next == automaton[index].next.end()) {
      return NIL;
    }
    index = next->second;
  }
  return index;
}