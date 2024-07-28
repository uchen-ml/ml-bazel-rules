#include <cstddef>
#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"

#include "tokens/tokens.h"

ABSL_FLAG(std::optional<std::string>, output, std::nullopt,
          "Output destination");

namespace {

std::set<std::string> SortEnv(char* env[]) {
  std::set<std::string> sorted_env;
  for (size_t i = 0; env[i] != nullptr; ++i) {
    sorted_env.emplace(env[i]);
  }
  return sorted_env;
}

std::vector<std::vector<char>> ReadFiles(std::span<char*> paths) {
  std::vector<std::vector<char>> files_to_process;
  for (const auto& f : paths) {
    LOG(INFO) << absl::StrFormat("Processing %s", f);
    std::ifstream file(f);
    if (file) {
      files_to_process.emplace_back(std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>());
    } else {
      LOG(ERROR) << f << " does not exist";
    }
  }
  return files_to_process;
}

std::map<std::string, size_t> IdentifyTokens(std::span<const char> data) {
  uchen::tools::tokens::TokenStore store;
  for (int i = 0; i < 20; ++i) {
    auto tokens = store.Tokenize(data);
    std::vector<std::string> combined;
    auto combine = Combine(tokens, 5);
    if (combine.empty()) {
      break;
    }
    for (const auto& [token, count] : combine) {
      combined.emplace_back(absl::StrFormat("%s(%d)", token, count));
    }
    store.Update(combine);
  }
  std::map<std::string, size_t> tokens;
  for (const auto& token : store.Tokenize(data)) {
    if (!token.is_special() && token.name().size() > 1) {
      tokens[token.name()] += 1;
    }
  }
  return tokens;
}

}  // namespace

int main(int argc, char* argv[], char** env) {
  absl::SetProgramUsageMessage(absl::StrCat(
      "Generates bag of words from the input files. Sample usage:\n", argv[0],
      " <src_file1> <src_file2> ..."));
  auto remaining = absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  if (remaining.size() <= 1) {
    std::cerr << "At least one source file should be provided";
    return 1;
  }
  VLOG(1) << absl::StrJoin(SortEnv(env), "\n");

  for (const auto& data : ReadFiles(std::span(remaining).subspan(1))) {
    for (const auto& [token, count] : IdentifyTokens(data)) {
      LOG(INFO) << token << " " << count;
    }
  }
}