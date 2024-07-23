#include "tokens/bagger.h"

#include <map>
#include <utility>

namespace uchen::tools::tokens {

//
// Bagger
//

std::map<std::pair<Token, Token>, int> Bagger::Lump(
    std::span<const Token> stream) {
  if (stream.empty()) {
    return {};
  }
  return {};
}

}  // namespace uchen::tools::tokens