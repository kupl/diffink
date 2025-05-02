#ifndef DIFFINK_FRONTEND_COMMANDLINEINTERFACE_H
#define DIFFINK_FRONTEND_COMMANDLINEINTERFACE_H

#include "DiffInk/Api.h"
#include <argparse.hpp>

namespace diffink::frontend {

class CommandLineInterface {
private:
  argparse::ArgumentParser ArgParser;

private:
  void setArguments();

public:
  CommandLineInterface(int argc, char *argv[]);

  CommandLineInterface(const CommandLineInterface &) = delete;

  void run();
};

} // namespace diffink::frontend

#endif // DIFFINK_FRONTEND_COMMANDLINEINTERFACE_H