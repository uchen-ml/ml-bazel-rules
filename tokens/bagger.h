#ifndef TOKENS_BAGGER_H
#define TOKENS_BAGGER_H

#include <map>
#include <span>

#include "tokens/tokens.h"

namespace uchen::tools::tokens {

// Bagger lumps together tokens based on the configuration to make a word bag
// for the model training
class Bagger {
 public:
  std::map<std::pair<Token, Token>, int> Lump(std::span<const Token> view);
};
}  // namespace uchen::tools::tokens

#endif  // TOKENS_BAGGER_H