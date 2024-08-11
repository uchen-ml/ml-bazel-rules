#ifndef PATH_MATCHER_H
#define PATH_MATCHER_H

#include <cstddef>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>

namespace uchen::data {

class PathMatcher {
 public:
  explicit PathMatcher(const std::unordered_set<std::string>& excludes,
                       const std::unordered_set<std::string>& exts,
                       size_t min_size, size_t max_size)
      : excludes_(excludes),
        exts_(exts),
        min_size_(min_size),
        max_size_(max_size) {}
  bool operator()(std::string_view path);

 private:
  std::unordered_set<std::string> excludes_;
  std::unordered_set<std::string> exts_;
  size_t min_size_;
  size_t max_size_;
};

class PathMatcherBuilder {
 public:
  PathMatcherBuilder() = default;

  PathMatcher build() const {
    return PathMatcher(excludes_, exts_, min_size_, max_size_);
  }

  PathMatcherBuilder including(std::span<const std::string> includes) const;
  PathMatcherBuilder excluding(std::span<const std::string> excludes) const;
  PathMatcherBuilder only_extensions(
      std::span<const std::string> includes) const;

  PathMatcherBuilder set_min_size(size_t size) const;
  PathMatcherBuilder set_max_size(size_t size) const;

 private:
  PathMatcherBuilder(const std::unordered_set<std::string>& excludes,
                     const std::unordered_set<std::string>& exts,
                     size_t min_size, size_t max_size)
      : excludes_(excludes),
        exts_(exts),
        min_size_(min_size),
        max_size_(max_size) {}

  std::unordered_set<std::string> excludes_;
  std::unordered_set<std::string> exts_;
  size_t min_size_ = 0;
  size_t max_size_ = std::numeric_limits<size_t>::max();
};

}  // namespace uchen::data

#endif  // PATH_MATCHER_H
