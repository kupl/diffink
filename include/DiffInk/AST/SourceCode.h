#ifndef DIFFINK_AST_SOURCECODE_H
#define DIFFINK_AST_SOURCECODE_H

#include <stdexcept>
#include <string>
#include <tree_sitter/api.h>
#include <vector>

namespace diffink {

class SourceCode {
private:
  static constexpr unsigned char OneByteMask{0b10000000};
  static constexpr unsigned char OneByteExpected{0};
  static constexpr unsigned char ContinuationMask{0b11000000};
  static constexpr unsigned char ContinuationExpected{0b10000000};
  static constexpr unsigned char TwoByteMask{0b11100000};
  static constexpr unsigned char TwoByteExpected{0b11000000};
  static constexpr unsigned char ThreeByteMask{0b11110000};
  static constexpr unsigned char ThreeByteExpected{0b11100000};
  static constexpr unsigned char FourByteMask{0b11111000};
  static constexpr unsigned char FourByteExpected{0b11110000};

private:
  std::string Content;
  std::vector<TSPoint> ByteToPos;
  std::vector<TSPoint> ByteToUTF8Pos;

private:
  void setByteMap();

public:
  SourceCode(std::string &&Content);

  // void read(const std::filesystem::path &FilePath);

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

  std::string::size_type getCstringSize() const noexcept {
    return Content.size();
  }

  std::string::size_type getContentSize() const noexcept {
    return Content.size() + 1;
  }

  std::string getSubstring(std::string::size_type StartByte,
                           std::string::size_type EndByte) const noexcept {
    return std::string(getContent() + StartByte, getContent() + EndByte);
  }
};

} // namespace diffink

#endif // DIFFINK_AST_SOURCECODE_H