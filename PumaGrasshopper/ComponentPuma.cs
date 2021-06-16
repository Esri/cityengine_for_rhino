﻿/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021 Esri R&D Center Zurich
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

using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Grasshopper.Kernel.Types;
using Grasshopper.Kernel.Data;

using Rhino.Geometry;
using System.Linq;
using PumaGrasshopper.Properties;
using System.Windows.Forms;
using GH_IO.Serialization;
using Grasshopper.Kernel.Parameters;
using System.Drawing;

// In order to load the result of this wizard, you will also need to
// add the output bin/ folder of this project to the list of loaded
// folder in Grasshopper.
// You can use the _GrasshopperDeveloperSettings Rhino command for that.
namespace PumaGrasshopper
{

    public class ComponentPuma : GH_Component, IGH_VariableParameterComponent
    {
        const string COMPONENT_NAME = "Puma";
        const string COMPONENT_NICK_NAME = "Puma";

        const string RPK_INPUT_NAME = "Path to Rule Package";
        const string RPK_INPUT_NICK_NAME = "RPK";

        const string GEOM_INPUT_NAME = "Input Shapes";
        const string GEOM_INPUT_NICK_NAME = "Shapes";

        const string GEOM_OUTPUT_NAME = "Generated Models";
        const string GEOM_OUTPUT_NICK_NAME = "Models";

        const string MATERIAL_OUTPUT_NAME = "Materials";
        const string MATERIAL_OUTPUT_NICK_NAME = "Materials";

        const string REPORTS_OUTPUT_NAME = "CGA Reports";
        const string REPORTS_OUTPUT_NICK_NAME = "Reports";

        const string RPK_PATH_SERIALIZE = "RPK_PATH";
        const string SEED_INPUT_NAME = "Seed";
        const string SEED_KEY = "seed";

        /// Stores the optional input parameters
        RuleAttribute[] mRuleAttributes;
        private readonly List<IGH_Param> mParams;

        bool mDoGenerateMaterials;

        string mCurrentRPK = "";

