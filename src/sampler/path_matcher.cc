#include "src/sampler/path_matcher.h"

#include <cstdlib>
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

bool PathMatcher::operator()(std::string_view path) const {
  if (path.length() == 0) {
    return false;
  }
  for (const auto& prefix : excludes_) {
    if (path.starts_with(prefix)) {
      return false;
    }
  }
  bool result = exts_.empty();
  for (const auto& suffix : exts_) {
    if (path.ends_with(suffix)) {
      result = true;
      break;
    }
  }
  return result;
}

//
// PathMatcherBuilder
//

PathMatcherBuilder PathMatcherBuilder::excluding(
    std::span<const std::string> excludes) const {
  std::unordered_set<std::string> excls;
  absl::c_transform(excludes, std::inserter(excls, excls.end()),
                    AddTrailingSlash);
  return PathMatcherBuilder(excls, exts_);
}

PathMatcherBuilder PathMatcherBuilder::only_extensions(
    std::span<const std::string> exts) const {
  std::unordered_set<std::string> set;
  absl::c_transform(exts, std::inserter(set, set.end()),
                    [](std::string_view ext) {
                      return absl::StrCat(".", absl::StripPrefix(ext, "."));
                    });
  return PathMatcherBuilder(excludes_, set);
}

}  // namespace uchen::data