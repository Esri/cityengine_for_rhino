////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN ExtrudeShape command
//
#include "stdafx.h"
#include "RhinoPRTPlugIn.h"

bool getRpkPath(std::wstring& rpk) {
	std::wstring ext = L".rpk";
	std::wstring filename = L"default";
	std::wstring default_ext = L"RPK (*.rpk) | *.rpk";
	CFileDialog fd(true, (LPCWSTR)ext.c_str(), (LPCWSTR)filename.c_str(), OFN_FILEMUSTEXIST, (LPCWSTR)default_ext.c_str());
	auto result = fd.DoModal();
	if (!result) return false;
	rpk.assign(fd.GetPathName());
	return true;
}

#pragma region ExtrudeShape command


class CCommandExtrudeShape : public CRhinoCommand
{
public:
	CCommandExtrudeShape() : CRhinoCommand(false, false, &RhinoPRTPlugIn(), false) {};
	UUID CommandUUID() override
	{
		// {B960EDCA-8D50-4293-8CD6-FE5F8843C975}
		static const GUID ExtrudeShapeCommand_UUID =
		{ 0xB960EDCA, 0x8D50, 0x4293, { 0x8C, 0xD6, 0xFE, 0x5F, 0x88, 0x43, 0xC9, 0x75 } };
		return ExtrudeShapeCommand_UUID;
	}
	const wchar_t* EnglishCommandName() override { return L"ExtrudeShape"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandExtrudeShape object
static class CCommandExtrudeShape theExtrudeShapeCommand;

CRhinoCommand::result CCommandExtrudeShape::RunCommand(const CRhinoCommandContext& context)
{
	std::wstring rpk;
	if (!getRpkPath(rpk)) return cancel;

	// Select starting shapes
	ON_SimpleArray<const ON_Mesh*> mesh_array;

	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select 1 or more starting shapes");
	go.SetGeometryFilter(CRhinoGetObject::surface_object | CRhinoGetObject::mesh_object | 
						 CRhinoGetObject::closed_polysrf | CRhinoGetObject::GEOMETRY_TYPE_FILTER::extrusion_object);
	// Get selected objects
	CRhinoGet::result res = go.GetObjects(1, 0);
	if(res == CRhinoGet::object)
	{
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
			else if (geom_type == CRhinoObject::GEOMETRY_TYPE::polysrf_object)
			{
				 
				const ON_Brep* brep = obj_ref.Brep();
				if (brep) {
					ON_SimpleArray<ON_Mesh*> brep_meshes;
					int nb_meshes = brep->CreateMesh(ON_MeshParameters::QualityRenderMesh, brep_meshes);

					mesh = new ON_Mesh();
					for (int i = 0; i < nb_meshes; ++i) {
						mesh->Append(*brep_meshes[i]);
					}
				}
			}
			else if(geom_type == CRhinoObject::GEOMETRY_TYPE::mesh_object) {
				mesh = new ON_Mesh();
				mesh->Append(*obj_ref.Mesh());
			}
			else {
				LOG_ERR << L"Incompatible initial shape." << std::endl;
			}

			if (mesh) mesh_array.Append(mesh);
		}
	}
	else return cancel;

	if (mesh_array.Count() == 0) return failure;

	// Model generation argument setup
	SetPackage(rpk.c_str());
	AddMeshTest(&mesh_array);

	// PRT Generation
	auto generated_models = RhinoPRT::myPRTAPI->GenerateGeometry();

	// Create Rhino object with given geometry
	for (auto& model : generated_models) {
		ON_Mesh mesh = pcu::getMeshFromGenModel(model);

		auto meshOBject = context.m_doc.AddMeshObject(mesh);
	}

	context.m_doc.Redraw();
	
	return CRhinoCommand::success;
}

#pragma endregion

//
// END ExtrudeShape command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
