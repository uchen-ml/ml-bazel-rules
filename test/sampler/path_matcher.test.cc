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

TEST(Matcher, FileSize) {
  auto matcher = uchen::data::PathMatcherBuilder().set_min_size(15).build();
  EXPECT_FALSE(matcher("test/sampler/data/include/10.sample"));
  EXPECT_TRUE(matcher("test/sampler/data/include/20.sample"));
  EXPECT_TRUE(matcher("test/sampler/data/include/30.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/40.sample"));
  EXPECT_FALSE(matcher(""));
  matcher = uchen::data::PathMatcherBuilder()
                .set_min_size(15)
                .set_max_size(25)
                .build();
  EXPECT_FALSE(matcher("test/sampler/data/include/10.sample"));
  EXPECT_TRUE(matcher("test/sampler/data/include/20.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/30.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/40.sample"));
  EXPECT_FALSE(matcher(""));
  matcher = uchen::data::PathMatcherBuilder().set_max_size(25).build();
  EXPECT_TRUE(matcher("test/sampler/data/include/10.sample"));
  EXPECT_TRUE(matcher("test/sampler/data/include/20.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/30.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/40.sample"));
  EXPECT_FALSE(matcher(""));
}

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
  auto matcher =
      uchen::data::PathMatcherBuilder()
          .including(std::array{"test/sampler/data/include"s})
          .excluding(std::array{"test/sampler/data/include/exclude"s})
          .only_extensions(std::array{"sample"s})
          .set_min_size(15)
          .set_max_size(25)
          .build();
  EXPECT_TRUE(matcher("test/sampler/data/include/20.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/doesnotexist.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/exclude/20.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/exclude/20.sample"));
  EXPECT_FALSE(matcher("test/sampler/data/include/exclude/a.b"));
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