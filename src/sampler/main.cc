#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"

#include "src/sampler/path_matcher.h"

ABSL_FLAG(std::vector<std::string>, included, {}, "Included files");
ABSL_FLAG(std::vector<std::string>, excluded, {}, "Exclude paths");
ABSL_FLAG(std::vector<std::string>, ext, {}, "File extensions");
ABSL_FLAG(size_t, min_size, 0, "Minimum file size");
ABSL_FLAG(size_t, max_size, std::numeric_limits<size_t>::max(),
          "Maximum file size");
ABSL_FLAG(int, seed, 42, "Random seed");
ABSL_FLAG(size_t, samples, 0, "Number of samples");
ABSL_FLAG(std::optional<std::string>, output, std::nullopt,
          "Output file for the list of samples");

namespace {

template <typename S>
std::queue<std::filesystem::path> GetUniqueCanonicalPaths(const S& paths) {
  std::queue<std::filesystem::path> result;
  for (const auto& path : paths) {
    if (std::filesystem::is_directory(path)) {
      result.emplace(std::filesystem::canonical(path));
    } else {
      LOG(ERROR) << path << " was not found";
    }
  }
  return result;
}

}  // namespace

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  auto paths = absl::ParseCommandLine(argc, argv);
  absl::SetProgramUsageMessage(
      absl::StrCat("Usage: ", paths[0], " <directort>"));
  if (paths.size() < 2) {
    std::cerr << "Directory is required. Usage:\n  " << paths[0]
              << " <directory>\n";
    return 1;
  }
  std::filesystem::path directory = std::filesystem::canonical(paths[1]);
  LOG(INFO) << "Arguments: "
            << absl::StrJoin(std::span(argv, argc).subspan(1), " ");
  std::queue<std::filesystem::path> includes;
  for (const auto& include : absl::GetFlag(FLAGS_included)) {
    includes.emplace(directory / include);
  }
  std::vector<std::string> excludes;
  for (const auto& exclude : absl::GetFlag(FLAGS_excluded)) {
    excludes.emplace_back((directory / exclude).string());
  }
  uchen::data::PathMatcher matcher =
      uchen::data::PathMatcherBuilder()
          .excluding(excludes)
          .only_extensions(absl::GetFlag(FLAGS_ext))
          .set_min_size(absl::GetFlag(FLAGS_min_size))
          .set_max_size(absl::GetFlag(FLAGS_max_size))
          .build();
  std::set<std::string> files;
  std::string prefix = std::filesystem::current_path().string();
  while (!includes.empty()) {
    for (const auto& entry :
         std::filesystem::directory_iterator(includes.front())) {
      if (!std::filesystem::exists(entry)) {
        LOG(ERROR) << entry << " does not exist";
        continue;
      }
      if (std::filesystem::is_directory(entry)) {
        includes.emplace(entry);
        continue;
      }
      std::string path = entry.path().string();
      if (matcher(path)) {
        files.emplace(path.substr(prefix.size() + 1));
      }
    }
    includes.pop();
  }
  std::vector<std::string> samples(files.begin(), files.end());
  if (absl::GetFlag(FLAGS_samples) > 0) {
    std::mt19937 gen(absl::GetFlag(FLAGS_seed));
    // It is templting to use std::sample here, but it will generate entirely
    // new list when size changes, which causes more recomplutes than needed.
    std::shuffle(samples.begin(), samples.end(), gen);
    samples.resize(std::min(absl::GetFlag(FLAGS_samples), samples.size()));
    std::sort(samples.begin(), samples.end());
  }
  auto output = absl::GetFlag(FLAGS_output);
  if (output.has_value()) {
    std::ofstream out(output.value());
    if (!out) {
      LOG(ERROR) << "Failed to open " << output.value();
      return 1;
    }
    for (const auto& sample : samples) {
      out << sample << '\n';
    }
  } else {
    for (const auto& sample : samples) {
      LOG(INFO) << sample;
    }
  }
  LOG(INFO) << "Done";
  return 0;
}