#include "tokens/tokens.h"

#include <cctype>
#include <compare>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_set>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "gmock/gmock.h"

namespace uchen::tools::tokens {
namespace {

TEST(TokensTest, AddsWordStartEnd) {
  TokenStore store;
  EXPECT_THAT(store.Tokenize("Hello World"),
              ::testing::ElementsAre("<si>", "<sw>", "<uc>", "h", "e", "l", "l",
                                     "o", "<ew>", " ", "<sw>", "<uc>", "w", "o",
                                     "r", "l", "d", "<ew>", "<ei>"));
}

TEST(TokensTest, LowerUpperCase) {
  TokenStore store;
  EXPECT_THAT(store.Tokenize("woRld"),
              ::testing::ElementsAre("<si>", "<sw>", "w", "o", "<uc>", "r", "l",
                                     "d", "<ew>", "<ei>"));
}

TEST(TokensTest, EmptyInput) {
  TokenStore store;
  EXPECT_THAT(store.Tokenize(""), ::testing::ElementsAre("<si>", "<ei>"));
}

TEST(TokensTest, UsesLongToken) {
  TokenStore store({"abc"});
  EXPECT_THAT(store.Tokenize("deabcde"),
              ::testing::ElementsAre("<si>", "<sw>", "d", "e", "abc", "d", "e",
                                     "<ew>", "<ei>"));
}

}  // namespace
}  // namespace uchen::tools::tokens

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}