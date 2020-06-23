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
	CCommandExtrudeShape() = default;
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
	// Add a square, expose the extrusion height. Give the shape and rule, give everything to ptr.
	// The callback should be able to get the answer back, then convert the created face into rhino geometry using a decoder,
	// and finally display it in Rhino.

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

	ModelGenerator model_generator(shapes);

	// Model generation argument setup
	const std::string rpk = "C:/Users/lor11212/Documents/Rhino/rhino-plugin-prototype/extrusion_rule.rpk";
	const pcu::ShapeAttributes shapeAttr;
	std::vector<pcu::ShapeAttributes> shapeAttrs;
	shapeAttrs.push_back(shapeAttr);
	const std::wstring encoder = L"com.esri.rhinoprt.RhinoEncoder";
	const pcu::EncoderOptions encoder_options;

	// PRT Generation
	auto generated_models = model_generator.generateModel(shapeAttrs, rpk, encoder, encoder_options);
	

	return CRhinoCommand::success;
}

#pragma endregion

//
// END ExtrudeShape command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
