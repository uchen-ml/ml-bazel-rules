#include "src/sampler/path_matcher.h"

#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_set>

#include "absl/algorithm/container.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/strip.h"

namespace uchen::data {

namespace {
std::string AddTrailingSlash(std::string_view path) {
  return absl::StrCat(absl::StripSuffix(path, "/"), "/");
}
}  // namespace

//
// PathMatcher
//
bool PathMatcher::operator()(std::string_view path) {
  if (path.length() == 0) {
    return false;
  }
  bool result = includes_.empty();
  for (const auto& prefix : includes_) {
    if (path.starts_with(prefix)) {
      result = true;
      break;
    }
  }
  if (!result) {
    return false;
  }
  for (const auto& prefix : excludes_) {
    if (path.starts_with(prefix)) {
      return false;
    }
  }
  result = exts_.empty();
  for (const auto& suffix : exts_) {
    if (path.ends_with(suffix)) {
      result = true;
      break;
    }
  }
  if (!result ||
      (min_size_ == 0 && max_size_ == std::numeric_limits<size_t>::max())) {
    return result;
  }
  std::filesystem::path fs_path(path);
  if (!std::filesystem::exists(fs_path)) {
    return false;
  }
  auto size = std::filesystem::file_size(fs_path);
  return size >= min_size_ && size <= max_size_;
}

//
// PathMatcherBuilder
//

PathMatcherBuilder PathMatcherBuilder::including(
    std::span<const std::string> includes) const {
  std::unordered_set<std::string> incls;
  absl::c_transform(includes, std::inserter(incls, incls.end()),
                    AddTrailingSlash);
  return PathMatcherBuilder(incls, excludes_, exts_, min_size_, max_size_);
}

PathMatcherBuilder PathMatcherBuilder::excluding(
    std::span<const std::string> excludes) const {
  std::unordered_set<std::string> excls;
  absl::c_transform(excludes, std::inserter(excls, excls.end()),
                    AddTrailingSlash);
  return PathMatcherBuilder(includes_, excls, exts_, min_size_, max_size_);
}

PathMatcherBuilder PathMatcherBuilder::only_extensions(
    std::span<const std::string> exts) const {
  std::unordered_set<std::string> set;
  absl::c_transform(exts, std::inserter(set, set.end()),
                    [](std::string_view ext) {
                      return absl::StrCat(".", absl::StripPrefix(ext, "."));
                    });
  return PathMatcherBuilder(includes_, excludes_, set, min_size_, max_size_);
}

PathMatcherBuilder PathMatcherBuilder::set_min_size(size_t size) const {
  return PathMatcherBuilder(includes_, excludes_, exts_, size, max_size_);
}

PathMatcherBuilder PathMatcherBuilder::set_max_size(size_t size) const {
  return PathMatcherBuilder(includes_, excludes_, exts_, min_size_, size);
}

}  // namespace uchen::data