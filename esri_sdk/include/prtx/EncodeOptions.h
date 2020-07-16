/*
  COPYRIGHT (c) 2012-2020 Esri R&D Center Zurich
  TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
  Unpublished material - all rights reserved under the
  Copyright Laws of the United States and applicable international
  laws, treaties, and conventions.

  For additional information, contact:
  Environmental Systems Research Institute, Inc.
  Attn: Contracts and Legal Services Department
  380 New York Street
  Redlands, California, 92373
  USA

  email: contracts@esri.com
*/

#ifndef PRTX_ENCODEOPTIONS_H_
#define PRTX_ENCODEOPTIONS_H_

#include "prtx.h"
#include "prtx/AnnotationBuilder.h"
#include "prtx/EncoderInfoBuilder.h"
#include "prtx/EncoderFactory.h"

#include <string>
#include <vector>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4251 4231 4275 4660)
#endif


namespace prtx {


/**
 * Helper class to map enum items to string values.
 */
class PRTX_EXPORTS_API StringEnum {
public:
	struct Item {
		int id;
		std::wstring value;
		std::wstring description;
	};

	struct PodItem {
		int id;
		const wchar_t* value;
		const wchar_t* description;
	};

public:
	StringEnum();

	template<size_t N>
	StringEnum(const Item (&items)[N])
	: mItems(&items[0], &items[N])
	  {
		init(0);
	  }

	template<size_t N>
	StringEnum(const PodItem (&items)[N])
	: mItems(N)
	  {
		init(&items[0]);
	  }

	template<size_t N>
	StringEnum(const StringEnum& base, const int (&itemsToKeep)[N]) {
		init(base.beginItems(), base.endItems(), itemsToKeep, N);
	}

	void addItem(int id, const wchar_t* value, const wchar_t* description);
	bool removeItem(int id);

	template<size_t N>
	void addItems(const StringEnum& base, const int (&itemsToKeep)[N]) {
		addItems(base.beginItems(), base.endItems(), itemsToKeep, N);
	}
	void removeAllItems();

	int getIdByValue(const std::wstring& value) const;
	int getIdByValue(const wchar_t* value) const;
	const std::wstring& getValueById(int id) const;
	const wchar_t* getCValueById(int id) const;
	const std::wstring& operator[](int id) const;

	typedef std::vector<Item>::const_iterator ItemConstIter;

	ItemConstIter beginItems() const {
		return mItems.begin();
	}

	ItemConstIter endItems() const {
		return mItems.end();
	}

private:
	void init(const PodItem* items);
	void init(ItemConstIter itemsBegin, ItemConstIter itemsEnd, const int* itemsToKeep, size_t numItemsToKeep);
	void addItems(ItemConstIter itemsBegin, ItemConstIter itemsEnd, const int* itemsToKeep, size_t numItemsToKeep);
	void updateIdEqualsIndex();

private:
	std::vector<Item> mItems;
	bool mIdEqualsIndex;
};


/**
 * @namespace prtx::Annotations
 * @brief Supported annotation types for encoder options or encoder-supported initial shape attributes.
 * 
 * Typically these annotations are used to programmatically construct a user interface from an EncoderInfo instance or
 * to make certain initial shape attributes mandatory, for example.
 *
 * The documentation uses "option", "option attribute", "initial shape attribute" synonymously.
 *
 * \sa prt::EncoderInfo
 * \sa prt::createEncoderInfo
 */
namespace Annotations {


const std::wstring ORDER		= L"@Order";		///< Defines a semantic ordering of the option (one double parameter).
const std::wstring LABEL		= L"@Label";		///< Defines a (UI) label for the option (one string parameter).
const std::wstring GROUP		= L"@Group";		///< Defines a (UI) group for the option (one string parameter, one double parameter).
const std::wstring DESCRIPTION	= L"@Description";	///< Adds a description for the option (e.g. for tooltips) (one string parameter).
const std::wstring RANGE		= L"@Range";		///< Defines a value range for the option (two double parameters).
const std::wstring POINT2D		= L"@Point2D";		///< Declares an option to be interpreted as a 2D point. Only for array options.
const std::wstring POINT3D		= L"@Point3D";		///< Declares an option to be interpreted as a 3D point. Only for array options.
const std::wstring PERLAYER		= L"@PerLayer";		///< Assigns each value of an array option to a layer. Only for array options.
const std::wstring HIDDEN		= L"@Hidden";		///< Hides the option in the UI.
const std::wstring MANDATORY	= L"@Mandatory";	///< Makes the initial shape attribute mandatory.


} // namespace Annotations


/**
 * Helper class to simplify the annotation of encoder options.
 * Note: the default options need to be set on the EncoderInfoBuilder before this
 * class is used. Trying to annotate an non-existing option will throw an exception.
 *
 * A typical usage example:
 * \code{.cpp}
	prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());
	amb->setInt(L"omg", 666);

	prtx::EncoderInfoBuilder eib;
	eib.setDefaultOptions(amb->createAttributeMap());

	prtx::EncodeOptionsAnnotator eoa(eib);
	eoa.option(L"omg").setLabel(L"Oh My God").setDescription(L"An incredible blasphemous option.").flagAsHidden();

	prtx::PRTUtils::EncoderInfoPtr info(eib.createInfo());
	// ... now use info to create encoder factory ...
 * \endcode
 *
 * \sa prtx::EncoderInfoBuilder
 * \sa prt::AttributeMapBuilder
 */
class PRTX_EXPORTS_API EncodeOptionsAnnotator {
public:
	class OptionAnnotator;

private:
	template<typename Opt>
	class AnnotationAdder;

public:
	/**
	 * Creates an EncodeOptionsAnnotator instance which operates on infoBuilder.
	 *
	 * \sa prtx::EncoderInfoBuilder
	 */
	EncodeOptionsAnnotator(EncoderInfoBuilder& infoBuilder);
	EncodeOptionsAnnotator(const EncodeOptionsAnnotator&) = delete;
	EncodeOptionsAnnotator& operator=(const EncodeOptionsAnnotator&) = delete;

