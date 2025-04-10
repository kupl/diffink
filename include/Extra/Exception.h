#ifndef TOOL_EXCEPTION_H
#define TOOL_EXCEPTION_H

#include <exception>
#include <string>

namespace modc {

class Exception : public std::exception {
private:
  std::string Msg;

public:
  Exception(const std::string &Class, const std::string &Function,
            const std::string &Problem) {
    Msg.append(Class)
        .append(":")
        .append(Function)
        .append(": ")
        .append(Problem)
        .push_back('\n');
  }

  const char *what() const noexcept override { return Msg.c_str(); }
};

} // namespace modc

#endif // TOOL_EXCEPTION_H