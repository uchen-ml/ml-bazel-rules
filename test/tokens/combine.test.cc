#include <span>
#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"

#include "gmock/gmock.h"
#include "tokens/bagger.h"
#include "tokens/tokens.h"

namespace uchen::tools::tokens {
namespace {

TEST(CombineTest, Combines) {
  TokenStore store;
  constexpr std::string_view data = "aaabaknaabkn";
  auto merged = Combine(store.Tokenize(data));
  EXPECT_THAT(merged, ::testing::ElementsAre(::testing::Pair("aa", 3),
                                             ::testing::Pair("kn", 2)));
  store.Update(merged);
  merged = Combine(store.Tokenize(data));
  EXPECT_THAT(merged, ::testing::ElementsAre(::testing::Pair("ab", 1),
                                             ::testing::Pair("knaa", 1)));
  store.Update(merged);
  using std::string_literals::operator""s;
  EXPECT_THAT(store.Tokenize(absl::StrCat("+ &"s, data, "l n")),
              ::testing::ElementsAre("<si>", "+", " ", "&", "<sw>", "aa", "ab",
                                     "a", "knaa", "b", "kn", "l", "<ew>", " ",
                                     "<sw>", "n", "<ew>", "<ei>"));
}

TEST(CombineTest, EmptyInput) {
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