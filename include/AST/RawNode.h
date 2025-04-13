#ifndef AST_RAWNODE_H
#define AST_RAWNODE_H

#include <string>
#include <tree_sitter/api.h>

namespace diffink {

struct RawNode {
  TSSymbol Symbol;
  std::string Type;
  std::string Text;
  uint32_t StartByte;
  uint32_t EndByte;
};

} // namespace diffink

#endif // AST_RAWNODE_H