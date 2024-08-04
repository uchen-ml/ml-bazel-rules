#include "sampler/path_matcher.h"

#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"

namespace {

TEST(Matcher, MatchesIncludes) {
  uchen::data::PathMatcher matcher(std::array<std::string, 1>({"matches"}));
  EXPECT_TRUE(matcher("matches/f.a"));
  EXPECT_FALSE(matcher("matches_not/f.a"));
  EXPECT_FALSE(matcher("nomatch/a.a"));
  EXPECT_FALSE(matcher(""));
}

TEST(Matcher, NoIncludes) {
  uchen::data::PathMatcher matcher(std::array<std::string, 0>{});
  EXPECT_TRUE(matcher("any"));
  EXPECT_FALSE(matcher(""));
}

}  // namespace

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}