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

#ifndef PRT_H_
#define PRT_H_

#ifdef _WIN32
#	ifdef PRT_EXPORTS
#		define PRT_EXPORTS_API __declspec(dllexport)
#	else
#		define PRT_EXPORTS_API __declspec(dllimport)
#	endif
#else
#	define PRT_EXPORTS_API __attribute__ ((visibility ("default")))
#endif


/**
 * \mainpage ArcGIS Procedural Runtime
 *
 * The CityEngine SDK enables you as a 3rd party developer to extend CityEngine with additional import and export formats and storage backends beyond simple files. Moreover, you can integrate the procedural runtime in your own client applications taking full advantage of the procedural core without running CityEngine or ArcGIS.
 *
 * The main documentation resources are:
 * - PRT Documentation
 *   - <a href="../arcgis_prt_whitepaper.pdf">PRT white paper</a>
 *   - <a href="../arcgis_prt_architecture.pdf">PRT architecture with examples</a>
 *   - \ref AppNotes
 * - PRT API Reference
 *   - <a href="namespaceprt.html#func-members">PRT Functions </a>
 *   - <a href="namespaceprt.html#nested-classes">PRT Classes </a>
 * - PRT SPI (PRTX) Reference
 *   - <a href="namespaceprtx.html#nested-classes">PRTX Classes </a>
 * - Related Documentation
 *   - <a href="cgaref/cgareference/cgaindex.html">CGA Reference</a>
 *   - <a href="esri_prt_codecs.html">Built-In Codecs Reference</a>
 *
 * \page AppNotes Application Notes
 *
 * ## Application Note 1: Instancing
 *
 * In general, the term "instancing" refers to the re-use of data - usually geometry - by using references instead of copies. During model generation in PRT instancing can happen in two ways:
 * -# Asset Instancing: if an asset is only inserted (@cgaref{op_i.html,insert()}) and not modified by CGA (e.g. @cgaref{attr_material.html,material properties}) then the corresponding prtx::Geometry object will carry its original URI through into the encoder and can be re-used accordingly by the encoder.
 * -# Geometry/Material Instancing: if the geometry of an asset is used multiple times and its materials are modified in the rules then the encoder author can make use of the EncodePreparator instance() flag. If this flag is set to true, the EncodePreparator will re-use the geometry objects (same reference/pointer) and return a separate list of finalized materials (\ref prtx::EncodePreparator::FinalizedInstance::getMaterials()). If the intended target format supports this type of instancing the encoder can directly translate this one-geometry-to-multiple-materials mapping into the target format.
 *
 * ## Application Note 2: Exporting ordered meta data per initial shape
 *
 * All <a href="esri_prt_codecs.html">built-in encoders</a> of the CE SDK which support writing
 * initial shape attributes understand the reserved initial shape attribute keys "/enc/metaData/"
 * and "/enc/metaDataOrder". If any attributes are prefixed with "/enc/metaData" the encoder will
 * attempt to output them accordingly, e.g. the WebScene encoder will write them out as "object attributes"
 * into the 3ws files and they can be inspected in the CityEngine WebViewer. The order of those attributes
 * can be specified by populating the string array in the attribute "/enc/metaDataOrder" accordingly.
 *
 * Here is an example how to get the attributes "myItem1, myItem2, myItem3" to output in the order "myItem1, myItem3, myItem2":
@verbatim
prt::AttributeMapBuilder* builder = prt::AttributeMapBuilder::create();
builder->setInt(L"/enc/metaData/myItem1", 10);
builder->setInt(L"/enc/metaData/myItem2", 20);
builder->setInt(L"/enc/metaData/myItem3", 30);
builder->setInt(L"myItemForCGA", 99);

const wchar_t* order[3] = { L"/enc/metaData/myItem1", L"/enc/metaData/myItem3", L"/enc/metaData/myItem2" };
builder->setStringArray(L"/enc/metaDataOrder", order, 3);

const prt::AttributeMap* attrMap = builder->createAttributeMap();
builder->destroy();

prt::InitialShapeBuilder* isBuilder = prt::InitialShapeBuilder::create();
isBuilder->setAttributes(..., attrMap, ...);
const prt::InitialShape* initialShape = isBuilder->createInitialShapeAndReset();
isBuilder->destroy();

prt::generate(...); // -> encoders will emit "myItem1, myItem3, myItem2"

initialShape->destroy();
attrMap->destroy();
@endverbatim
 *
 * \sa prt::InitialShapeBuilder::setAttributes
 *
 * ## Application Note 3: Requirements for CityEngine custom exporters
 *
 * A PRT encoder must meet a few requirements to be successfully loaded by CityEngine as a custom exporter:
 * -# CityEngine needs to have version 2014.0 or later
 * -# The encoder factory needs to specify id, name, description, type and file extension (see below for an example).
 * -# To make an encoder option appear in the CityEngine UI, the following annotations are mandatory: LABEL, ORDER, GROUP and DESCRIPTION
 * -# The boolean option "errorFallback" needs to be present. If true, the encoder is expected to encode the initial shape geometry. Usually, this option should be hidden (see prtx::EncodeOptionsAnnotator::flagAsHidden()).
 *
 * Here is an example how to setup an encoder info object inside an encoder factory (the full sources are available in the "stlenc" example):
@verbatim
prtx::EncoderInfoBuilder encoderInfoBuilder;
encoderInfoBuilder.setID(...);
encoderInfoBuilder.setName(...);
encoderInfoBuilder.setDescription(...);
encoderInfoBuilder.setType(prt::CT_GEOMETRY);
encoderInfoBuilder.setExtension(L".ext");
@endverbatim
 *
 * \sa prtx::EncoderInfoBuilder
 *
 * ## Application Note 4: How-To create a FILE URI from a file system path
 *
 * All URIs in PRT follow the standard RFC3986: http://www.ietf.org/rfc/rfc3986.txt
 * In practice this means:
 * -# the URI string must be UTF-8 encoded
 * -# all characters which are not allowed according to section 2 of RFC3986 must be percent-encoded (see prtx::URI::percentEncode).
 * -# "file" URIs must contain an absolute path, i.e. start with a slash after "file:". This is also required on windows, i.e. "file:/C:/path/".
 *
 * Here is an example how to create such an URI from a file system path:
@verbatim
// linux/osx example (assuming UTF8 for native strings)
std::string fsp = "/home/johndoe/my/path/hello.txt";
std::string uri = prtx::URI::SCHEME_FILE + ":" + fsp; // results in 'file:/home/johndoe/my/path/hello.txt'
uri = prtx::URI::percentEncode(uri);
std::wstring wuri(uri.size(), L' ');
std::copy(uri.begin(), uri.end(), wuri.begin());
// now use wuri.c_str() for prt API calls...

// windows example (assuming Latin1 string encoding)
std::string fsp = "C:/some/path/file.txt"; // note: windows paths must be converted to forward slashes
std::string u8fsp = boost::locale::conv::to_utf<char>(fsp, "Latin1");
std::string uri = prtx::URI::SCHEME_FILE + ":/" + u8fsp; // note the slash before the drive letter
uri = prtx::URI::percentEncode(uri);
std::wstring wuri(uri.size(), L' ');
std::copy(uri.begin(), uri.end(), wuri.begin());
// now use wuri.c_str() for prt API calls...
@endverbatim
 *
 */


#endif /* PRT_H_ */
