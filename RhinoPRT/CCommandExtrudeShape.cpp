////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN ExtrudeShape command
//
#include "stdafx.h"
#include "RhinoPRTPlugIn.h"

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
	// Add a square, expose the extrusion height. Give the shape and rule, give everything to PRT.
	// The callback should be able to get the answer back, then convert the created face into rhino geometry using a decoder,
	// and finally display it in Rhino.

	// Select starting shape
	bool bAttribute = true;

	CRhinoGetObject go;
	go.SetCommandPrompt(L"Select starting shape");
	go.AddCommandOptionToggle(
		RHCMDOPTNAME(L"Location"),
		RHCMDOPTVALUE(L"Object"),
		RHCMDOPTVALUE(L"Attribute"),
		bAttribute,
		&bAttribute
	);

	// Get selected object
	for (;;) {
		CRhinoGet::result res = go.GetObjects(1, 1);
		if (res == CRhinoGet::option)
			continue;
		if (res != CRhinoGet::object)
			return cancel;
		break;
	}

	const CRhinoObjRef& ref = go.Object(0);
	const CRhinoObject* obj = ref.Object();
	if (!obj)
		return failure;

	// manually adding a surface for testing/debugging purposes
	const ON_Plane plane;
	const ON_PlaneSurface psurf(plane);

	CRhinoSurfaceObject* surf_obj = context.m_doc.AddSurfaceObject(psurf);
	context.m_doc.Redraw();

	// Model generation argument setup

	// TODO: the rpk must be entered by the user.
	const std::wstring rpk = L"C:/Users/lor11212/Documents/Rhino/rhino-plugin-prototype/extrusion_rule.rpk";
	SetPackage(rpk.c_str());
	
	// Create the mesh and get it
	int crv_id = surf_obj->CreateMeshes(ON::mesh_type::preview_mesh, ON_MeshParameters::QualityRenderMesh);

	ON_SimpleArray<const ON_Mesh*> mesh_array;
	int msh_id = surf_obj->GetMeshes(ON::mesh_type::preview_mesh, mesh_array);

	if (mesh_array.Count() == 0) return CRhinoCommand::failure;

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
