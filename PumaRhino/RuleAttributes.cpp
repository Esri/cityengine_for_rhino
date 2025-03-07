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

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "RuleAttributes.h"
#include "Logger.h"

#include <algorithm>

namespace {

enum class CompareResult { SKIP = 0, LESS_THAN, GREATER_THAN };

std::wstring getNiceName(const std::wstring& attrName) {
	return pcu::removeStyle(attrName);
}

CompareResult compareImport(const std::wstring& left, const std::wstring& right) {
	const auto& leftImport = pcu::getImportPrefix(left);
	const auto& rightImport = pcu::getImportPrefix(right);

	if (leftImport.empty()) {
		if (!rightImport.empty())
			return CompareResult::LESS_THAN;
	}
	else if (rightImport.empty()) {
		return CompareResult::GREATER_THAN;
	}
	else if (leftImport.compare(rightImport) == 0) {
		// same import, compare next import level if any
		return compareImport(pcu::removeImport(left), pcu::removeImport(right));
	}

	return leftImport.compare(rightImport) < 0 ? CompareResult::LESS_THAN : CompareResult::GREATER_THAN;
}

bool compareByOrder(const RuleAttributeUPtr& left, const RuleAttributeUPtr& right) {
	if (left->order != right->order)
		return left->order < right->order;
	return left->mNickname.compare(right->mNickname) < 0;
}

bool compareRuleAttributes(const RuleAttributeUPtr& left, const RuleAttributeUPtr& right) {
	CompareResult result = compareImport(left->mFullName, right->mFullName);
	if (result != CompareResult::SKIP)
		return result == CompareResult::LESS_THAN;

	if (left->groups.size() == 0) {
		if (right->groups.size() == 0) {
			// No groups for both attributes: sort by order if they are set, else sort by string compare.
			return compareByOrder(left, right);
		}
		else {
			return true; // Attributes without group are placed first.
		}
	}
	else if (right->groups.size() == 0) {
		return false; // Attributes without group are placed first.
	}

	for (int i = 0; i < std::min(left->groups.size(), right->groups.size()); ++i) {
		int group_cmpr = left->groups[i].compare(right->groups[i]);
		if (group_cmpr != 0) {
			// different group, sort by groupOrder if they are set, else use string compare.
			if (left->groupOrder != right->groupOrder)
				return left->groupOrder < right->groupOrder;
			return group_cmpr < 0;
		}
		// same group, sort by subgroup
	}

	// Both have same groups. If one has more groups than the other, it is placed after.
	if (left->groups.size() != right->groups.size())
		return left->groups.size() < right->groups.size();

	// Both have exact same group, sort by order or string compare.
	return compareByOrder(left, right);
}

} // namespace

AnnotationRange::AnnotationRange(const prt::Annotation* an) : AnnotationBase(AttributeAnnotation::RANGE), mStepSize(0) {

	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		const prt::AnnotationArgument* arg = an->getArgument(argIdx);
		const wchar_t* key = arg->getKey();
		if (std::wcscmp(key, MIN_KEY) == 0) {
			mMin = arg->getFloat();
		}
		else if (std::wcscmp(key, MAX_KEY) == 0) {
			mMax = arg->getFloat();
		}
		else if (std::wcscmp(key, STEP_KEY) == 0) {
			mStepSize = arg->getFloat();
		}
		else if (std::wcscmp(key, RESTRICTED_KEY) == 0) {
			mRestricted = arg->getBool();
		}
	}
}

RangeAttributes AnnotationRange::getAnnotArguments() const {
	return {mMin, mMax, mStepSize, mRestricted};
}

/// Specific implementations of enum annotation constructors

template <>
AnnotationEnum<bool>::AnnotationEnum(const prt::Annotation* an)
    : AnnotationBase(AttributeAnnotation::ENUM, EnumAnnotationType::BOOL) {
	mEnums.reserve(an->getNumArguments());

	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		const prt::AnnotationArgument* argument = an->getArgument(argIdx);
		if (std::wcscmp(argument->getKey(), RESTRICTED_KEY) == 0)
			mRestricted = argument->getBool();
		else
			mEnums.emplace_back(an->getArgument(argIdx)->getBool());
	}
}

template <>
AnnotationEnum<std::wstring>::AnnotationEnum(const prt::Annotation* an)
    : AnnotationBase(AttributeAnnotation::ENUM, EnumAnnotationType::STRING) {
	mEnums.reserve(an->getNumArguments());

	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		const prt::AnnotationArgument* argument = an->getArgument(argIdx);
		if (std::wcscmp(argument->getKey(), RESTRICTED_KEY) == 0)
			mRestricted = argument->getBool();
		else
			mEnums.emplace_back(an->getArgument(argIdx)->getStr());
	}
}

template <>
AnnotationEnum<double>::AnnotationEnum(const prt::Annotation* an)
    : AnnotationBase(AttributeAnnotation::ENUM, EnumAnnotationType::DOUBLE) {
	mEnums.reserve(an->getNumArguments());

	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		const prt::AnnotationArgument* argument = an->getArgument(argIdx);
		if (std::wcscmp(argument->getKey(), RESTRICTED_KEY) == 0)
			mRestricted = argument->getBool();
		else
			mEnums.emplace_back(an->getArgument(argIdx)->getFloat());
	}
}

/// =========================================================== ///

