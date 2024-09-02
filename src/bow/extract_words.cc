#include "src/bow/extract_words.h"

#include <unordered_set>

#include "absl/log/log.h"
#include "absl/strings/ascii.h"

namespace uchen::tools::text {
namespace {

class ExtractorState {
 public:
  bool isEnd(char c) {
    if (isUnfusible(c)) {
      current_type_ = TokenType::kNone;
      return true;
    }
    if (absl::ascii_isalnum(c) ||
        (c == '.' && current_type_ == TokenType::kAlNum)) {
      if (absl::ascii_isupper(c)) {
        TokenType old = current_type_;
        current_type_ =
            old == TokenType::kUpperCase || old == TokenType::kSingleUpper
                ? TokenType::kUpperCase
                : TokenType::kSingleUpper;
        return old != TokenType::kUpperCase && old != TokenType::kSingleUpper;
      } else {
        TokenType old = current_type_;
        current_type_ = TokenType::kAlNum;
        return old != TokenType::kAlNum && old != TokenType::kSingleUpper;
      }
    }
    if (isOperator(c)) {
      TokenType old = current_type_;
      current_type_ = TokenType::kOperator;
      return old != TokenType::kOperator;
    }
    if (!absl::ascii_isspace(c)) {
      LOG(INFO) << "Unknown character: " << c;
    }
    return true;
  }

  bool isSkippable(char c) { return absl::ascii_isspace(c); }

 private:
  enum class TokenType {
    kNone,
    kAlNum,
    kUpperCase,
    kSingleUpper,
    kOperator,
  };

  bool isOperator(char c) {
    return !std::isspace(c) && !std::isalnum(c) && !isUnfusible(c);
  }

  bool isUnfusible(char c) {
    return unfusible.find(c) != unfusible.end() || isSkippable(c);
  }

  TokenType current_type_ = TokenType::kNone;
  std::unordered_set<char> unfusible = {'(', ')',  '[',  ']', '{',
                                        '}', '\'', '\"', ',', ';'};
};
}  // namespace

std::unordered_map<std::string, uint32_t> ExtractWords(
    std::string_view stream) {
  std::unordered_map<std::string, uint32_t> word_counts;
  ExtractorState state;
  size_t start = 0;
  for (size_t i = 0; i < stream.length(); ++i) {
    if (state.isEnd(stream[i])) {
      if (start < i) {
        word_counts[absl::AsciiStrToLower(stream.substr(start, i - start))] +=
            1;
        start = i;
      }
    }
    if (state.isSkippable(stream[i])) {
      start = i + 1;
    }
  }
  if (start < stream.length()) {
    word_counts[absl::AsciiStrToLower(stream.substr(start))] += 1;
  }
  return word_counts;
}

}  // namespace uchen::tools::text