	/**
	 * Access to member function chaining helper class for one option.`
	 */
	OptionAnnotator option(const std::wstring& optionKey);

	/**
	 * Adds an Annotations::LABEL annotation.
	 */
	void setLabel(const std::wstring& optionKey, const std::wstring& label);

	/**
	 * Adds an Annotations::DESCRIPTION annotation.
	 */
	void setDescription(const std::wstring& optionKey, const std::wstring& desc);

	/**
	 * Adds an Annotations::ORDER annotation.
	 */
	void setOrder(const std::wstring& optionKey, const double& order);

	/**
	 * Adds an Annotations::GROUP annotation.
	 */
	void setGroup(const std::wstring& optionKey, const std::wstring& groupName, const double& order);

	/**
	 * Adds an Annotations::RANGE annotation with double values (= numeric range).
	 */
	void setRange(const std::wstring& optionKey, double min, double max);

	/**
	 * Adds an Annotations::RANGE annotation with string values (= enumeration).
	 */
	void setEnumerants(const std::wstring& optionKey, const StringEnum& items);

	/**
	 * Adds an Annotations::POINT2D annotation.
	 */
	void flagAsPoint2D(const std::wstring& optionKey);

	/**
	 * Adds an Annotations::POINT3D annotation.
	 */
	void flagAsPoint3D(const std::wstring& optionKey);

	/**
	 * Adds an Annotations::PERLAYER annotation with a string default value.
	 */
	void flagAsPerLayer(const std::wstring& optionKey, const std::wstring& layerDefault);

	/**
	 * Adds an Annotations::PERLAYER annotation with a double default value.
	 */
	void flagAsPerLayer(const std::wstring& optionKey, const double& layerDefault);

	/**
	 * Adds an Annotations::PERLAYER annotation with bool default value.
	 */
	void flagAsPerLayer(const std::wstring& optionKey, const bool& layerDefault);

	/**
	 * Adds an Annotations::PERLAYER annotation with a in32_t default value.
	 */
	void flagAsPerLayer(const std::wstring& optionKey, const int32_t& layerDefault);

	/**
	 * Adds an Annotations::HIDDEN annotation.
	 */
	void flagAsHidden(const std::wstring& optionKey);

	/**
	 * Adds an Annotations::MANDATORY annotation.
	 */
	void flagAsMandatory(const std::wstring& optionKey);

public:

	/**
	 * Helper class to set multiple annotations on one option by member function chaining.
	 */
	class PRTX_EXPORTS_API OptionAnnotator {
	public:
		OptionAnnotator(EncodeOptionsAnnotator& annotator, const std::wstring& optionKey)
		: mAnnotator(annotator), mOptionKey(optionKey) {}

		OptionAnnotator(const OptionAnnotator& rhs) : mAnnotator(rhs.mAnnotator), mOptionKey(rhs.mOptionKey) { }

		OptionAnnotator& operator=(const OptionAnnotator&) = delete;

	public:
		OptionAnnotator& setLabel(const std::wstring& label) {
			mAnnotator.setLabel(mOptionKey, label);
			return *this;
		}

		OptionAnnotator& setDescription(const std::wstring& desc) {
			mAnnotator.setDescription(mOptionKey, desc);
			return *this;
		}

		OptionAnnotator& setOrder(const double& order) {
			mAnnotator.setOrder(mOptionKey, order);
			return *this;
		}

		OptionAnnotator& setGroup(const std::wstring& name, const double& order) {
			mAnnotator.setGroup(mOptionKey, name, order);
			return *this;
		}

		OptionAnnotator& flagAsPoint2D() {
			mAnnotator.flagAsPoint2D(mOptionKey);
			return *this;
		}

		OptionAnnotator& flagAsPoint3D() {
			mAnnotator.flagAsPoint3D(mOptionKey);
			return *this;
		}

		OptionAnnotator& flagAsHidden() {
			mAnnotator.flagAsHidden(mOptionKey);
			return *this;
		}

		OptionAnnotator& flagAsMandatory() {
			mAnnotator.flagAsMandatory(mOptionKey);
			return *this;
		}

		template<typename T> OptionAnnotator& flagAsPerLayer(const T& layerDefault) {
			mAnnotator.flagAsPerLayer(mOptionKey, layerDefault);
			return *this;
		}

		OptionAnnotator& setRange(double min, double max) {
			mAnnotator.setRange(mOptionKey, min, max);
			return *this;
		}

		OptionAnnotator& setEnumerants(const StringEnum& items) {
			mAnnotator.setEnumerants(mOptionKey, items);
			return *this;
		}

	private:
		EncodeOptionsAnnotator& mAnnotator;
		const std::wstring& mOptionKey;

		//friend class EncodeOptionsAnnotator;
	};

protected:
	EncoderInfoBuilder& mInfoBuilder;
	AnnotationBuilder mAnnotationBuilder;
};


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_ENCODEOPTIONS_H_ */
