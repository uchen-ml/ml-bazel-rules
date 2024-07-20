#include "tokens/tokens.h"

#include <cctype>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "absl/log/log.h"
#include "absl/strings/str_join.h"

namespace uchen::tools::tokens {

std::vector<Token> Tokenize(const TokenStore& token_store,
                            std::string_view string) {
  std::vector<Token> result;
  result.emplace_back(token_store.special_tokens().input_start);
  bool is_word = false;
  for (char c : string) {
    if (std::isalnum(c)) {
      if (!is_word) {
        result.emplace_back(token_store.special_tokens().identifier_start);
        is_word = true;
      }
    } else if (is_word) {
      result.emplace_back(token_store.special_tokens().identifier_end);
      is_word = false;
    }
    if (std::isupper(c)) {
      result.emplace_back(token_store.special_tokens().upper_case);
    }
    result.emplace_back(token_store.GetToken(std::tolower(c)));
  }
  if (is_word) {
    result.emplace_back(token_store.special_tokens().identifier_end);
  }
  result.emplace_back(token_store.special_tokens().input_end);
  return result;
}

// Token class impl
std::string Token::string() const { return store_->token_name(id_); }

std::strong_ordering Token::operator<=>(const char* sv) const {
  return *this <=> store_->FromString(sv);
}

// TokenStore class impl
TokenStore::TokenStore() {
  first_char_ = token_names_.size();
  for (int i = 0; i < 127; ++i) {
    token_names_.emplace_back(1, static_cast<char>(i));
  }
}

Token TokenStore::GetToken(char c) const {
  if (c + first_char_ >= token_names_.size()) {
    return special_tokens_.unknown;
  } else {
    return Token(this, first_char_ + c + 1);
  }
}

Token TokenStore::FromString(std::string_view sv) const {
  if (sv.length() == 1) {
    return GetToken(sv[0]);
  } else {
    for (int i = 0; i < first_char_; ++i) {
      if (sv == token_names_[i]) {
        return Token(this, i + 1);
      }
    }
  }
  return special_tokens_.unknown;
}

}  // namespace uchen::tools::tokens