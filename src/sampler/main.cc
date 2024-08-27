#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <limits>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <string_view>
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
ABSL_FLAG(std::string, output, {}, "Output directory for samples");
ABSL_FLAG(size_t, dirs, 0, "Number of directories to split samples");

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

std::vector<std::string> BuildFileList(
    const std::filesystem::path& directory,
    const uchen::data::PathMatcher& matcher,
    const std::filesystem::path& prefix = std::filesystem::current_path()) {
  size_t prefix_len = prefix.string().length() + 1;
  std::set<std::string> files;
  std::queue<std::filesystem::path> includes;
  for (const auto& include : absl::GetFlag(FLAGS_included)) {
    includes.emplace(directory / include);
  }
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
        files.emplace(path.substr(prefix_len));
      }
    }
    includes.pop();
  }
  return std::vector<std::string>(std::move_iterator(files.begin()),
                                  std::move_iterator(files.end()));
}

}  // namespace

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  auto paths = absl::ParseCommandLine(argc, argv);
  absl::SetProgramUsageMessage(
      absl::StrCat("Usage: ", paths[0], " <directory>"));
  LOG(INFO) << "Arguments: "
            << absl::StrJoin(std::span(argv, argc).subspan(1), " ");
  if (paths.size() < 2) {
    std::cerr << "Directory is required. Usage:\n  " << paths[0]
              << " <directory>\n";
    return 1;
  }
  std::filesystem::path directory = std::filesystem::canonical(paths[1]);
  std::vector<std::string> excludes;
  for (const auto& exclude : absl::GetFlag(FLAGS_excluded)) {
    excludes.emplace_back((directory / exclude).string());
  }
  std::string output = absl::GetFlag(FLAGS_output);
  if (output.empty()) {
    LOG(ERROR) << "Output directory is required";
    return 1;
  }
  std::filesystem::path output_dir(output);
  if (!std::filesystem::is_directory(output_dir)) {
    LOG(ERROR) << output_dir << " is not a valid directory";
    return 1;
  }
  auto matcher = uchen::data::PathMatcherBuilder()
                     .excluding(excludes)
                     .only_extensions(absl::GetFlag(FLAGS_ext))
                     .set_min_size(absl::GetFlag(FLAGS_min_size))
                     .set_max_size(absl::GetFlag(FLAGS_max_size))
                     .build();
  std::vector<std::string> samples = BuildFileList(directory, matcher);
  size_t samples_needed = absl::GetFlag(FLAGS_samples);
  if (samples.size() < samples_needed) {
    std::cerr << "Not enough samples found, " << samples.size()
              << " samples found\n";
    return 1;
  }
  if (samples_needed > 0) {
    std::mt19937 gen(absl::GetFlag(FLAGS_seed));
    // It is templting to use std::sample here, but it will generate entirely
    // new list when when number of samples changes, which causes more
    // recomputes than needed.
    std::shuffle(samples.begin(), samples.end(), gen);
    samples.resize(std::min(samples_needed, samples.size()));
  }
  size_t dirs = absl::GetFlag(FLAGS_dirs);
  if (dirs == 0) {
    std::cerr << "Number of directories is required\n";
    return 1;
  }
  size_t samples_per_dir = (samples.size() - 1 + dirs) / dirs;
  for (size_t d = 0; d < dirs; ++d) {
    auto dir = output_dir / std::to_string(d + 1);
    std::filesystem::create_directory(dir);
    for (size_t i = 0; i < samples_per_dir; ++i) {
      size_t ind = d * samples_per_dir + i;
      if (ind >= samples.size()) {
        break;
      }
      std::filesystem::path dst = dir / absl::StrCat(i + 1, ".sample");
      std::filesystem::copy_file(samples[ind], dst);
    }
  }
  LOG(INFO) << "Done, " << samples.size() << " samples copied";
  return 0;
}