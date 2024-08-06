#ifndef PATH_MATCHER_H
#define PATH_MATCHER_H

#include <span>
#include <string>
#include <string_view>
#include <unordered_set>

namespace uchen::data {

class PathMatcher {
 public:
  explicit PathMatcher(const std::unordered_set<std::string>& includes,
                       const std::unordered_set<std::string>& excludes,
                       const std::unordered_set<std::string>& exts)
      : includes_(includes), excludes_(excludes), exts_(exts) {}
  bool operator()(std::string_view path);

 private:
  std::unordered_set<std::string> includes_;
  std::unordered_set<std::string> excludes_;
  std::unordered_set<std::string> exts_;
};

class PathMatcherBuilder {
 public:
  PathMatcherBuilder() = default;

  PathMatcher build() const { return PathMatcher(includes_, excludes_, exts_); }

  PathMatcherBuilder including(std::span<const std::string> includes) const;
  PathMatcherBuilder excluding(std::span<const std::string> excludes) const;
  PathMatcherBuilder only_extensions(
      std::span<const std::string> includes) const;

 private:
  PathMatcherBuilder(const std::unordered_set<std::string>& includes,
                     const std::unordered_set<std::string>& excludes,
                     const std::unordered_set<std::string>& exts)
      : includes_(includes), excludes_(excludes), exts_(exts) {}

  std::unordered_set<std::string> includes_;
  std::unordered_set<std::string> excludes_;
  std::unordered_set<std::string> exts_;
};

}  // namespace uchen::data

#endif  // PATH_MATCHER_H
