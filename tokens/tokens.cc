#include "tokens/tokens.h"

#include <cctype>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "absl/log/log.h"
#include "absl/strings/str_join.h"

namespace uchen::tools::tokens {

// Token class impl
std::strong_ordering operator<=>(const Token& token, std::string_view s) {
  if (token.name() < s) {
    return std::strong_ordering::less;
  } else if (token.name() > s) {
    return std::strong_ordering::greater;
  } else {
    return std::strong_ordering::equivalent;
  }
}

bool operator==(const Token& token, std::string_view s) {
  return (token <=> s) == std::strong_ordering::equivalent;
}

// TokenStore class impl
std::vector<Token> TokenStore::Tokenize(std::string_view input) const {
  std::vector<Token> tokens{{"<si>", true}};
  bool is_word = false;
  std::optional<Token> current;
  for (char c : input) {
    if (current != std::nullopt) {
      std::string candidate =
          absl::StrCat(current->name(), std::string_view(&c, 1));
      if (tokens_.contains(candidate)) {
        current = Token(candidate, false);
        continue;
      } else {
        tokens.emplace_back(std::move(*current));
      }
    }
    if (isIdentifierChar(c)) {
      if (!is_word) {
        tokens.emplace_back("<sw>", true);
        is_word = true;
      }
      char ch = c;
      if (std::isupper(ch)) {
        tokens.emplace_back("<uc>", true);
        ch = std::tolower(c);
      }
      current.emplace(std::string_view(&ch, 1), false);
    } else {
      if (is_word) {
        tokens.emplace_back("<ew>", true);
        is_word = false;
      }
      current.emplace(std::string_view(&c, 1), false);
    }
  }
  if (current.has_value()) {
    tokens.emplace_back(std::move(*current));
  }
  if (is_word) {
    tokens.emplace_back("<ew>", true);
  }
  tokens.emplace_back("<ei>", true);
  return tokens;
}

}  // namespace uchen::tools::tokens