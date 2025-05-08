#ifndef FRONTEND_SCRIPTEXPORTER_H
#define FRONTEND_SCRIPTEXPORTER_H

#include "DiffInk/TreeDiff/TreeDiff.h"
#include <cmath>
#include <nlohmann/json.hpp>

class ScriptExporter {
private:
  diffink::ExtendedEditScript Script;

public:
  ScriptExporter(diffink::ExtendedEditAction &&Script) noexcept
      : Script{std::move(Script)} {}

  ScriptExporter(diffink::ExtendedEditScript &Script) noexcept
      : Script(Script) {}

  std::string exportAsString() const;

  std::pair<std::string, std::string>
  exportAsHTML(const diffink::SourceCode &OldSrc,
               const diffink::SourceCode &NewSrc) const;

  nlohmann::json exportAsJSON() const;
};

#endif // FRONTEND_SCRIPTEXPORTER_H