        /// <summary>
        /// Each implementation of GH_Component must provide a public
        /// constructor without any arguments.
        /// Category represents the Tab in which the component will appear,
        /// Subcategory the panel. If you use non-existing tab or panel names,
        /// new tabs/panels will automatically be created.
        /// </summary>
        public ComponentPuma()
          : base(COMPONENT_NAME, COMPONENT_NICK_NAME,
              "Puma runs CityEngine CGA rules on input shapes and returns the generated models. (Version " + PRTWrapper.GetVersion() + ")",
              ComponentLibraryInfo.MainCategory, ComponentLibraryInfo.PumaSubCategory)
        {
            // Initialize PRT engine
            bool status = PRTWrapper.InitializeRhinoPRT();
            if (!status) throw new Exception("Fatal Error: PRT initialization failed.");

            mRuleAttributes = new RuleAttribute[0];
            mParams = new List<IGH_Param>();

            mDoGenerateMaterials = true;
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddParameter(new Param_FilePath(), RPK_INPUT_NAME, RPK_INPUT_NICK_NAME,
                "Path to a CityEngine rule package (RPK).",
                GH_ParamAccess.item);
            pManager.AddGeometryParameter(GEOM_INPUT_NAME, GEOM_INPUT_NICK_NAME,
                "Input shapes on which to execute the rules.",
                GH_ParamAccess.tree);
            pManager.AddIntegerParameter(SEED_KEY, SEED_INPUT_NAME, 
                "A number that will be used to seed the PRT random number generator.", 
                GH_ParamAccess.tree, 0);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddGeometryParameter(GEOM_OUTPUT_NAME, GEOM_OUTPUT_NICK_NAME,
                "Generated model geometry per input shape.",
                GH_ParamAccess.tree);
            pManager.AddGenericParameter(MATERIAL_OUTPUT_NAME, MATERIAL_OUTPUT_NICK_NAME,
                "Material attributes per input shape.",
                GH_ParamAccess.tree);
            pManager.AddGenericParameter(REPORTS_OUTPUT_NAME, REPORTS_OUTPUT_NICK_NAME,
                "CGA report values per input shape.",
                GH_ParamAccess.tree);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object can be used to retrieve data from input parameters and
        /// to store data in output parameters.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            // Get default inputs

            // RPK path is a single item.
            string rpk_file = "";
            if (!DA.GetData(RPK_INPUT_NAME, ref rpk_file))
                return;
            if (rpk_file.Length == 0)
                return;

            // Once we have a rpk file, directly extract the rule attributes
            PRTWrapper.SetPackage(rpk_file);

            // Update the rule attributes only if the rpk is changed.
            if (mCurrentRPK != rpk_file)
            {
                mCurrentRPK = rpk_file;

                //if rule attributes input parameters are already existing, remove them.
                if(mRuleAttributes.Length > 0)
                {
                    foreach(var param in mParams)
                    {
                        Params.UnregisterInputParameter(param);
                    }

                    mParams.Clear();
                }

                mRuleAttributes = PRTWrapper.GetRuleAttributes();
                foreach (RuleAttribute attrib in mRuleAttributes)
                {
                    if(attrib.mFullName != SEED_KEY)
                        CreateInputParameter(attrib);
                }

                Params.OnParametersChanged();
                ExpireSolution(true);
                return;
            }

            PRTWrapper.ClearInitialShapes();

            // Get the initial shape inputs
            if (!DA.GetDataTree<IGH_GeometricGoo>(GEOM_INPUT_NAME, out GH_Structure<IGH_GeometricGoo> shapeTree))
                return;

            // Transform each geometry to a mesh
            List<Mesh> meshes = new List<Mesh>();

            int initShapeIdx = 0;
            foreach(IGH_GeometricGoo geom in shapeTree.AllData(true))
            {
                Mesh mesh = ConvertToMesh(geom);

                if (mesh != null)
                {
                    mesh.SetUserString(PRTWrapper.INIT_SHAPE_IDX_KEY, initShapeIdx.ToString());
                    meshes.Add(mesh);
                }
                initShapeIdx++;
            }

            // No compatible mesh was given
            if (meshes.Count == 0)
                return;

            if(!PRTWrapper.AddMesh(meshes))
                return;

            // Get all node input corresponding to the list of mRuleAttributes registered.
            FillAttributesFromNode(DA, meshes.Count);

            var generatedMeshes = PRTWrapper.GenerateMesh();

            if (mDoGenerateMaterials && generatedMeshes != null)
            {
                GH_Structure<GH_Material> materials = PRTWrapper.GetAllMaterialIds(generatedMeshes.DataCount);
                DA.SetDataTree(1, materials);
            }

            OutputReports(DA, generatedMeshes);
            DA.SetDataTree(0, generatedMeshes);
        }

        protected override void AppendAdditionalComponentMenuItems(ToolStripDropDown menu)
        {
            base.AppendAdditionalComponentMenuItems(menu);

            Menu_AppendItem(menu, "Generate Materials", OnMaterialToggleClicked, true, mDoGenerateMaterials);
        }

        private void OnMaterialToggleClicked(object sender, EventArgs e)
        {
            mDoGenerateMaterials = !mDoGenerateMaterials;
            PRTWrapper.SetMaterialGenerationOption(mDoGenerateMaterials);

            ExpireSolution(true);
        }

        private void OutputReports(IGH_DataAccess DA, GH_Structure<GH_Mesh> gh_meshes)
        {
            GH_Structure<ReportAttribute> outputTree = new GH_Structure<ReportAttribute>();

            int count = gh_meshes.DataCount;
            for(int meshID = 0; meshID < count; ++meshID)
            {
                var reports = PRTWrapper.GetAllReports(meshID);

                // The new branch
                GH_Path path = new GH_Path(meshID);
                reports.ForEach(x => outputTree.Append(x, path));
            }

            DA.SetDataTree(2, outputTree);
        }

