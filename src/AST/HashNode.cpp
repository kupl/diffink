#include "AST/HashNode.h"

namespace diffink {

void HashNode::setMetadataRecursively() {
  auto TypeHash = xxh::xxhash3<BitMode>(&Type, sizeof(Type));
  if (isLeaf()) {
    ExactHash = xxh::xxhash3<BitMode>({TypeHash, xxh::xxhash3<BitMode>(Text)});
    StructuralHash = TypeHash;
    Height = 1;
    return;
  }

  std::vector<xxh::hash_t<BitMode>> StructuralHashList, ExactHashList;
  ExactHashList.reserve(Children.size());
  StructuralHashList.reserve(Children.size());

  for (auto Child : Children) {
    Child->setMetadataRecursively();
    ExactHashList.push_back(Child->ExactHash);
    StructuralHashList.push_back(Child->StructuralHash);
    if (Child->Height >= Height)
      Height = Child->Height + 1;
  }

  ExactHash =
      xxh::xxhash3<BitMode>({TypeHash, xxh::xxhash3<BitMode>(ExactHashList)});
  StructuralHash = xxh::xxhash3<BitMode>(
      {TypeHash, xxh::xxhash3<BitMode>(StructuralHashList)});
}

} // namespace diffink