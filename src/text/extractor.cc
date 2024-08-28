#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"

ABSL_FLAG(std::vector<std::string>, inputs, {}, "Input files");
ABSL_FLAG(std::vector<std::string>, outputs, {}, "Output files");
ABSL_FLAG(size_t, min_length, 1, "Minimum extracted text length");
ABSL_FLAG(size_t, max_length, 0, "Maximum extracted text length");
ABSL_FLAG(size_t, seed, 42, "Random seed");

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  auto paths = absl::ParseCommandLine(argc, argv);
  absl::SetProgramUsageMessage(
      absl::StrCat("Usage: ", paths[0], " <directory>"));
  std::vector inputs = absl::GetFlag(FLAGS_inputs);
  if (inputs.empty()) {
    LOG(ERROR) << "Input files are required";
    return 1;
  }
  std::vector outputs = absl::GetFlag(FLAGS_outputs);
  if (outputs.size() != inputs.size()) {
    LOG(ERROR) << "Number of input files must match number of output files ("
               << inputs.size() << " != " << outputs.size() << ")";
    return 1;
  }
  size_t min_length = absl::GetFlag(FLAGS_min_length);
  size_t max_length = absl::GetFlag(FLAGS_max_length);
  std::mt19937 gen{absl::GetFlag(FLAGS_seed)};
  std::uniform_int_distribution<size_t> distr(min_length, max_length);
  for (size_t i = 0; i < inputs.size(); ++i) {
    size_t file_size = std::filesystem::file_size(inputs[i]);
    if (file_size < max_length) {
      std::cerr << "File " << inputs[i] << " is shorter than max_length\n";
      return 1;
    }
    std::ifstream input{inputs[i]};
    if (!input) {
      std::cerr << "Failed to open " << inputs[i] << " for reading\n";
    }
    std::ofstream output{outputs[i]};
    if (!output) {
      std::cerr << "Failed to open " << outputs[i] << " for writing\n";
    }
    std::istreambuf_iterator<char> it{input};
    size_t sample_len = distr(gen);
    int start =
        std::uniform_int_distribution<size_t>{0, file_size - sample_len}(gen);
    std::vector<char> buffer(sample_len, 0);
    input.seekg(start);
    input.read(buffer.data(), buffer.size());
    output.write(buffer.data(), buffer.size());
  }
}