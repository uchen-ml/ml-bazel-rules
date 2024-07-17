#ifndef __TOOLS_TOKENS_H

#define __TOOLS_TOKENS_H

#include <string_view>

namespace uchen::tools::tokens {

std::string Tokenize(std::string_view string);

}

#endif  // __TOOLS_TOKENS_H