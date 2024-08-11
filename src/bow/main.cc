#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"

#include "src/bow/tokens.h"

ABSL_FLAG(std::vector<std::string>, inputlists, {}, "List of input files");
ABSL_FLAG(std::string, output, {}, "Output file");

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  absl::SetProgramUsageMessage(absl::StrFormat(
      "Usage: %s --inputlists <input_list_files> --output <output_file>",
      argv[0]));
  absl::ParseCommandLine(argc, argv);
  LOG(INFO) << absl::StrJoin(std::span(argv, argc), " ");
  LOG(INFO) << "Current working directory: " << std::filesystem::current_path();
  std::stringstream ss;
  for (std::string_view file : absl::GetFlag(FLAGS_inputlists)) {
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
  std::string input = ss.str();
  ss = {};
  LOG(INFO) << "Read " << input.size() << " bytes from input files";
  if (input.empty()) {
    LOG(ERROR) << "No input data";
    return 1;
  }
  uchen::tools::tokens::TokenStore store;
  for (const auto& token : store.Tokenize(input)) {
    std::cout << token.name() << "\n";
  }
  return 1;
}