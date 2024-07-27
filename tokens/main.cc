#include <iostream>
#include <span>

#include "absl/strings/str_join.h"

int main(int argc, char* argv[]) {
  std::span<char*> args(argv, argc);
  std::cout << absl::StrJoin(args, " ") << "\n";
}