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

#include <iostream>
#include <fstream>

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
		catch (std::exception& e) {
			std::ofstream outfile;
			outfile.open("C:\\Windows\\Temp\\rhino_log_2.txt", std::ios::out | std::ios::trunc);
			outfile << e.what() << std::endl;
			outfile.close();
			
			mEncodePreparator->add(context.getCache(), *is, initialShapeIndex);
		}
		catch (...) {
			mEncodePreparator->add(context.getCache(), *is, initialShapeIndex);
		}

		std::vector<prtx::EncodePreparator::FinalizedInstance> finalizedInstances;
		mEncodePreparator->fetchFinalizedInstances(finalizedInstances, ENC_PREP_FLAGS);
		uint32_t vertexIndexBase = 0;

		std::vector<double> vertexCoords;
		std::vector<uint32_t> faceIndices;
		std::vector<uint32_t> faceCounts;

		for (const auto& instance : finalizedInstances) {
			const prtx::MeshPtrVector& meshes = instance.getGeometry()->getMeshes();

			vertexCoords.clear();
			faceIndices.clear();
			faceCounts.clear();

			for (const auto& mesh : meshes) {
				const prtx::DoubleVector& verts = mesh->getVertexCoords();
				vertexCoords.insert(vertexCoords.end(), verts.begin(), verts.end());

				for (uint32_t fi = 0; fi < mesh->getFaceCount(); ++fi) {
					const uint32_t* vtxIdx = mesh->getFaceVertexIndices(fi);
					const uint32_t vtxCnt = mesh->getFaceVertexCount(fi);
					faceCounts.push_back(vtxCnt);
					for (uint32_t vi = 0; vi < vtxCnt; vi++)
						faceIndices.push_back(vtxIdx[vi] + vertexIndexBase);
				}
				vertexIndexBase += (uint32_t)verts.size() / 3;
			}

			cb->addGeometry(instance.getInitialShapeIndex(), vertexCoords.data(), vertexCoords.size(),
				faceIndices.data(), faceIndices.size(), faceCounts.data(), faceCounts.size());
		}
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