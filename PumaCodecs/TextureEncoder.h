#pragma once

#include "prtx/NamePreparator.h"
#include "prtx/Texture.h"

#include "prt/Callbacks.h"

#include <string>

namespace TextureEncoder {

enum class Format : uint8_t { AUTO, JPG, PNG, TIF };

std::wstring encode(const prtx::TexturePtr& tex, prt::SimpleOutputCallbacks* soh, prtx::NamePreparator& namePreparator,
                    const prtx::NamePreparator::NamespacePtr& namespaceFilenames,
                    const std::wstring& memTexFileNamePrefix, const Format& targetFormat = Format::AUTO);

} // namespace TextureEncoder
