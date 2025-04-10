#ifndef PARSER_CODEFILE_H
#define PARSER_CODEFILE_H

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <tree_sitter/api.h>
#include <vector>

namespace diffink {

class CodeFile {
private:
  std::string Name;
  std::string Content;
  std::vector<TSPoint> ByteToPos;

public:
  CodeFile() = default;

  void read(const std::filesystem::path &FilePath);

  const std::string &getName() const { return Name; }

  const char *getContent() const { return Content.c_str(); }

  TSPoint operator[](std::string::size_type Pos) const {
    return ByteToPos[Pos];
  }

  std::string::size_type getContentSize() const { return Content.size(); }
};

} // namespace diffink

#endif // PARSER_CODEFILE_H