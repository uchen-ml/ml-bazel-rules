#include "tokens/tokens.h"

#include <cctype>
#include <compare>
#include <cstddef>
#include <initializer_list>
#include <map>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace uchen::tools::tokens {

namespace {

struct TuplesByElement {
  bool operator()(
      const std::pair<std::pair<std::string, std::string>, int>& v1,
      const std::pair<std::pair<std::string, std::string>, int>& v2) const {
    std::strong_ordering c = v1.second <=> v2.second;
    if (c != std::strong_ordering::equivalent) {
      return c == std::strong_ordering::greater;
    }
    c = v1.first.first <=> v2.first.first;
    if (c != std::strong_ordering::equivalent) {
      return c == std::strong_ordering::less;
    }
    return v1.first.second < v1.first.second;
  }
};

}  // namespace

std::map<std::string, size_t> Combine(std::span<const Token> tokens,
                                      size_t min_matches) {
  std::map<std::pair<std::string, std::string>, size_t> to_fuse;
  Token prev = tokens.front();
  for (const Token& token : std::span(tokens).subspan(1)) {
    if (!prev.is_special() && !token.is_special()) {
      to_fuse[{prev.name(), token.name()}] += 1;
    }
    prev = std::move(token);
  }
  std::set<std::pair<std::pair<std::string, std::string>, size_t>,
           TuplesByElement>
      s(std::make_move_iterator(to_fuse.begin()),
        std::make_move_iterator(to_fuse.end()));
  std::map<std::string, size_t> merged;
  std::unordered_set<std::string> processed;
  for (const auto& [tokens, count] : std::move(s)) {
    if (count < min_matches) {
      break;
    }
    if (processed.contains(tokens.first) || processed.contains(tokens.second)) {
      continue;
    }
    merged[tokens.first + tokens.second] = count;
    processed.emplace(tokens.first);
    processed.emplace(tokens.second);
  }
  return merged;
}

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

std::strong_ordering operator<=>(const Token& t1, const Token& t2) {
  std::strong_ordering ordering = t1.name() <=> t2.name();
  if (ordering != 0) {
    return ordering;
  }
  if (t1.is_special() == t2.is_special()) {
    return std::strong_ordering::equivalent;
  }
  if (t1.is_special()) {
    return std::strong_ordering::greater;
  }
  return std::strong_ordering::less;
}

bool operator==(const Token& token, std::string_view s) {
  return (token <=> s) == std::strong_ordering::equivalent;
}

// TokenStore class impl
TokenStore::TokenStore(std::initializer_list<const std::string_view> tokens) {
  for (std::string_view token : tokens) {
    Add(token);
  }
}

void TokenStore::Add(std::string_view token) {
  for (size_t i = 0; i < token.size(); i++) {
    tokens_.emplace(token.substr(0, i + 1));
  }
}

std::vector<Token> TokenStore::Tokenize(std::span<const char> input) const {
  std::unordered_map<char, std::string> magic_chars = {
      {'\r', "<nl>"}, {'\n', "<nl>"}, {'(', "<(>"}, {')', "<)>"},
      {'{', "<{>"},   {'}', "<}>"},   {'[', "<[>"}, {']', "<]>"}};
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
      auto it = magic_chars.find(c);
      if (it != magic_chars.end()) {
        current.emplace(it->second, true);
      } else {
        current.emplace(std::string_view(&c, 1), false);
      }
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