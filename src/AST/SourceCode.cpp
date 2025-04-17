#include "AST/SourceCode.h"

namespace diffink {

void SourceCode::setByteMap() {
  ByteToPos.clear();
  decltype(ByteToPos)().swap(ByteToPos);
  ByteToPos.reserve(getContentSize());
  ByteToUTF8Pos.clear();
  decltype(ByteToUTF8Pos)().swap(ByteToUTF8Pos);
  ByteToUTF8Pos.reserve(getContentSize());

  std::string::size_type Row{0}, Col{0};
  std::string::size_type UTF8Row{0}, UTF8Col{0};
  auto CStr = getContent();
  auto NullOffset = Content.size();

  for (std::string::size_type i = 0; i != Content.size();) {
    ByteToPos.emplace_back(Row, Col);
    ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
    auto Byte = static_cast<unsigned char>(CStr[i]);

    if ((Byte & OneByteMask) == OneByteExpected) {
      if (CStr[i] == '\n') {
        ++Row;
        ++UTF8Row;
        Col = 0;
        UTF8Col = 0;
      } else {
        ++UTF8Col;
        ++Col;
      }
      ++i;
      continue;
    }

    if ((Byte & TwoByteMask) == TwoByteExpected) {
      if (i + 1 >= NullOffset)
        throw std::runtime_error("Invalid UTF-8 sequence");
      if ((static_cast<unsigned char>(CStr[i + 1]) & ContinuationMask) !=
          ContinuationExpected)
        throw std::runtime_error("Invalid UTF-8 sequence");
      i += 2;
      ByteToPos.emplace_back(Row, Col + 1);
      Col += 2;
      ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
      ++UTF8Col;
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
      i += 3;
      ByteToPos.emplace_back(Row, Col + 1);
      ByteToPos.emplace_back(Row, Col + 2);
      Col += 3;
      ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
      ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
      ++UTF8Col;
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
      i += 4;
      ByteToPos.emplace_back(Row, Col + 1);
      ByteToPos.emplace_back(Row, Col + 2);
      ByteToPos.emplace_back(Row, Col + 3);
      Col += 4;
      ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
      ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
      ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
      ++UTF8Col;
      continue;
    }
    throw std::runtime_error("Invalid UTF-8 sequence");
  }
  ByteToPos.emplace_back(Row, Col);
  ByteToUTF8Pos.emplace_back(UTF8Row, UTF8Col);
}

void SourceCode::read(const std::filesystem::path &FilePath) {
  std::ifstream File(FilePath, std::ios::binary);
  if (!File)
    throw std::runtime_error("Failed to open file: " + FilePath.string());
  Name = FilePath.filename().string();

  const auto FileSize = std::filesystem::file_size(FilePath);
  this->Content.clear();
  decltype(Content)().swap(Content);
  Content = std::string(FileSize, '\0');

  if (!File.read(Content.data(), FileSize))
    throw std::runtime_error("Failed to read file: " + FilePath.string());
  setByteMap();
}

void SourceCode::newContent(const std::string Name,
                            const std::string &Content) {
  this->Name = Name;
  this->Content.clear();
  decltype(this->Content)().swap(this->Content);
  this->Content = Content;
  setByteMap();
}

} // namespace diffink