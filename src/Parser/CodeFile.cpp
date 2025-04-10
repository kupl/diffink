#include "Parser/CodeFile.h"

namespace diffink {

void CodeFile::read(const std::filesystem::path &FilePath) {
  std::ifstream File(FilePath, std::ios::binary);
  if (!File)
    throw std::runtime_error("Failed to open file: " + FilePath.string());
  Name = FilePath.filename().string();

  const auto FileSize = std::filesystem::file_size(FilePath);
  Content = std::string(FileSize, '\0');
  if (!File.read(Content.data(), FileSize))
    throw std::runtime_error("Failed to read file: " + FilePath.string());

  auto RawContent = Content.c_str();
  ByteToPos.reserve(FileSize + 1);
  std::string::size_type Row{0}, Col{0};
  for (const auto Char : Content) {
    ByteToPos.emplace_back(Row, Col);
    if (Char == '\n') {
      ++Row;
      Col = 0;
    } else
      ++Col;
  }
  ByteToPos.emplace_back(Row, Col);
}

} // namespace diffink