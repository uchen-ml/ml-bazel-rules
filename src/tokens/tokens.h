#ifndef __TOOLS_TOKENS_H

#define __TOOLS_TOKENS_H

#include <compare>
#include <initializer_list>
#include <map>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "absl/log/check.h"
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
bool operator==(const Token& token, std::string_view s);

class TokenStore {
 public:
  TokenStore() = default;
  TokenStore(std::initializer_list<const std::string_view> initial);
  std::vector<Token> Tokenize(std::string_view input) const;
  void Update(const auto& tokens) {
    for (std::string_view token : tokens) {
      Add(token);
    }
  }

 private:
  bool isIdentifierChar(char c) const { return std::isalnum(c) || c == '_'; }
  void Add(absl::string_view sv);

  std::unordered_set<std::string> tokens_;
};

}  // namespace uchen::tools::tokens

namespace std {

inline std::ostream& operator<<(std::ostream& os,
                                const uchen::tools::tokens::Token& token) {
  os << absl::StrCat(token);
  return os;
}

}  // namespace std

#endif  // __TOOLS_TOKENS_H