        /// <summary>
        /// Input object types supported are: GH_Mesh, GH_Brep, GH_Rectangle, GH_Surface, GH_Box, GH_Plane.
        /// </summary>
        /// <param name="shape">An initial shape</param>
        /// <returns>The shape converted to a Mesh</returns>
        private Mesh ConvertToMesh(IGH_GeometricGoo shape)
        {
            Mesh mesh = null;

            // Cast the shape to its actual Rhino.Geometry type.
            IGH_GeometricGoo geoGoo = shape; // copy

            if(geoGoo is GH_Mesh)
            {
                GH_Mesh m = geoGoo as GH_Mesh;
                if(!GH_Convert.ToMesh(m, ref mesh, GH_Conversion.Both)) return null;
            }
            else if (geoGoo is GH_Brep)
            {
                GH_Brep brep = geoGoo as GH_Brep;
                Brep brepShape = null;
                if(!GH_Convert.ToBrep(brep, ref brepShape, GH_Conversion.Both)) return null;

                mesh = new Mesh();
                mesh.Append(Mesh.CreateFromBrep(brepShape, MeshingParameters.DefaultAnalysisMesh));
                mesh.Compact();
            }
            else if (geoGoo is GH_Rectangle)
            {
                Rectangle3d rect = Rectangle3d.Unset;
                bool status = GH_Convert.ToRectangle3d(geoGoo as GH_Rectangle, ref rect, GH_Conversion.Both);

                if (!status) return null;

                mesh = Mesh.CreateFromClosedPolyline(rect.ToPolyline());
            }
            else if (geoGoo is GH_Surface)
            {
                Surface surf = null;
                if(!GH_Convert.ToSurface(geoGoo as GH_Surface, ref surf, GH_Conversion.Both)) return null;
                mesh = Mesh.CreateFromSurface(surf, MeshingParameters.QualityRenderMesh);
            }
            else if (geoGoo is GH_Box)
            {
                if(!GH_Convert.ToMesh(geoGoo as GH_Box, ref mesh, GH_Conversion.Both)) return null;
            }
            else if(geoGoo is GH_Plane)
            {
                if (!GH_Convert.ToMesh(geoGoo as GH_Plane, ref mesh, GH_Conversion.Both)) return null;
            }
            else
            {
                return null;
            }

            mesh.Vertices.UseDoublePrecisionVertices = true;
            mesh.Faces.ConvertTrianglesToQuads(Rhino.RhinoMath.ToRadians(2), .875);

            return mesh;
        }

        /// <summary>
        /// Add rule attributes inputs to the grasshopper component.
        /// </summary>
        /// <param name="attrib">A rule attribute to add as input</param>
        private void CreateInputParameter(RuleAttribute attrib)
        {
            var parameter = attrib.GetInputParameter();
            mParams.Add(parameter);

            // Check if the param already exists and replace it to avoid adding duplicates.
            int index = Params.IndexOfInputParam(parameter.Name);
            if (index != -1)
            {
                //If the existing parameter is connected to a remote source, the wire connection need to be ported.
                for(int i = 0; i < Params.Input[index].SourceCount; ++i)
                {
                    parameter.AddSource(Params.Input[index].Sources[i]);
                }

                Params.Input.RemoveAt(index);
                Params.Input.Insert(index, parameter);
            }
            else
                Params.RegisterInputParam(parameter);
        }

