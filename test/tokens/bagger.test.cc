#include "tokens/bagger.h"

#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"

#include "gmock/gmock.h"
#include "tokens/tokens.h"

namespace uchen::tools::tokens {
namespace {

TEST(BaggerTest, DISABLED_BagsTogether) {
  TokenStore store;
  constexpr std::string_view data = "aaabaaab";
  auto stream = store.Tokenize(data);
  Bagger bagger;
  EXPECT_THAT(bagger.Lump(stream), ::testing::ElementsAre(::testing::Pair(
                                       ::testing::Pair("a", "a"), 2)));
  EXPECT_THAT(store.Tokenize(data),
              ::testing::ElementsAre("aa", "a", "b", "aa", "a", "b"));
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