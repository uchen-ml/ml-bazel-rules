#ifndef __TOOLS_TOKENS_H

#define __TOOLS_TOKENS_H

#include <compare>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <map>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

namespace uchen::tools::tokens {

class Token {
 public:
  Token(std::string_view name, bool is_special)
      : name_(name), is_special_(is_special) {}

  std::string name() const { return name_; }
  bool is_special() const { return is_special_; }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Token& token) {
    absl::Format(&sink, "%s", token.name_);
  }

 private:
  std::string name_;
  bool is_special_;
};

std::strong_ordering operator<=>(const Token& token, std::string_view s);
std::strong_ordering operator<=>(const Token& t1, const Token& t2);
bool operator==(const Token& token, std::string_view s);

class TokenStore {
 public:
  TokenStore() = default;
  TokenStore(std::initializer_list<const std::string_view> initial);
  explicit TokenStore(std::span<const std::string> initial) {
    for (auto token : initial) {
      Add(token);
    }
  }
  std::vector<Token> Tokenize(const char* input) const {
    // Mostly used in tests, drops the terminating 0
    return Tokenize(std::span<const char>(input, std::strlen(input)));
  }
  std::vector<Token> Tokenize(std::span<const char> input) const;
  void Update(const std::map<std::string, size_t>& tokens) {
    for (const auto& token : tokens) {
      Add(token.first);
    }
  }

 private:
  bool isIdentifierChar(char c) const { return std::isalnum(c) || c == '_'; }
  void Add(absl::string_view sv);

  std::unordered_set<std::string> tokens_;
};

std::map<std::string, size_t> Combine(std::span<const Token> tokens,
                                      size_t min_matches = 1);

}  // namespace uchen::tools::tokens

namespace std {

inline std::ostream& operator<<(std::ostream& os,
                                const uchen::tools::tokens::Token& token) {
  os << absl::StrCat(token);
  return os;
}

}  // namespace std

#endif  // __TOOLS_TOKENS_H