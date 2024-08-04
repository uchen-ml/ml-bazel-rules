#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"

ABSL_FLAG(std::string, base, "", "Base directory");
ABSL_FLAG(std::string, filelist, "", "List of files in the dataset");
ABSL_FLAG(std::vector<std::string>, include, {}, "Include paths");
ABSL_FLAG(std::vector<std::string>, exclude, {}, "Exclude paths");
ABSL_FLAG(std::vector<std::string>, extension, {},
          "Case-insensitive file extensions (including preceding .) for files "
          "that will be considered");

namespace {

class PathMatcher {
 public:
  PathMatcher(std::span<std::string> extension,
              std::span<const std::string> includes,
              std::span<const std::string> excludes)
      : extension_(extension.begin(), extension.end()),
        includes_(includes.begin(), includes.end()) {}

  bool Match(const std::filesystem::path& path) const {
    if (!includes_.empty() &&
        std::none_of(includes_.begin(),
                     includes_.end(), [&](std::string_view prefix))) {
    }
    LOG_FIRST_N(INFO, 5) << "Matches! " << path;
    return true;
  }

 private:
  std::unordered_set<std::string> extension_;
  std::unordered_set<std::string> includes_;
};

absl::StatusOr<std::set<std::filesystem::path>> GetCandidatePaths(
    const std::string& list_file, const PathMatcher& matcher) {
  std::ifstream f(list_file);
  if (!f) {
    return absl::NotFoundError(absl::StrFormat("Unable to open %s", list_file));
  }
  // Ordered intentionally
  std::set<std::filesystem::path> names;
  std::string s;
  while (std::getline(f, s)) {
    std::filesystem::path p(s);
    if (matcher.Match(p)) {
      names.emplace(p);
    }
  }
  return names;
}

}  // namespace

int main(int argc, char* argv[]) {
  auto files = absl::ParseCommandLine(argc, argv);
  absl::SetProgramUsageMessage(
      absl::StrCat("Usage: ", files[0], " --filelist <filelist_path>"));
  absl::InitializeLog();
  LOG(INFO) << "Arguments: "
            << absl::StrJoin(std::span(argv, argc).subspan(1), " ");
  auto filelist = absl::GetFlag(FLAGS_filelist);
  if (filelist.empty()) {
    std::cerr << "File list was not provided" << "\n"
              << absl::ProgramUsageMessage() << "\n";
  }
  PathMatcher matcher(absl::GetFlag(FLAGS_base), absl::GetFlag(FLAGS_include),
                      absl::GetFlag(FLAGS_exclude));
  auto candidates = GetCandidatePaths(filelist, matcher);
  if (!candidates.ok()) {
    LOG(ERROR) << candidates.status().message();
    return candidates.status().raw_code();
  }
  size_t ns = 0;
  for (const auto& p : *candidates) {
    ns += p.string().length();
  }
  LOG(INFO) << absl::StrFormat("%zu bytes in %zu entries", ns,
                               candidates->size());
  return 1;
}