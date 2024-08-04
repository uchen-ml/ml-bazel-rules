#ifndef PATH_MATCHER_H
#define PATH_MATCHER_H

#include <filesystem>
#include <span>
#include <unordered_set>

namespace uchen::data {

class PathMatcher {
 public:
  explicit PathMatcher(std::span<const std::string> includes)
      : includes_(includes.begin(), includes.end()) {}
  bool operator()(const std::filesystem::path& path);

 private:
  std::unordered_set<std::string> includes_;
};

}  // namespace uchen::data

#endif  // PATH_MATCHER_H