bool annotCompatibleWithType(AttributeAnnotation annot, prt::AnnotationArgumentType type) {
	switch (type) {
		case prt::AnnotationArgumentType::AAT_BOOL:
			return annot == AttributeAnnotation::ENUM;
		case prt::AnnotationArgumentType::AAT_INT:
		case prt::AnnotationArgumentType::AAT_FLOAT:
			return annot == AttributeAnnotation::ENUM || annot == AttributeAnnotation::RANGE;
		case prt::AnnotationArgumentType::AAT_STR:
			return annot != AttributeAnnotation::RANGE;
		default:
			return false;
	}
}

void addAnnotationObject(const wchar_t* annotName, const prt::Annotation* an, prt::AnnotationArgumentType attrType,
                         std::vector<AnnotationUPtr>& annotVector) {
	if (!std::wcscmp(annotName, ANNOT_COLOR) && annotCompatibleWithType(AttributeAnnotation::COLOR, attrType)) {
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::COLOR));
	}
	else if (!std::wcscmp(annotName, ANNOT_ENUM)) {
		if (attrType == prt::AAT_BOOL)
			annotVector.emplace_back(new AnnotationEnum<bool>(an));
		if (attrType == prt::AAT_FLOAT)
			annotVector.emplace_back(new AnnotationEnum<double>(an));
		if (attrType == prt::AAT_STR)
			annotVector.emplace_back(new AnnotationEnum<std::wstring>(an));
		if (attrType == prt::AAT_INT)
			annotVector.emplace_back(new AnnotationEnum<int>(an));
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::NOANNOT));
	}
	else if (!std::wcscmp(annotName, ANNOT_RANGE) && annotCompatibleWithType(AttributeAnnotation::RANGE, attrType)) {
		annotVector.emplace_back(new AnnotationRange(an));
	}
	else if (!std::wcscmp(annotName, ANNOT_DIR) && annotCompatibleWithType(AttributeAnnotation::DIR, attrType)) {
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::DIR));
	}
	else if (!std::wcscmp(annotName, ANNOT_FILE) && annotCompatibleWithType(AttributeAnnotation::FILE, attrType)) {
		annotVector.emplace_back(new AnnotationFile(an));
	}
	else {
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::NOANNOT));
	}
}

void createRuleAttributes(const std::wstring& ruleFile, const prt::RuleFileInfo& ruleFileInfo, RuleAttributes& ra) {
	std::wstring mainCgaRuleName = pcu::filename(ruleFile);
	size_t idxExtension = mainCgaRuleName.find(L".cgb");
	if (idxExtension != std::wstring::npos)
		mainCgaRuleName = mainCgaRuleName.substr(0, idxExtension);

	for (size_t i = 0; i < ruleFileInfo.getNumAttributes(); ++i) {
		const prt::RuleFileInfo::Entry* attr = ruleFileInfo.getAttribute(i);

		if (attr->getNumParameters() != 0)
			continue;

		// Skip attributes that are not default style.
		if (!pcu::isDefaultStyle(attr->getName()))
			continue;

		RuleAttributeUPtr ruleAttr{new RuleAttribute()};
		ruleAttr->mFullName = attr->getName();
		ruleAttr->mRuleFile = mainCgaRuleName;
		ruleAttr->mNickname = getNiceName(ruleAttr->mFullName);
		ruleAttr->mType = attr->getReturnType();

		// process prt::Annotation
		bool hidden = false;

		for (size_t a = 0; a < attr->getNumAnnotations(); ++a) {
			const prt::Annotation* an = attr->getAnnotation(a);
			const wchar_t* anName = an->getName();

			if (!(std::wcscmp(anName, ANNOT_HIDDEN))) {
				hidden = true;
			}
			else if (!std::wcscmp(anName, ANNOT_ORDER)) {
				if (an->getNumArguments() >= 1 && an->getArgument(0)->getType() == prt::AAT_FLOAT) {
					ruleAttr->order = static_cast<int>(an->getArgument(0)->getFloat());
				}
			}
			else if (!std::wcscmp(anName, ANNOT_GROUP)) {
				for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
					if (an->getArgument(argIdx)->getType() == prt::AAT_STR) {
						ruleAttr->groups.push_back(an->getArgument(argIdx)->getStr());
					}
					else if (argIdx == an->getNumArguments() - 1 &&
					         an->getArgument(argIdx)->getType() == prt::AAT_FLOAT) {
						ruleAttr->groupOrder = static_cast<int>(an->getArgument(argIdx)->getFloat());
					}
				}
			}
			else {
				addAnnotationObject(anName, an, attr->getReturnType(), ruleAttr->mAnnotations);
			}
		}

		if (hidden)
			continue;

		ra.emplace_back(std::move(ruleAttr));
	}

	// Group and order attributes.
	// Order by descending priorities:
	// - Import prefix
	// - First sort by group / group order
	// - Then by order in group
	// - Alphanumerical in case no annotation
	std::sort(ra.begin(), ra.end(), compareRuleAttributes);
}

std::wostream& operator<<(std::wostream& ostr, const RuleAttribute& ap) {
	ostr << L"RuleAttribute '" << ap.mFullName << L"': ruleFile = '" << ap.mRuleFile << L"'" << std::endl;
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const RuleAttribute& ap) {
	std::wostringstream wostr;
	wostr << ap;
	ostr << pcu::toOSNarrowFromUTF16(wostr.str());
	return ostr;
}
