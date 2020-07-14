////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN ExtrudeShape command
//
#include "stdafx.h"
#include "RhinoPRTPlugIn.h"

#pragma region ExtrudeShape command

ON_Mesh getMeshFromGenModel(GeneratedModel& model) {
	auto faces = model.getFaces();
	auto vertices = model.getVertices();
	auto indices = model.getIndices();

	size_t nbVertices = vertices.size() / 3;

	ON_Mesh mesh(faces.size(), nbVertices, false, false);

	for (size_t v_id = 0; v_id < nbVertices; ++v_id) {
		mesh.SetVertex(v_id, ON_3dPoint(vertices[v_id * 3], vertices[v_id * 3 + 1], vertices[v_id * 3 + 2]));
	}

	int faceid(0);
	int currindex(0);
	for (int face : faces) {
		if (face == 3) {
			mesh.SetTriangle(faceid, indices[currindex], indices[currindex + 1], indices[currindex + 2]);
			currindex += face;
		}
		else if (face == 4) {
			mesh.SetQuad(faceid, indices[currindex], indices[currindex + 1], indices[currindex + 2], indices[currindex + 3]);
			currindex += face;
		} else {
			//ignore face because it is invalid
			currindex += face;
		}
		faceid++;
	}

	// Printing an error log if the created mesh is invalid
	FILE* fp = ON::OpenFile(L"C:\\Windows\\Temp\\rhino_log_2.txt", L"w");
	if (fp) {
		ON_TextLog log(fp);
		if (!mesh.IsValid(&log))
			mesh.Dump(log);
		ON::CloseFile(fp);
	}

	mesh.ComputeVertexNormals();
	mesh.Compact();

	return mesh;
}

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

	const ON_Plane plane;
	const ON_PlaneSurface psurf(plane);

	CRhinoSurfaceObject* surf_obj = context.m_doc.AddSurfaceObject(psurf);
	context.m_doc.Redraw();
	
	// Create the mesh and get it
	int crv_id = surf_obj->CreateMeshes(ON::mesh_type::preview_mesh, ON_MeshParameters::QualityRenderMesh);

	ON_SimpleArray<const ON_Mesh*> mesh_array;
	int msh_id = surf_obj->GetMeshes(ON::mesh_type::preview_mesh, mesh_array);

	std::vector<InitialShape> shapes;
	for (int i = 0; i < mesh_array.Count(); ++i) {
		std::vector<double> vertices;
		for (int j = 0; j < mesh_array[i]->VertexCount(); ++j) {
			vertices.push_back(mesh_array[i]->Vertex(j).x);
			vertices.push_back(mesh_array[i]->Vertex(j).y);
			vertices.push_back(mesh_array[i]->Vertex(j).z);
		}
	
		shapes.push_back(InitialShape(vertices));
	}


	// Model generation argument setup

	// TODO: the rpk must be entered by the user.
	const std::wstring rpk = L"C:/Users/lor11212/Documents/Rhino/rhino-plugin-prototype/extrusion_rule.rpk";
	SetPackage(rpk.c_str());

	RhinoPRT::myPRTAPI->AddInitialShape(shapes);

	// PRT Generation
	auto generated_models = RhinoPRT::myPRTAPI->GenerateGeometry();

	// Create Rhino object with given geometry
	for (auto& model : generated_models) {
		ON_Mesh mesh = getMeshFromGenModel(model);

		/*if (mesh.IsValid()) {
			if (!mesh.HasVertexNormals())
				mesh.ComputeVertexNormals();
			
			mesh.Compact();

			context.m_doc.AddMeshObject(mesh);
			context.m_doc.Redraw();
		}*/

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
