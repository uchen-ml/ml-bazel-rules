#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"

int main(int argc, char* argv[]) {
  absl::InitializeLog();
  auto paths = absl::ParseCommandLine(argc, argv);
  absl::SetProgramUsageMessage(
      absl::StrCat("Usage: ", paths[0], " <directory>"));
  LOG(INFO) << "Arguments: "
            << absl::StrJoin(std::span(argv, argc).subspan(1), " ");
}