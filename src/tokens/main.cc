#include <iostream>

#include "tokens.h"
#include "tokens/tokens.h"

int main() {
  uchen::tools::tokens::TokenStore store;
  for (const auto& token : store.Tokenize("Hello")) {
    std::cout << token.name() << "\n";
  }
}