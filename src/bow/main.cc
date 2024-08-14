#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"

#include "src/bow/tokens.h"

ABSL_FLAG(std::vector<std::string>, inputs, {}, "List of input files");
ABSL_FLAG(std::vector<std::string>, combine, {}, "List of files to combine");
ABSL_FLAG(std::string, output, {}, "Output file");
ABSL_FLAG(size_t, token_apperances, 3, "Minimum number of token apperances");

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

std::string ReadInputBuffer(std::span<const std::string> files) {
  std::stringstream ss;
  for (std::string_view file : files) {
    std::ifstream list_file(file.data());
    if (!list_file) {
      LOG(ERROR) << "Unable to open file: " << file;
      continue;
    }
    for (std::string line; std::getline(list_file, line);) {
      std::ifstream input_file(std::filesystem::current_path() / line);
      if (!input_file) {
        LOG(ERROR) << "Unable to open file: "
                   << std::filesystem::current_path() / line;
        continue;
      }
      ss << input_file.rdbuf();
    }
  }
  return ss.str();
}

std::vector<uchen::tools::tokens::Token> BuildTokenStream(
    const std::string& input, size_t apperances) {
  uchen::tools::tokens::TokenStore store;
  bool more = true;
  while (more) {
    std::map combined = uchen::tools::tokens::Combine(store.Tokenize(input));
    for (auto it = combined.begin(); it != combined.end();) {
      if (it->second < apperances) {
        it = combined.erase(it);
      } else {
        ++it;
      }
    }
    more = store.Update(combined);
  }
  return store.Tokenize(input);
}

std::map<std::string, size_t> BuildBagOfWords(
    const std::vector<std::string>& files, size_t apperances) {
  std::string input = ReadInputBuffer(files);
  LOG(INFO) << "Read " << input.size() << " bytes from input files";
  std::map<std::string, size_t> tokens;
  for (const auto& token : BuildTokenStream(input, apperances)) {
    if (!token.is_special() && token.name().size() > 1) {
      tokens[token.name()] += 1;
    }
  }
  return tokens;
}

std::map<std::string, size_t> CombineBags(const std::vector<std::string>& files,
                                          size_t apperances) {
  std::map<std::string, size_t> tokens;
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

}  // namespace

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
  std::map<std::string, size_t> tokens =
      inputs.empty()
          ? CombineBags(combine, absl::GetFlag(FLAGS_token_apperances))
          : BuildBagOfWords(inputs, absl::GetFlag(FLAGS_token_apperances));
  for (const auto& [token, count] : tokens) {
    output << token << "\t" << count << std::endl;
  }
  LOG(INFO) << "Wrote " << tokens.size() << " unique tokens to "
            << absl::GetFlag(FLAGS_output);
  return 0;
}