        private void SetAttributeOfShapes(IGH_DataAccess DA, int shapeCount, RuleAttribute attribute)
        {
            switch (attribute.mAttribType)
            {
                case AnnotationArgumentType.AAT_INT:
                {
                    if (!DA.GetDataTree(attribute.mFullName, out GH_Structure<GH_Integer> tree)) return;
                    ExtractTreeValues(tree, attribute, shapeCount);
                    break;
                }
                case AnnotationArgumentType.AAT_BOOL:
                case AnnotationArgumentType.AAT_BOOL_ARRAY:
                {
                    if (!DA.GetDataTree(attribute.mFullName, out GH_Structure<GH_Boolean> tree)) return;
                    ExtractTreeValues(tree, attribute, shapeCount);
                    break;
                }
                case AnnotationArgumentType.AAT_FLOAT:
                case AnnotationArgumentType.AAT_FLOAT_ARRAY:
                {
                    if (!DA.GetDataTree(attribute.mFullName, out GH_Structure<GH_Number> tree)) return;
                    ExtractTreeValues(tree, attribute, shapeCount);
                    break;
                }
                case AnnotationArgumentType.AAT_STR:
                case AnnotationArgumentType.AAT_STR_ARRAY:
                    if (attribute.IsColor())
                    {
                        if (!DA.GetDataTree(attribute.mFullName, out GH_Structure<GH_Colour> tree)) return;
                        ExtractTreeValues(tree, attribute, shapeCount);
                    }
                    else
                    {
                        if (!DA.GetDataTree(attribute.mFullName, out GH_Structure<GH_String> tree)) return;
                        ExtractTreeValues(tree, attribute, shapeCount);
                    }
                    break;
                default:
                {
                    if (!DA.GetDataTree(attribute.mFullName, out GH_Structure<IGH_Goo> tree)) return;
                    ExtractTreeValues(tree, attribute, shapeCount);
                    break;
                }
            }
        }

        private void ExtractTreeValues<T>(GH_Structure<T> tree, RuleAttribute attribute, int shapeCount) where T: IGH_Goo
        {
            if (tree.IsEmpty) return;

            int shapeId = 0;

            if (attribute.mAttribType == AnnotationArgumentType.AAT_BOOL_ARRAY ||
               attribute.mAttribType == AnnotationArgumentType.AAT_FLOAT_ARRAY ||
               attribute.mAttribType == AnnotationArgumentType.AAT_STR_ARRAY)
            {
                // Grasshopper behaviour: repeat last item/branch when there is more shapes than rule attributes.
                while (shapeCount > tree.Branches.Count)
                {
                    tree.Branches.Add(tree.Branches.Last());
                }

                foreach (List<T> branch in tree.Branches)
                {
                    if (shapeId >= shapeCount) return;

                    SetRuleAttributeArray(shapeId, attribute, branch);
                    shapeId++;
                }
            }
            else
            {
                // Transform the tree to a list
                List<T> values = tree.ToList();

                // Complete missing values with the last one.
                while (shapeCount > values.Count)
                {
                    values.Add(values.Last());
                }

                foreach (T value in values)
                {
                    if (value.IsValid)
                    {
                        if (shapeId >= shapeCount) return;

                        SetRuleAttribute(shapeId, attribute, value);
                    }
                    shapeId++;
                }
            }
        }

        private void SetRuleAttributeArray<T>(int shapeId, RuleAttribute attribute, List<T> values) where T:IGH_Goo
        {
            switch (attribute.mAttribType)
            {
                case AnnotationArgumentType.AAT_FLOAT_ARRAY:
                    {
                        List<double> doubleList = values.Cast<double>().ToList();
                        PRTWrapper.SetRuleAttributeDoubleArray(shapeId, attribute.mRuleFile, attribute.mFullName, doubleList);
                        return;
                    }
                case AnnotationArgumentType.AAT_BOOL_ARRAY:
                    {
                        List<Boolean> boolList = values.Cast<bool>().ToList();
                        PRTWrapper.SetRuleAttributeBoolArray(shapeId, attribute.mRuleFile, attribute.mFullName, boolList);
                        return;
                    }
                case AnnotationArgumentType.AAT_STR_ARRAY:
                    {
                        List<string> stringList = values.Cast<string>().ToList();
                        PRTWrapper.SetRuleAttributeStringArray(shapeId, attribute.mRuleFile, attribute.mFullName, stringList);
                        return;
                    }
                default:
                    return;
            }
        }

