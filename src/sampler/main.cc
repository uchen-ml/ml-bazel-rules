#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "absl/algorithm/container.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

#include "src/sampler/path_matcher.h"

ABSL_FLAG(std::vector<std::string>, exclude, {}, "Exclude paths");
ABSL_FLAG(std::vector<std::string>, ext, {},
          "Case-insensitive file extensions (including preceding .) for files "
          "that will be considered");
ABSL_FLAG(size_t, min_size, 0, "Minimum file size");
ABSL_FLAG(size_t, max_size, std::numeric_limits<size_t>::max(),
          "Maximum file size");
ABSL_FLAG(int, seed, 42, "Random seed");
ABSL_FLAG(size_t, samples, std::numeric_limits<size_t>::max(),
          "Number of samples");
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

std::vector<std::filesystem::path> FilterFiles(
    std::span<char*> paths, const uchen::data::PathMatcher& matcher,
    size_t min_size, size_t max_size) {
  std::queue<std::filesystem::path> includes =
      GetUniqueCanonicalPaths(paths.subspan(1));
  std::unordered_set<std::filesystem::path> processed;
  std::set<std::filesystem::path> files;
  while (!includes.empty()) {
    processed.emplace(includes.front());
    for (const auto& entry :
         std::filesystem::directory_iterator(includes.front())) {
      if (!std::filesystem::exists(entry)) {
        LOG(ERROR) << entry << " does not exist";
        continue;
      }
      auto canonical = std::filesystem::canonical(entry);
      if (!matcher(canonical.string())) {
        continue;
      }
      if (std::filesystem::is_directory(entry)) {
        if (!processed.contains(canonical)) {
          includes.push(canonical);
        }
      } else {
        size_t size = std::filesystem::file_size(canonical);
        if (size > min_size && size < max_size) {
          files.emplace(canonical);
        }
      }
    }
    includes.pop();
  }
  return {std::make_move_iterator(files.begin()),
          std::make_move_iterator(files.end())};
}

}  // namespace

int main(int argc, char* argv[]) {
  auto paths = absl::ParseCommandLine(argc, argv);
  absl::SetProgramUsageMessage(
      absl::StrCat("Usage: ", paths[0], " <directories>"));
  absl::InitializeLog();
  if (paths.size() == 1) {
    std::cerr << "No source directories were provided.\n  "
              << absl::ProgramUsageMessage();
    return 1;
  }
  auto matcher = uchen::data::PathMatcherBuilder()
                     .excluding(absl::GetFlag(FLAGS_exclude))
                     .only_extensions(absl::GetFlag(FLAGS_ext))
                     .build();
  auto files = FilterFiles(paths, matcher, absl::GetFlag(FLAGS_min_size),
                           absl::GetFlag(FLAGS_max_size));
  std::vector<std::filesystem::path> samples;
  std::mt19937 gen(absl::GetFlag(FLAGS_seed));
  absl::c_sample(files, std::back_inserter(samples),
                 absl::GetFlag(FLAGS_samples), gen);
  if (absl::GetFlag(FLAGS_output).has_value()) {
    std::ofstream output(absl::GetFlag(FLAGS_output).value());
    for (const auto& path : samples) {
      output << path.string() << "\n";
    }
    LOG(INFO) << "Wrote " << samples.size() << " samples to "
              << absl::GetFlag(FLAGS_output).value();
  } else {
    for (const auto& path : samples) {
      std::cout << path.string() << "\n";
    }
  }
  return 0;
}