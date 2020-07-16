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

#ifndef PRT_CONTENTTYPE_H_
#define PRT_CONTENTTYPE_H_


namespace prt {


/**
 * Encodes the types handled by Decoders and Encoders.
 */
enum ContentType {
	CT_UNDEFINED,		///< Extension does not make use of specific content type.
	CT_GEOMETRY,		///< Extension can handle geometries (and potentially also materials and textures).
	CT_MATERIAL,		///< Extension can handle materials (and potentially also textures).
	CT_TEXTURE,			///< Extension can handle textures.
	CT_SHADER,			///< (Reserved) Extension can handle shader resources.
	CT_CGB,				///< Extension can handle CGB resources.
	CT_INITIALSHAPE,	///< (Reserved) extension can handle initial shape.
	CT_CGAERROR,		///< Extension can handle CGA errors.
	CT_CGAPRINT,		///< Extension can handle CGA print output (see cga @cgaref{op_print.html,print()}).
	CT_CGAREPORT,		///< Extension can handle CGA reports (see cga @cgaref{op_report.html,report()}).
	CT_ATTRIBUTE,		///< Extension can handle CGA attributes.
	CT_SHAPETREE,		///< Extension can handle shape trees.
	CT_STRING,			///< Extension can handle strings.
	CT_TABLE,			///< Extension can handle tables.
	CT_COUNT			///< Sentinel value.
};


} // namespace prt


#endif /* PRT_CONTENTTYPE_H_ */
