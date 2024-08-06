#include "sampler/path_matcher.h"

#include <array>

#include <gtest/gtest.h>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"

#include "src/sampler/path_matcher.h"

namespace {

TEST(Matcher, MatchesIncludes) {
  using std::string_literals::operator""s;
  auto matcher = uchen::data::PathMatcherBuilder()
                     .including(std::array{"matches1"s, "mat/ches2"s})
                     .build();
  EXPECT_TRUE(matcher("matches1/f.a"));
  EXPECT_TRUE(matcher("mat/ches2/f.a"));
  EXPECT_FALSE(matcher("matches1_not/f.a"));
  EXPECT_FALSE(matcher("nomatch/a.a"));
  EXPECT_FALSE(matcher(""));
}

TEST(Matcher, MatchesExcludes) {
  using std::string_literals::operator""s;
  auto matcher = uchen::data::PathMatcherBuilder()
                     .excluding(std::array{"matches1"s, "mat/ches2"s})
                     .build();
  EXPECT_FALSE(matcher("matches1/f.a"));
  EXPECT_FALSE(matcher("mat/ches2/f.a"));
  EXPECT_FALSE(matcher(""));
  EXPECT_TRUE(matcher("matches1_not/f.a"));
  EXPECT_TRUE(matcher("nomatch/a.a"));
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

TEST(Matcher, AllFilters) {
  using std::string_literals::operator""s;
  auto matcher = uchen::data::PathMatcherBuilder()
                     .including(std::array{"include"s})
                     .excluding(std::array{"include/exclude"s})
                     .only_extensions(std::array{"a"s})
                     .build();
  EXPECT_TRUE(matcher("include/a.a"));
  EXPECT_FALSE(matcher("include/a.b"));
  EXPECT_FALSE(matcher("include/exclude/a.a"));
  EXPECT_FALSE(matcher("exclude/a.a"));
  EXPECT_FALSE(matcher("include/exclude/a.b"));
  EXPECT_FALSE(matcher(""));
}

TEST(Matcher, NoFilters) {
  auto matcher = uchen::data::PathMatcherBuilder().build();
  EXPECT_TRUE(matcher("boop"));
  EXPECT_FALSE(matcher(""));
}

}  // namespace

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}