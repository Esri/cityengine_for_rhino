/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
 *
 * Copyright (c) 2021-2024 Esri R&D Center Zurich
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

 ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN ApplyRulePackage command
//
#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "RhinoPRTPlugIn.h"

/// Helper function to open a file browser using MFC's CFileDialog class.
bool getRpkPath(std::wstring& rpk) {
	std::wstring ext = L".rpk";
	std::wstring filename = L"default";
	std::wstring default_ext = L"RPK (*.rpk) | *.rpk";
	CFileDialog fd(true, (LPCWSTR)ext.c_str(), (LPCWSTR)filename.c_str(), OFN_FILEMUSTEXIST,
	               (LPCWSTR)default_ext.c_str());
	auto result = fd.DoModal();
	if (!result)
		return false;
	rpk.assign(fd.GetPathName());
	return true;
}

#pragma region ApplyRulePackage command

class CCommandApplyRulePackage : public CRhinoCommand {
public:
	CCommandApplyRulePackage() : CRhinoCommand(false, false, &RhinoPRTPlugIn(), false){};
	UUID CommandUUID() override {
		// {B960EDCA-8D50-4293-8CD6-FE5F8843C975}
		static const GUID ApplyRulePackageCommand_UUID = {
		        0xB960EDCA, 0x8D50, 0x4293, {0x8C, 0xD6, 0xFE, 0x5F, 0x88, 0x43, 0xC9, 0x75}};
		return ApplyRulePackageCommand_UUID;
	}
	const wchar_t* EnglishCommandName() override {
		return L"ApplyRulePackage";
	}
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandExtrudeShape object
static class CCommandApplyRulePackage theApplyRulePackageCommand;

CRhinoCommand::result CCommandApplyRulePackage::RunCommand(const CRhinoCommandContext& context) {
	std::wstring rpk;
	if (!getRpkPath(rpk))
		return cancel;

	// Select starting shapes
	ON_SimpleArray<const ON_Mesh*> mesh_array;

	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select 1 or more starting shapes");
	go.SetGeometryFilter(CRhinoGetObject::surface_object | CRhinoGetObject::mesh_object |
	                     CRhinoGetObject::closed_polysrf | CRhinoGetObject::GEOMETRY_TYPE_FILTER::extrusion_object);

	// Get selected objects and convert them to ON_Mesh.
	CRhinoGet::result res = go.GetObjects(1, 0);
	if (res == CRhinoGet::object) {
		int count = go.ObjectCount();
		for (int i = 0; i < count; ++i) {
			ON_Mesh* mesh = nullptr;

			const CRhinoObjRef& obj_ref = go.Object(i);

			auto geom_type = obj_ref.GeometryType();

			if (geom_type == CRhinoObject::GEOMETRY_TYPE::surface_object) {

				const ON_Surface* srf = obj_ref.Surface();
				if (srf) {
					mesh = new ON_Mesh();
					srf->CreateMesh(ON_MeshParameters::QualityRenderMesh, mesh);
				}
			}
			else if (geom_type == CRhinoObject::GEOMETRY_TYPE::extrusion_object) {
				const ON_Extrusion* extr = obj_ref.Extrusion();
				if (extr) {
					mesh = extr->CreateMesh(ON_MeshParameters::QualityRenderMesh);
				}
			}
			else if (geom_type == CRhinoObject::GEOMETRY_TYPE::polysrf_object) {

				const ON_Brep* brep = obj_ref.Brep();
				if (brep) {
					ON_SimpleArray<ON_Mesh*> brep_meshes;
					int nb_meshes = brep->CreateMesh(ON_MeshParameters::QualityRenderMesh, brep_meshes);

					mesh = new ON_Mesh();
					for (int j = 0; j < nb_meshes; ++j) {
						mesh->Append(*brep_meshes[j]);
					}
				}
			}
			else if (geom_type == CRhinoObject::GEOMETRY_TYPE::mesh_object) {
				mesh = new ON_Mesh();
				mesh->Append(*obj_ref.Mesh());
			}
			else {
				LOG_ERR << L"Incompatible initial shape." << std::endl;
			}

			if (mesh) {
				mesh->SetUserString(L"InitShapeIdx", std::to_wstring(i).c_str());
				mesh_array.Append(mesh);
			}
		}
	}
	else
		return cancel;

	if (mesh_array.Count() == 0) {
		LOG_ERR << L"No compatible initial shape was given, aborting...";
		return failure;
	}

	std::vector<RawInitialShape> rawInitialShapes;
	rawInitialShapes.reserve(mesh_array.Count());
	for (int i = 0; i < mesh_array.Count(); ++i)
		rawInitialShapes.emplace_back(*mesh_array[i]);

	// Initialise the attribute map builders for each initial shape.
	pcu::AttributeMapBuilderVector aBuilders(mesh_array.Count());
	for (auto& it : aBuilders) {
		it.reset(prt::AttributeMapBuilder::create());
	}

	// PRT Generation
	const auto& generated_models = RhinoPRT::get().GenerateGeometry(rpk, rawInitialShapes, aBuilders);

	// Add the objects to the Rhino scene.
	for (size_t initialShapeIndex = 0; initialShapeIndex < generated_models.size(); initialShapeIndex++) {
		if (!generated_models[initialShapeIndex])
			continue;

		const auto& meshBundle = generated_models[initialShapeIndex]->createRhinoMeshes(initialShapeIndex);

		std::for_each(meshBundle.begin(), meshBundle.end(),
		              [&context](const ON_Mesh& mesh) { context.m_doc.AddMeshObject(mesh); });
	}

	context.m_doc.Redraw();

	return CRhinoCommand::success;
}

#pragma endregion

//
// END ApplyRulePackage command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////