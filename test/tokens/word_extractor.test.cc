#include <cctype>
#include <string_view>

#include "absl/log/globals.h"
#include "absl/log/initialize.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "src/bow/extract_words.h"

namespace uchen::tools::text {

namespace {

using ::testing::Pair;

TEST(WordsExtractorTest, AddsWordStartEnd) {
  using std::string_literals::operator""s;
  EXPECT_THAT(ExtractWords("let a1 = 10;"),
              ::testing::UnorderedElementsAre(Pair("let"s, 1), Pair("a1"s, 1),
                                              Pair("="s, 1), Pair("10"s, 1),
                                              Pair(";"s, 1)));
}

TEST(WordsExtractorTest, CamelCase) {
  EXPECT_THAT(ExtractWords("getUrl(10.2f);"),
              ::testing::UnorderedElementsAre(
                  Pair("get", 1), Pair("url", 1), Pair(".", 1), Pair("2f", 1),
                  Pair("(", 1), Pair("10", 1), Pair(")", 1), Pair(";", 1)));
}

TEST(WordsExtractorTest, UpperCase) {
  EXPECT_THAT(
      ExtractWords("buildURL(\"Crazy!\"); // comment\nauto k='A';"),
      ::testing::UnorderedElementsAre(
          Pair("build", 1), Pair("url", 1), Pair("(", 1), Pair("crazy", 1),
          Pair("!", 1), Pair("\"", 2), Pair(")", 1), Pair("//", 1),
          Pair("comment", 1), Pair("auto", 1), Pair("k", 1), Pair("=", 1),
          Pair("'", 2), Pair("a", 1), Pair(";", 2)));
}

TEST(WordsExtractorTest, Dot) {
  EXPECT_THAT(ExtractWords("example.service.multiple.connections"),
              ::testing::UnorderedElementsAre(
                  Pair(".", 3), Pair("example", 1), Pair("service", 1),
                  Pair("connections", 1), Pair("multiple", 1)));
}

}  // namespace
}  // namespace uchen::tools::text

int main() {
  absl::InitializeLog();
  absl::SetStderrThreshold(absl::LogSeverity::kInfo);
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}