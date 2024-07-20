#include <iostream>

#include "tokens.h"
#include "tokens/tokens.h"

int main() {
  uchen::tools::tokens::TokenStore store;
  for (const auto& token : uchen::tools::tokens::Tokenize(store, "Hello")) {
    std::cout << token.string() << "\n";
  }
}