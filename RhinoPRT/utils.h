#pragma once

#include "RhinoCallbacks.h"

#include "stdafx.h"

#include "prt/API.h"
#include "prt/FileOutputCallbacks.h"
#include "prt/LogHandler.h"

#include <memory>
#include <string>

struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

// forward declaration to break include loop
class GeneratedModel;

namespace pcu {

	std::wstring getDllLocation();

	template <typename C>
	C getDirSeparator();
	template <>
	char getDirSeparator();
	template <>
	wchar_t getDirSeparator();

	struct ShapeAttributes {
		std::wstring ruleFile;
		std::wstring startRule;
		std::wstring shapeName;
		int seed;

		ShapeAttributes(const std::wstring rulef = L"bin/rule.cgb", const std::wstring startRl = L"Default$Lot",
			const std::wstring shapeN = L"Lot", int sd = 555) :
			ruleFile(rulef), startRule(startRl), shapeName(shapeN), seed(sd) { }
	};

	struct EncoderOptions {
		bool emitReport = false;
		bool emitGeometry = true;
	};


	/**
	 * helpers for prt object management
	 */
	struct PRTDestroyer {
		void operator()(const prt::Object* p) const {
			if (p)
				p->destroy();
		}
	};

	using ObjectPtr = std::unique_ptr<const prt::Object, PRTDestroyer>;
	using CachePtr = std::unique_ptr<prt::CacheObject, PRTDestroyer>;
	using ResolveMapPtr = std::unique_ptr<const prt::ResolveMap, PRTDestroyer>;
	using InitialShapePtr = std::unique_ptr<const prt::InitialShape, PRTDestroyer>;
	using InitialShapeBuilderPtr = std::unique_ptr<prt::InitialShapeBuilder, PRTDestroyer>;
	using AttributeMapPtr = std::unique_ptr<const prt::AttributeMap, PRTDestroyer>;
	using AttributeMapBuilderPtr = std::unique_ptr<prt::AttributeMapBuilder, PRTDestroyer>;
	using FileOutputCallbacksPtr = std::unique_ptr<prt::FileOutputCallbacks, PRTDestroyer>;
	using ConsoleLogHandlerPtr = std::unique_ptr<prt::ConsoleLogHandler, PRTDestroyer>;
	using FileLogHandlerPtr = std::unique_ptr<prt::FileLogHandler, PRTDestroyer>;
	using RuleFileInfoPtr = std::unique_ptr<const prt::RuleFileInfo, PRTDestroyer>;
	using EncoderInfoPtr = std::unique_ptr<const prt::EncoderInfo, PRTDestroyer>;
	using DecoderInfoPtr = std::unique_ptr<const prt::DecoderInfo, PRTDestroyer>;
	using SimpleOutputCallbacksPtr = std::unique_ptr<prt::SimpleOutputCallbacks, PRTDestroyer>;
	using RhinoCallbacksPtr = std::unique_ptr<RhinoCallbacks>;

	AttributeMapPtr createAttributeMapForShape(const ShapeAttributes& attrs, prt::AttributeMapBuilder& bld);
	AttributeMapPtr createAttributeMapForEncoder(const EncoderOptions& attrs, prt::AttributeMapBuilder& bld);
	AttributeMapPtr createValidatedOptions(const std::wstring& encID, const AttributeMapPtr& unvalidatedOptions);

	const ON_Mesh getMeshFromGenModel(const GeneratedModel& model);


	/**
	 * String and URI helpers
	 */
	using URI = std::string;

	std::string toOSNarrowFromUTF16(const std::wstring& osWString);
	std::wstring toUTF16FromOSNarrow(const std::string& osString);
	std::wstring toUTF16FromUTF8(const std::string& utf8String);
	std::string toUTF8FromOSNarrow(const std::string& osString);
	std::string percentEncode(const std::string& utf8String);
	URI toFileURI(const std::string& p);
	URI toFileURI(const std::wstring& p);

	std::wstring filename(const std::wstring& path);

	/**
	 * Resolve map helpers
	 */
	constexpr const wchar_t* ANNOT_START_RULE = L"@StartRule";
	std::wstring getRuleFileEntry(const ResolveMapPtr& resolveMap);
	std::wstring detectStartRule(const RuleFileInfoPtr& ruleFileInfo);

} // namespace pcu