        private void SetRuleAttribute<T>(int shapeId, RuleAttribute attribute, T value) where T : IGH_Goo {
            switch (attribute.mAttribType)
            {
                case AnnotationArgumentType.AAT_FLOAT:
                    {
                        if (!value.CastTo(out double number)) throw new Exception(Utils.GetCastErrorMessage(attribute, "double"));
                        PRTWrapper.SetRuleAttributeDouble(shapeId, attribute.mRuleFile, attribute.mFullName, number);
                        return;
                    }
                case AnnotationArgumentType.AAT_BOOL:
                    {
                        if (!value.CastTo(out bool boolean)) throw new Exception(Utils.GetCastErrorMessage(attribute, "bool"));
                        PRTWrapper.SetRuleAttributeBoolean(shapeId, attribute.mRuleFile, attribute.mFullName, boolean);
                        return;
                    }
                case AnnotationArgumentType.AAT_INT:
                    {
                        if (!value.CastTo(out int integer)) throw new Exception(Utils.GetCastErrorMessage(attribute, "integer")); 
                        PRTWrapper.SetRuleAttributeInteger(shapeId, attribute.mRuleFile, attribute.mFullName, integer);
                        return;
                    }
                case AnnotationArgumentType.AAT_STR:
                    {
                        string text;
                        if (attribute.IsColor())
                        {
         
                            if (!value.CastTo(out Color color)) throw new Exception(Utils.GetCastErrorMessage(attribute, "Color"));
                            text = Utils.hexColor(color);
                        }
                        else
                        {
                            if (!value.CastTo(out text)) throw new Exception(Utils.GetCastErrorMessage(attribute, "string"));
                        }

                        PRTWrapper.SetRuleAttributeString(shapeId, attribute.mRuleFile, attribute.mFullName, text);
                        return;
                    }
                default:
                    return;
            }
        }

        private void FillAttributesFromNode(IGH_DataAccess DA, int shapeCount)
        {
            for (int idx = 0; idx < mRuleAttributes.Length; ++idx)
            {
                RuleAttribute attrib = mRuleAttributes[idx];
                SetAttributeOfShapes(DA, shapeCount, attrib);
            }
        }

        public bool CanInsertParameter(GH_ParameterSide side, int index)
        {
            return false;
        }

        public bool CanRemoveParameter(GH_ParameterSide side, int index)
        {
            return false;
        }

        public IGH_Param CreateParameter(GH_ParameterSide side, int index)
        {
            return null;
        }

        public bool DestroyParameter(GH_ParameterSide side, int index)
        {
            return false;
        }

        public void VariableParameterMaintenance()
        {
            return;
        }

        public override bool Write(GH_IWriter writer)
        {
            writer.SetString(RPK_PATH_SERIALIZE, mCurrentRPK);
            return base.Write(writer);
        }

        public override bool Read(GH_IReader reader)
        {
            if(reader.ChunkExists(RPK_PATH_SERIALIZE))
                mCurrentRPK = reader.GetString(RPK_PATH_SERIALIZE);
            return base.Read(reader);
        }

        /// <summary>
        /// Provides an Icon for every component that will be visible in the User Interface.
        /// Icons need to be 24x24 pixels.
        /// </summary>
        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                // You can add image files to your project resources and access them like this:
                return Resources.gh_prt_main_component;
            }
        }

        /// <summary>
        /// Each component must have a unique Guid to identify it.
        /// It is vital this Guid doesn't change otherwise old ghx files
        /// that use the old ID will partially fail during loading.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("ad54a111-cdbc-4417-bddd-c2195c9482d8"); }
        }
    }
}