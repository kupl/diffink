#ifndef DIFFINK_FRONTEND_SCRIPTEXPORTER_H
#define DIFFINK_FRONTEND_SCRIPTEXPORTER_H

#include "DiffInk/TreeDiff/EditAction.h"
#include <nlohmann/json.hpp>

namespace diffink {

class ScriptExporter {
private:
  ExtendedEditScript Script;

public:
  ScriptExporter(ExtendedEditAction &&Script) noexcept
      : Script{std::move(Script)} {}

  ScriptExporter(ExtendedEditScript &Script) noexcept : Script(Script) {}

  std::string exportAsString() const;

  std::pair<std::string, std::string>
  exportAsHTML(const SourceCode &OldSrc, const SourceCode &NewSrc) const;

  std::string exportAsJSON() const;
};

} // namespace diffink

#endif // DIFFINK_FRONTEND_SCRIPTEXPORTER_H