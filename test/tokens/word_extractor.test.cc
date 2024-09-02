#include <cctype>
#include <string_view>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "src/bow/extract_words.h"

namespace uchen::tools::text {

namespace {

TEST(WordsExtractorTest, AddsWordStartEnd) {
  using std::string_literals::operator""s;
  EXPECT_THAT(ExtractWords("let a1 = 10;"),
              ::testing::UnorderedElementsAre(
                  ::testing::Pair("let"s, 1), ::testing::Pair("a1"s, 1),
                  ::testing::Pair("="s, 1), ::testing::Pair("10"s, 1),
                  ::testing::Pair(";"s, 1)));
}

TEST(WordsExtractorTest, CamelCase) {
  EXPECT_THAT(ExtractWords("getUrl(10.2f);"),
              ::testing::UnorderedElementsAre(
                  ::testing::Pair("get", 1), ::testing::Pair("url", 1),
                  ::testing::Pair("(", 1), ::testing::Pair("10.2f", 1),
                  ::testing::Pair(")", 1), ::testing::Pair(";", 1)));
}

TEST(WordsExtractorTest, UpperCase) {
  EXPECT_THAT(ExtractWords("buildURL(\"Crazy!\"); // comment\nauto k='A';"),
              ::testing::UnorderedElementsAre(
                  ::testing::Pair("build", 1), ::testing::Pair("url", 1),
                  ::testing::Pair("(", 1), ::testing::Pair("crazy", 1),
                  ::testing::Pair("!", 1), ::testing::Pair("\"", 2),
                  ::testing::Pair(")", 1), ::testing::Pair("//", 1),
                  ::testing::Pair("comment", 1), ::testing::Pair("auto", 1),
                  ::testing::Pair("k", 1), ::testing::Pair("=", 1),
                  ::testing::Pair("'", 2), ::testing::Pair("a", 1),
                  ::testing::Pair(";", 2)));
}

}  // namespace
}  // namespace uchen::tools::text

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}