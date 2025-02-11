/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
 *
 * Copyright (c) 2021-2025 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TextureEncoder.h"

#include "prtx/Exception.h"
#include "prtx/ExtensionManager.h"
#include "prtx/PRTUtils.h"
#include "prtx/Texture.h"
#include "prtx/URI.h"

#include "prt/EncoderInfo.h"

#include <limits>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace TextureEncoder {

namespace IDs {

const std::wstring PNG = L"com.esri.prt.codecs.PNGEncoder";
const std::wstring JPG = L"com.esri.prt.codecs.JPGEncoder";
const std::wstring TIF = L"com.esri.prt.codecs.TIFFEncoder";
const std::wstring DXT = L"com.esri.prt.codecs.DxtEncoder";
const std::wstring RAW = L"com.esri.prt.codecs.RAWEncoder";

} // namespace IDs

// key names for texture encoder options provided by PRT
namespace OptionNames {

constexpr const wchar_t* NAME = L"textureName";
constexpr const wchar_t* FLIPH = L"textureFlipH";
constexpr const wchar_t* BASE64 = L"textureBase64";
constexpr const wchar_t* MAXDIM = L"textureMaxDimension";
constexpr const wchar_t* SCALING = L"textureScalingFactor";
constexpr const wchar_t* FIXED_DIMENSIONS = L"textureFixedDimensions";

constexpr const wchar_t* JPG_QUALITY = L"textureQuality";
constexpr const wchar_t* JPG_FORCE_JFIF = L"forceJFIFHeaders";
constexpr const wchar_t* PNG_COMPRESSION = L"textureCompression";

constexpr const wchar_t* EXISTING_FILES = L"existingFiles";
constexpr const wchar_t* EXISTING_FILES_OVERWRITE = L"OVERWRITE";
constexpr const wchar_t* EXISTING_FILES_SKIP = L"SKIP";

} // namespace OptionNames

std::wstring const& selectEncoderID(Format format) {
	switch (format) {
		case Format::JPG:
			return IDs::JPG;
		case Format::PNG:
			return IDs::PNG;
		case Format::TIF:
			return IDs::TIF;
		default:
			throw std::invalid_argument("unsupported format");
	}
}

std::wstring const getBestMatchingEncoder(const prtx::Texture& tex) {
	const prtx::URIPtr& uri = tex.getURI();

	std::wstring const extension = uri->getExtension();
	if (!extension.empty()) {
		std::vector<std::wstring> availableEncoders;
		prtx::ExtensionManager::instance().listEncoderIds(availableEncoders);

		std::wstring bestId;
		double bestMerit = std::numeric_limits<double>::lowest();
		for (const auto& encId : availableEncoders) {
			prtx::PRTUtils::EncoderInfoUPtr encInfo(prtx::ExtensionManager::instance().createEncoderInfo(encId));
			if (encInfo->getType() == prt::CT_TEXTURE) {
				const std::wstring_view extensions(encInfo->getExtensions());
				const bool hasCompatibleFileExtension = (extensions.find(extension + L';') != std::wstring_view::npos);
				if (hasCompatibleFileExtension && (encInfo->getMerit() > bestMerit)) {
					bestId = encId;
					bestMerit = encInfo->getMerit();
				}
			}
		}

		if (!bestId.empty())
			return bestId;
	}

	// fallback behavior
	if (extension == L".png")
		return IDs::PNG;
	else if (tex.getFormat() == prtx::Texture::RGB8 || tex.getFormat() == prtx::Texture::GREY8)
		return IDs::JPG;
	else
		return IDs::PNG;
}

std::wstring getExtensionForEncoder(std::wstring const& textureEncoderID, std::wstring const& currentExt) {
	const prtx::PRTUtils::EncoderInfoUPtr encoderInfo(
	        prtx::ExtensionManager::instance().createEncoderInfo(textureEncoderID));
	const std::wstring_view extensions(encoderInfo->getExtensions());
	const bool hasCompatibleFileExtension = (extensions.find(currentExt + L';') != std::wstring_view::npos);
	return hasCompatibleFileExtension ? currentExt : std::wstring(extensions.substr(0, extensions.find_first_of(L';')));
}

