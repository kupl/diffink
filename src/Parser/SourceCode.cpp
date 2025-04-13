#include "Parser/SourceCode.h"

namespace diffink {

void SourceCode::setByteToPos() {
  decltype(ByteToPos)().swap(ByteToPos);
  ByteToPos.reserve(getContentSize());
  std::string::size_type Row{0}, Col{0};
  auto CStr = getContent();
  auto NullOffset = Content.size();

  for (std::string::size_type i = 0; i != Content.size(); ++i) {
    ByteToPos.emplace_back(Row, Col);
    auto Byte = static_cast<unsigned char>(CStr[i]);

    if ((Byte & OneByteMask) == OneByteExpected) {
      if (CStr[i] == '\n') {
        ++Row;
        Col = 0;
      } else
        ++Col;
      continue;
    }

    if ((Byte & TwoByteMask) == TwoByteExpected) {
      if (i + 1 >= NullOffset)
        throw std::runtime_error("Invalid UTF-8 sequence");
      if ((static_cast<unsigned char>(CStr[i + 1]) & ContinuationMask) !=
          ContinuationExpected)
        throw std::runtime_error("Invalid UTF-8 sequence");
      ++i;
      ByteToPos.emplace_back(Row, Col);
      ++Col;
      continue;
    }

    if ((Byte & ThreeByteMask) == ThreeByteExpected) {
      if (i + 2 >= NullOffset)
        throw std::runtime_error("Invalid UTF-8 sequence");
      if ((static_cast<unsigned char>(CStr[i + 1]) & ContinuationMask) !=
              ContinuationExpected ||
          (static_cast<unsigned char>(CStr[i + 2]) & ContinuationMask) !=
              ContinuationExpected)
        throw std::runtime_error("Invalid UTF-8 sequence");
      i += 2;
      ByteToPos.emplace_back(Row, Col);
      ByteToPos.emplace_back(Row, Col);
      ++Col;
      continue;
    }

    if ((Byte & FourByteMask) == FourByteExpected) {
      if (i + 3 >= NullOffset)
        throw std::runtime_error("Invalid UTF-8 sequence");
      if ((static_cast<unsigned char>(CStr[i + 1]) & ContinuationMask) !=
              ContinuationExpected ||
          (static_cast<unsigned char>(CStr[i + 2]) & ContinuationMask) !=
              ContinuationExpected ||
          (static_cast<unsigned char>(CStr[i + 3]) & ContinuationMask) !=
              ContinuationExpected)
        throw std::runtime_error("Invalid UTF-8 sequence");
      i += 3;
      ByteToPos.emplace_back(Row, Col);
      ByteToPos.emplace_back(Row, Col);
      ByteToPos.emplace_back(Row, Col);
      ++Col;
      continue;
    }
    throw std::runtime_error("Invalid UTF-8 sequence");
  }
  ByteToPos.emplace_back(Row, Col);
}

void SourceCode::read(const std::filesystem::path &FilePath) {
  std::ifstream File(FilePath, std::ios::binary);
  if (!File)
    throw std::runtime_error("Failed to open file: " + FilePath.string());
  Name = FilePath.filename().string();

  const auto FileSize = std::filesystem::file_size(FilePath);
  decltype(Content)().swap(Content);
  Content = std::string(FileSize, '\0');

  if (!File.read(Content.data(), FileSize))
    throw std::runtime_error("Failed to read file: " + FilePath.string());
  setByteToPos();
}

void SourceCode::newContent(const std::string Name,
                            const std::string &Content) {
  this->Name = Name;
  decltype(this->Content)().swap(this->Content);
  this->Content = Content;
  setByteToPos();
}

std::string SourceCode::getSubstring(uint32_t StartByte,
                                     uint32_t EndByte) const noexcept {
  auto CStr = getContent();
  return std::string(CStr + StartByte, CStr + EndByte);
}

} // namespace diffink