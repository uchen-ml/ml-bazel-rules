#include "tokens/bagger.h"

#include <compare>
#include <iterator>
#include <span>
#include <string_view>
#include <unordered_set>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"

#include "gmock/gmock.h"
#include "tokens/tokens.h"

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

std::vector<std::string> Combine(std::span<const Token> tokens) {
  std::map<std::pair<std::string, std::string>, int> to_fuse;
  Token prev = tokens.front();
  for (const Token& token : std::span(tokens).subspan(1)) {
    if (!prev.is_special() && !token.is_special()) {
      to_fuse[{prev.name(), token.name()}] += 1;
    }
    prev = std::move(token);
  }
  std::set<std::pair<std::pair<std::string, std::string>, int>, TuplesByElement>
      s(std::make_move_iterator(to_fuse.begin()),
        std::make_move_iterator(to_fuse.end()));
  std::vector<std::string> merged;
  std::unordered_set<std::string> processed;
  for (const auto& [tokens, count] : std::move(s)) {
    if (processed.contains(tokens.first) || processed.contains(tokens.second)) {
      continue;
    }
    merged.emplace_back(tokens.first + tokens.second);
    processed.emplace(tokens.first);
    processed.emplace(tokens.second);
  }
  return merged;
}

TEST(BaggerTest, BagsTogether) {
  TokenStore store;
  constexpr std::string_view data = "aaabaknaabkn";
  auto merged = Combine(store.Tokenize(data));
  EXPECT_THAT(merged, ::testing::ElementsAre("aa", "kn"));
  store.Update(merged);
  merged = Combine(store.Tokenize(data));
  EXPECT_THAT(merged, ::testing::ElementsAre("ab", "knaa"));
  store.Update(merged);
  using std::string_literals::operator""s;
  EXPECT_THAT(store.Tokenize(absl::StrCat("+ &"s, data, "l n")),
              ::testing::ElementsAre("<si>", "+", " ", "&", "<sw>", "aa", "ab",
                                     "a", "knaa", "b", "kn", "l", "<ew>", " ",
                                     "<sw>", "n", "<ew>", "<ei>"));
}

TEST(BaggerTest, EmptyInput) {
  TokenStore store;
  EXPECT_THAT(Bagger().Lump(store.Tokenize("")), ::testing::IsEmpty());
}

}  // namespace
}  // namespace uchen::tools::tokens

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}