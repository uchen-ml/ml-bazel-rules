#include "sampler/path_matcher.h"

#include <array>
#include <string>

#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"

#include "src/sampler/path_matcher.h"

namespace {

TEST(Matcher, MatchesIncludes) {
  auto matcher =
      uchen::data::PathMatcherBuilder()
          .including(std::array<std::string, 2>({"matches1", "matches2"}))
          .build();
  EXPECT_TRUE(matcher("matches1/f.a"));
  EXPECT_TRUE(matcher("matches2/f.a"));
  EXPECT_FALSE(matcher("matches1_not/f.a"));
  EXPECT_FALSE(matcher("nomatch/a.a"));
  EXPECT_FALSE(matcher(""));
}

TEST(Matcher, MatchesExcludes) {
  auto matcher =
      uchen::data::PathMatcherBuilder()
          .excluding(std::array<std::string, 2>({"matches1", "matches2"}))
          .build();
  EXPECT_TRUE(matcher("matches1/f.a"));
  EXPECT_TRUE(matcher("matches2/f.a"));
  EXPECT_FALSE(matcher("matches1_not/f.a"));
  EXPECT_FALSE(matcher("nomatch/a.a"));
  EXPECT_FALSE(matcher(""));
}

TEST(Matcher, MatchesIncludesExcludes) {}

TEST(Matcher, MatchesExtension) {
  using std::string_literals::operator""s;
  auto matcher = uchen::data::PathMatcherBuilder()
                     .only_extensions(std::array{"boop"s, ".beep"s, ".bup.z"s})
                     .build();
  EXPECT_TRUE(matcher("a/b/c.boop"));
  EXPECT_TRUE(matcher("a/b/c.beep"));
  EXPECT_TRUE(matcher("a/b/c.bup.z"));
  EXPECT_FALSE(matcher("a/b/cboop"));
  EXPECT_FALSE(matcher("a/b/cbeep"));
  EXPECT_FALSE(matcher("a/b/c.z"));
  EXPECT_FALSE(matcher("a/b/c"));
  EXPECT_FALSE(matcher(""));
}

TEST(Matcher, DISABLED_AllFilters) {}

TEST(Matcher, DISABLED_NoFilters) {
  // uchen::data::PathMatcher matcher(std::array<std::string, 0>{},
  // std::array<std::string, 0>{});
}

}  // namespace

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}