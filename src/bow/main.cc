#include <iostream>
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
  // std::vector<
  uchen::tools::tokens::TokenStore store;
  for (const auto& token : store.Tokenize("Hello")) {
    std::cout << token.name() << "\n";
  }
  return 1;
}