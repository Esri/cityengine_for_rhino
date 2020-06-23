#include "RhinoEncoder.h"

#include "IRhinoCallbacks.h"

#include "prtx/EncoderInfoBuilder.h"
#include "prtx/GenerateContext.h"
#include "prtx/Exception.h"
#include "prtx/Geometry.h"
#include "prtx/Mesh.h"
#include "prtx/ReportsCollector.h"
#include "prtx/Shape.h"
#include "prtx/ShapeIterator.h"
#include "prtx/prtx.h"

namespace {

	const wchar_t* EO_BASE_NAME = L"baseName";
	const wchar_t* EO_ERROR_FALLBACK = L"errorFallback";
	const std::wstring ENCFILE_EXT = L".txt";
	const wchar_t* EO_EMIT_REPORT = L"emitReport";
	const wchar_t* EO_EMIT_GEOMETRY = L"emitGeometry";

	const prtx::EncodePreparator::PreparationFlags ENC_PREP_FLAGS =
		prtx::EncodePreparator::PreparationFlags()
		.instancing(false)
		.triangulate(false)
		.mergeVertices(false)
		.cleanupUVs(false)
		.cleanupVertexNormals(false)
		.mergeByMaterial(true);
}

const std::wstring RhinoEncoder::ID = L"com.esri.rhinoprt.RhinoEncoder";
const std::wstring RhinoEncoder::NAME = L"Rhino Geometry and Report Encoder";
const std::wstring RhinoEncoder::DESCRIPTION = L"Encodes geometry and CGA report for Rhino.";

void RhinoEncoder::init(prtx::GenerateContext& context) {
	prtx::NamePreparator::NamespacePtr nsMaterials = mNamePreparator.newNamespace();
	prtx::NamePreparator::NamespacePtr nsMeshes = mNamePreparator.newNamespace();
	mEncodePreparator = prtx::EncodePreparator::create(true, mNamePreparator, nsMeshes, nsMaterials);
}

void RhinoEncoder::encode(prtx::GenerateContext& context, size_t initialShapeIndex) {

	const prtx::InitialShape* is = context.getInitialShape(initialShapeIndex);

	auto* cb = dynamic_cast<IRhinoCallbacks*>(getCallbacks());
	if (cb == nullptr)
		throw prtx::StatusException(prt::STATUS_ILLEGAL_CALLBACK_OBJECT);

	if (getOptions()->getBool(EO_EMIT_REPORT)) {
		//ignore for now
	}

	if (getOptions()->getBool(EO_EMIT_GEOMETRY)) {
		try {
			prtx::LeafIteratorPtr li = prtx::LeafIterator::create(context, initialShapeIndex);

			for (prtx::ShapePtr shape = li->getNext(); shape.get() != nullptr; shape = li->getNext()) {
				mEncodePreparator->add(context.getCache(), shape, is->getAttributeMap());
			}
		}
		catch (...) {
			mEncodePreparator->add(context.getCache(), *is, initialShapeIndex);
		}

		std::vector<prtx::EncodePreparator::FinalizedInstance> finalizedInstances;
		mEncodePreparator->fetchFinalizedInstances(finalizedInstances, ENC_PREP_FLAGS);
		uint32_t vertexIndexBase = 0;
	}
}

void RhinoEncoder::finish(prtx::GenerateContext& context) {}


RhinoEncoderFactory* RhinoEncoderFactory::createInstance() {
	prtx::EncoderInfoBuilder encoderInfoBuilder;

	encoderInfoBuilder.setID(RhinoEncoder::ID);
	encoderInfoBuilder.setName(RhinoEncoder::NAME);
	encoderInfoBuilder.setDescription(RhinoEncoder::DESCRIPTION);
	encoderInfoBuilder.setType(prt::CT_GEOMETRY);
	encoderInfoBuilder.setExtension(ENCFILE_EXT);

	// Default encoder options
	prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());
	amb->setString(EO_BASE_NAME, L"enc_default_name");
	amb->setBool(EO_ERROR_FALLBACK, prtx::PRTX_TRUE);
	amb->setBool(EO_EMIT_GEOMETRY, prtx::PRTX_TRUE);
	amb->setBool(EO_EMIT_REPORT, prtx::PRTX_TRUE);
	encoderInfoBuilder.setDefaultOptions(amb->createAttributeMap());

	return new RhinoEncoderFactory(encoderInfoBuilder.create());
}