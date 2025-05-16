#ifndef DIFFINK_AST_SOURCECODE_H
#define DIFFINK_AST_SOURCECODE_H

#include <stdexcept>
#include <string>
#include <tree_sitter/api.h>
#include <vector>

namespace diffink {

class SourceCode {
private:
  std::string Content;
  std::vector<TSPoint> ByteToPos;
  std::vector<TSPoint> ByteToUTF8Pos;
  std::vector<uint32_t> Lines;

private:
  void setByteMap();

public:
  SourceCode(std::string &&Content);

  const char *getContent() const noexcept { return Content.c_str(); }

  // Equivalent to "getPosition"
  TSPoint operator[](std::string::size_type ByteOffset) const noexcept {
    return ByteToPos[ByteOffset];
  }

  TSPoint getPosition(std::string::size_type ByteOffset) const noexcept {
    return ByteToPos[ByteOffset];
  }

  TSPoint getUTF8Position(std::string::size_type ByteOffset) const noexcept {
    return ByteToUTF8Pos[ByteOffset];
  }

  const decltype(Lines) &getLines() const noexcept { return Lines; }

  std::string::size_type getSize() const noexcept { return Content.size(); }

  uint32_t getLOC() const noexcept {
    return ByteToPos.empty() ? 0 : ByteToPos.back().row + 1;
  }

  std::string getSubstring(std::string::size_type StartByte,
                           std::string::size_type EndByte) const noexcept {
    return std::string(getContent() + StartByte, getContent() + EndByte);
  }
};

} // namespace diffink

#endif // DIFFINK_AST_SOURCECODE_H