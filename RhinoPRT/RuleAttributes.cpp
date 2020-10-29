#include "RuleAttributes.h"

#include "Logger.h"

#include <algorithm>

namespace {

	std::wstring getNiceName(const std::wstring& attrName) {
		return pcu::removeImport(pcu::removeStyle(attrName));
	}

}

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

RangeAttributes AnnotationRange::getAnnotArguments() const { return { mMin, mMax, mStepSize, mRestricted }; }

/// Specific implementations of enum annotation constructors

template<>
AnnotationEnum<bool>::AnnotationEnum(const prt::Annotation* an) : AnnotationBase(AttributeAnnotation::ENUM, EnumAnnotationType::BOOL)
{
	mEnums.reserve(an->getNumArguments());

	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		mEnums.emplace_back(an->getArgument(argIdx)->getBool());
	}
}

template<>
AnnotationEnum<std::wstring>::AnnotationEnum(const prt::Annotation* an) : AnnotationBase(AttributeAnnotation::ENUM, EnumAnnotationType::STRING)
{
	mEnums.reserve(an->getNumArguments());

	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		mEnums.emplace_back(an->getArgument(argIdx)->getStr());
	}
}

template<>
AnnotationEnum<double>::AnnotationEnum(const prt::Annotation* an) : AnnotationBase(AttributeAnnotation::ENUM, EnumAnnotationType::DOUBLE)
{
	mEnums.reserve(an->getNumArguments());

	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
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

void addAnnotationObject(const wchar_t* annotName, const prt::Annotation* an, prt::AnnotationArgumentType attrType, std::vector<AnnotationUPtr>& annotVector)
{
	if (!std::wcscmp(annotName, ANNOT_COLOR) && annotCompatibleWithType(AttributeAnnotation::COLOR, attrType)) {
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::COLOR));
	}
	else if (!std::wcscmp(annotName, ANNOT_ENUM)) {
		if(attrType == prt::AAT_BOOL) annotVector.emplace_back(new AnnotationEnum<bool>(an));
		if (attrType == prt::AAT_FLOAT) annotVector.emplace_back(new AnnotationEnum<double>(an));
		if (attrType == prt::AAT_STR) annotVector.emplace_back(new AnnotationEnum<std::wstring>(an));
		if (attrType == prt::AAT_INT) annotVector.emplace_back(new AnnotationEnum<int>(an));
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::NOANNOT));
	}
	else if (!std::wcscmp(annotName, ANNOT_RANGE) && annotCompatibleWithType(AttributeAnnotation::RANGE, attrType)) {
		annotVector.emplace_back(new AnnotationRange(an));
	}
	else if (!std::wcscmp(annotName, ANNOT_DIR) && annotCompatibleWithType(AttributeAnnotation::DIR, attrType)) {
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::DIR));
	}
	else if(!std::wcscmp(annotName, ANNOT_FILE) && annotCompatibleWithType(AttributeAnnotation::FILE, attrType)) {
		annotVector.emplace_back(new AnnotationFile(an));
	}
	else {
		annotVector.emplace_back(new AnnotationBase(AttributeAnnotation::NOANNOT));
	}
}

void createRuleAttributes(const std::wstring& ruleFile, const prt::RuleFileInfo& ruleFileInfo, RuleAttributes& ra)
{
	std::wstring mainCgaRuleName = pcu::filename(ruleFile);
	size_t idxExtension = mainCgaRuleName.find(L".cgb");
	if (idxExtension != std::wstring::npos)
		mainCgaRuleName = mainCgaRuleName.substr(0, idxExtension);
	
	for (size_t i = 0; i < ruleFileInfo.getNumAttributes(); ++i) {
		const prt::RuleFileInfo::Entry* attr = ruleFileInfo.getAttribute(i);

		if (attr->getNumParameters() != 0) continue;

		// Skip attributes that are not default style.
		if (!pcu::isDefaultStyle(attr->getName())) continue;

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

		if (hidden) continue;

		ra.emplace_back(std::move(ruleAttr));
		if (DBG) LOG_DBG << ruleAttr;
	}

	// Group and order attributes.
	// Set a global order:
	// - First sort by group / group order
	// - Then by order in group
	std::sort(ra.begin(), ra.end(), [](const RuleAttributeUPtr& left, const RuleAttributeUPtr& right){
		if (left->groups.size() == 0) {
			if (right->groups.size() == 0)
			{
				// No groups for both attributes: sort by order if they are set, else sort by string compare.
				if(left->order != right->order) return left->order < right->order;
				return left->mNickname.compare(right->mNickname) < 0;
			}
			else {
				return true; // Attributes without group are placed first.
			}
		}
		else if (right->groups.size() == 0) {
			return false; // Attributes without group are placed first.
		}

		// support only first level groups for now.
		int group_cmpr = left->groups.front().compare(right->groups.front());
		if (group_cmpr == 0) 
		{
			// same group, sort by order if set.
			if (left->order != right->order) return left->order < right->order;
			return left->mNickname.compare(right->mNickname) < 0;
		}
		else {
			// different group, sort by groupOrder if they are set, else use string compare.
			if (left->groupOrder != right->groupOrder) return left->groupOrder < right->groupOrder;
			return left->groups.front().compare(right->groups.front()) < 0;
		}
	});
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