std::wstring replaceExtension(std::wstring const& texName, std::wstring const& extension) {
	const prtx::URIPtr nameUri = prtx::URIUtils::createFileURI(prtx::URIUtils::percentEncode(L"/" + texName));
	const prtx::URIPtr newExtensionUri = prtx::URIUtils::replaceExtension(nameUri, extension);
	return newExtensionUri->getPath();
}

std::wstring getBaseName(const prtx::URIPtr& texURI, const std::wstring& namePrefix) {
	const std::wstring queryBaseName = texURI->getQuery(prtx::URI::QUERY_TEXTURE_NAME);
	const std::wstring uriBaseName = queryBaseName.empty() ? texURI->getBaseName() : queryBaseName;

	if (texURI->getScheme() == prtx::URI::SCHEME_MEMORY && namePrefix.size() > 0)
		return namePrefix + L'_' + uriBaseName;

	return uriBaseName;
}

std::wstring constructNameForTexture(const prtx::TexturePtr& texture, const std::wstring& namePrefix) {
	if (!texture || !texture->isValid())
		throw prtx::StatusException(prt::STATUS_ILLEGAL_VALUE);

	prtx::URIPtr texURI = texture->getURI();
	std::wstring const baseName = getBaseName(texURI, namePrefix);
	std::wstring const extension = texURI->getExtension();
	return baseName + extension;
}

prtx::PRTUtils::AttributeMapUPtr getEncOpts(const std::wstring& encoderId, const std::wstring& filename,
                                            prt::SimpleOutputCallbacks::OpenMode openMode) {
	prtx::PRTUtils::AttributeMapBuilderPtr builder(prt::AttributeMapBuilder::create());
	builder->setString(OptionNames::NAME, filename.c_str());
	builder->setBool(OptionNames::FLIPH, true);
	auto const evExistingFiles = openMode == prt::SimpleOutputCallbacks::OPENMODE_ALWAYS
	                                     ? OptionNames::EXISTING_FILES_OVERWRITE
	                                     : OptionNames::EXISTING_FILES_SKIP;
	builder->setString(OptionNames::EXISTING_FILES, evExistingFiles);

	const prtx::PRTUtils::EncoderInfoPtr encInfo(prtx::ExtensionManager::instance().createEncoderInfo(encoderId));
	prtx::PRTUtils::AttributeMapPtr rawEncOpts{builder->createAttributeMap()};
	const prt::AttributeMap* validOpts = nullptr;
	encInfo->createValidatedOptionsAndStates(rawEncOpts.get(), &validOpts);
	return prtx::PRTUtils::AttributeMapUPtr(validOpts);
}

std::wstring encode(const prtx::TexturePtr& texture, prt::SimpleOutputCallbacks* soh,
                    prtx::NamePreparator& namePreparator, const prtx::NamePreparator::NamespacePtr& namespaceFilenames,
                    const std::wstring& memTexFileNamePrefix, const Format& targetFormat) {
	if (!texture || !texture->isValid())
		throw prtx::StatusException(prt::STATUS_ILLEGAL_VALUE);

	std::wstring textureEncoderID;
	if (targetFormat == Format::AUTO)
		textureEncoderID = getBestMatchingEncoder(*texture);
	else
		textureEncoderID = selectEncoderID(targetFormat);

	const std::wstring texName = constructNameForTexture(texture, memTexFileNamePrefix);
	const std::wstring extension = getExtensionForEncoder(textureEncoderID, texture->getURI()->getExtension());
	const std::wstring texNameWithExtension = replaceExtension(texName, extension);
	const std::wstring uniqueName = namePreparator.legalizedAndUniquified(
	        texNameWithExtension.substr(1), prtx::NamePreparator::ENTITY_FILE, namespaceFilenames);

	prtx::PRTUtils::AttributeMapUPtr encOpts =
	        getEncOpts(textureEncoderID, uniqueName, prt::SimpleOutputCallbacks::OpenMode::OPENMODE_ALWAYS);
	prtx::EncoderPtr texEnc = prtx::ExtensionManager::instance().createEncoder(textureEncoderID, encOpts.get(), soh);
	texEnc->encode({texture});

	prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
	wchar_t const* const validatedName = encOpts->getString(OptionNames::NAME, &status);
	if (status != prt::STATUS_OK)
		throw prtx::StatusException(status);
	if (validatedName == nullptr)
		throw prtx::StatusException(prt::STATUS_ENCODE_FAILED);

	return std::wstring(validatedName);
}

} // namespace TextureEncoder
