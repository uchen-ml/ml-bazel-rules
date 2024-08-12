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

std::map<std::string, size_t> IdentifyTokens(std::span<std::string> ts,
                                             std::span<const char> data) {
  uchen::tools::tokens::TokenStore store(ts);
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
  std::map<std::string, size_t> prev;
  std::vector<std::string> init;
  for (const auto& data : ReadFiles(std::span(remaining).subspan(1))) {
    for (size_t i = 0; i < 3; ++i) {
      init.clear();
      for (const auto& [token, count] : prev) {
        if (count >= 6) {
          init.emplace_back(token);
        }
      }
      auto cur = IdentifyTokens(init, data);
      for (const auto& [token, count] : cur) {
        std::string prefix = "     ";
        auto it = prev.find(token);
        if (it == prev.end()) {
          prefix = absl::StrCat("+", count);
        } else {
          if (it->second != count) {
            prefix = absl::StrCat(
                count > it->second ? "+" : "",
                static_cast<long>(count) - static_cast<long>(it->second), " ",
                count);
          } else {
            prefix = absl::StrCat(count);
          }
          prev.erase(it);
        }
        LOG(INFO) << token << " " << prefix;
      }
      for (const auto& [token, count] : prev) {
        LOG(INFO) << token << " " << -static_cast<long>(count);
      }
      prev = std::move(cur);
      LOG(INFO) << "----";
    }
  }
}