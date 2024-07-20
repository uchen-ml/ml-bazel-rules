#include "tokens/tokens.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"

namespace uchen::tools::tokens {
namespace {

TEST(TokensTest, AddsWordStartEnd) {
  TokenStore store;
  EXPECT_THAT(Tokenize(store, "Hello World"),
              ::testing::ElementsAre("<si>", "<sw>", "<uc>", "h", "e", "l", "l",
                                     "o", "<ew>", " ", "<sw>", "<uc>", "w", "o",
                                     "r", "l", "d", "<ew>", "<ei>"));
}

TEST(TokensTest, LowerUpperCase) {
  TokenStore store;
  EXPECT_THAT(Tokenize(store, "woRld"),
              ::testing::ElementsAre("<si>", "<sw>", "w", "o", "<uc>", "r", "l",
                                     "d", "<ew>", "<ei>"));
}

}  // namespace
}  // namespace uchen::tools::tokens

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}