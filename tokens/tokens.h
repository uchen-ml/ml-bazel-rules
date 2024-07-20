#ifndef __TOOLS_TOKENS_H

#define __TOOLS_TOKENS_H

#include <compare>
#include <ostream>
#include <string_view>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

namespace uchen::tools::tokens {

class TokenStore;

class Token {
 public:
  explicit Token(const TokenStore* store, int id) : store_(store), id_(id) {}

  std::string string() const;

  std::strong_ordering operator<=>(const Token& token) const {
    return id_ <=> token.id_;
  }

  std::strong_ordering operator<=>(const char* sv) const;
  bool operator==(const auto& other) const { return (*this <=> other) == 0; }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Token& token) {
    absl::Format(&sink, "%s", token.string());
  }

 private:
  const TokenStore* store_;
  int id_;
};

class TokenStore {
 public:
  struct SpecialTokens {
    Token unknown;
    Token input_start;
    Token input_end;
    Token upper_case;
    Token identifier_start;
    Token identifier_end;
  };

  TokenStore();
  Token FromString(std::string_view string) const;
  const SpecialTokens& special_tokens() const { return special_tokens_; }
  std::string token_name(int code) const {
    DCHECK_LE(code, token_names_.size());
    return token_names_[code - 1];
  }
  Token GetToken(char c) const;

 private:
  int first_char_;
  SpecialTokens special_tokens_ = {.unknown = Token(this, 1),
                                   .input_start = Token(this, 2),
                                   .input_end = Token(this, 3),
                                   .upper_case = Token(this, 4),
                                   .identifier_start = Token(this, 5),
                                   .identifier_end = Token(this, 6)};
  std::vector<std::string> token_names_ = {"<unknown>", "<si>", "<ei>",
                                           "<uc>",      "<sw>", "<ew>"};
};

std::vector<Token> Tokenize(const TokenStore& token_store,
                            std::string_view string);
}  // namespace uchen::tools::tokens

namespace std {

inline std::ostream& operator<<(std::ostream& os,
                                const uchen::tools::tokens::Token& token) {
  os << absl::StrCat(token);
  return os;
}

}  // namespace std

#endif  // __TOOLS_TOKENS_H