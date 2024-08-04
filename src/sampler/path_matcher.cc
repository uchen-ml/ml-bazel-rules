#include "src/sampler/path_matcher.h"

#include <algorithm>
#include <filesystem>
#include <string_view>

namespace uchen::data {

bool PathMatcher::operator()(const std::filesystem::path& path) {
  if (path.string().length() == 0) {
    return false;
  }
  std::string path_string = path.string();
  if (!includes_.empty() &&
      std::none_of(includes_.begin(), includes_.end(),
                   [&](std::string_view prefix) -> bool {
                     return path_string.starts_with(prefix) &&
                            (path_string.length() == prefix.length() ||
                             (path_string[prefix.length()] == '/'));
                   })) {
    return false;
  }
  return true;
}

}  // namespace uchen::data