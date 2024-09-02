#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"

#include "src/bow/extract_words.h"

ABSL_FLAG(std::vector<std::string>, inputs, {}, "List of input files");
ABSL_FLAG(std::vector<std::string>, combine, {}, "List of files to combine");
ABSL_FLAG(std::string, output, {}, "Output file");

namespace uchen::tools::text {
namespace {

template <typename T>
struct LessBySecondElement {
  bool operator()(const std::pair<T, size_t>& v1,
                  const std::pair<T, size_t>& v2) const {
    if (v1.second == v2.second) {
      return v1.first > v2.first;
    }
    return v1.second > v2.second;
  }
};

std::unordered_map<std::string, uint32_t> BuildBagOfWords(
    const std::vector<std::string>& files) {
  std::unordered_map<std::string, uint32_t> counts;
  for (std::string_view file : files) {
    std::stringstream ss;
    std::ifstream sample(file.data());
    if (!sample) {
      LOG(ERROR) << "Unable to open file: " << file;
      continue;
    }
    ss << sample.rdbuf();
    for (const auto& [word, count] : ExtractWords(ss.view())) {
      counts[word] += count;
    }
  }
  return counts;
}

std::unordered_map<std::string, uint32_t> CombineBags(
    const std::vector<std::string>& files) {
  std::unordered_map<std::string, uint32_t> tokens;
  for (const auto& file : files) {
    std::ifstream input_file(file);
    if (!input_file) {
      LOG(ERROR) << "Unable to open file: " << file;
      continue;
    }
    for (std::string line; std::getline(input_file, line);) {
      // Find last tab
      size_t tab = line.find_last_of('\t');
      if (tab == std::string::npos) {
        LOG(ERROR) << "Invalid line: " << line << " in file: " << file;
        continue;
      }
      std::string token = line.substr(0, tab);
      std::string count_str = line.substr(tab + 1);
      if (token.empty() || count_str.empty()) {
        LOG(ERROR) << "Invalid line: " << line << " in file: " << file;
        continue;
      }
      for (auto c : line.substr(tab + 1)) {
        if (!std::isdigit(c)) {
          LOG(ERROR) << "Invalid line: " << line << " in file: " << file;
          continue;
        }
      }
      tokens[token] += std::stoul(line.substr(tab + 1));
    }
  }
  return tokens;
}

struct LessByFreq {
  bool operator()(const std::pair<std::string, uint32_t>& one,
                  const std::pair<std::string, uint32_t>& another) const {
    if (one.second > another.second) {
      return true;
    } else if (one.second < another.second) {
      return false;
    } else {
      return one.first < another.first;
    }
  }
};

std::vector<std::pair<std::string, uint32_t>> SortByFrequency(
    const std::unordered_map<std::string, uint32_t>& words) {
  std::vector<std::pair<std::string, uint32_t>> result;
  result.reserve(words.size());
  for (const auto& [word, freq] : words) {
    if (word.length() > 1) {
      result.emplace_back(word, freq);
    }
  }
  std::sort(result.begin(), result.end(), LessByFreq());
  LOG(INFO) << "Sorted " << result.size() << " tokens by frequency, top five: ";
  for (const auto& [word, freq] : std::span(result).first(5)) {
    LOG(INFO) << "    " << word << " -> " << freq;
  }

  return result;
}

}  // namespace
}  // namespace uchen::tools::text

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  absl::SetProgramUsageMessage(absl::StrFormat(
      "Usage: %s --inputlists <input_list_files> --output <output_file>",
      argv[0]));
  absl::ParseCommandLine(argc, argv);
  LOG(INFO) << absl::StrJoin(std::span(argv, argc), " ");
  // Create output file early to ensure no wasted work
  std::ofstream output(absl::GetFlag(FLAGS_output));
  if (!output) {
    LOG(ERROR) << "Unable to open output file: " << absl::GetFlag(FLAGS_output);
    return 1;
  }
  auto inputs = absl::GetFlag(FLAGS_inputs);
  auto combine = absl::GetFlag(FLAGS_combine);
  if (inputs.empty() && combine.empty()) {
    LOG(ERROR) << "Either --inputs or --combine must be specified";
    return 1;
  }
  if (!inputs.empty() && !combine.empty()) {
    LOG(ERROR) << "Only one of --inputs or --combine can be specified";
    return 1;
  }
  std::unordered_map<std::string, uint32_t> tokens =
      inputs.empty() ? uchen::tools::text::CombineBags(combine)
                     : uchen::tools::text::BuildBagOfWords(inputs);
  for (const auto& [token, count] :
       uchen::tools::text::SortByFrequency(tokens)) {
    output << token << "\t" << count << std::endl;
  }
  LOG(INFO) << "Wrote tokens to " << absl::GetFlag(FLAGS_output);
  return 0;
}