#ifndef UCHEN_TOOLS_SRC_BOW_EXTRACT_WORDS_H
#define UCHEN_TOOLS_SRC_BOW_EXTRACT_WORDS_H

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace uchen::tools::text {

std::unordered_map<std::string, uint32_t> ExtractWords(std::string_view stream);

}  // namespace uchen::tools::text

#endif  // UCHEN_TOOLS_SRC_BOW_EXTRACT_WORDS_H