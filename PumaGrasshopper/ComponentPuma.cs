/**
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
using System.Drawing;
using System.Diagnostics;
using Rhino.Runtime.InteropWrappers;
using System.IO;
using GH_IO.Serialization;

namespace PumaGrasshopper
{

    public class ComponentPuma : GH_Component, IGH_VariableParameterComponent
    {
        const string COMPONENT_NAME = "Puma";
        const string COMPONENT_NICK_NAME = "Puma";

        const string RPK_INPUT_NAME = "Path to Rule Package";
        const string RPK_INPUT_NICK_NAME = "RPK";
        const string RPK_INPUT_DESC = "Path to a CityEngine rule package (RPK).";

        const string GEOM_INPUT_NAME = "Input Shapes";
        const string GEOM_INPUT_NICK_NAME = "Shapes";
        const string GEOM_INPUT_DESC = "Input shapes on which to execute the rules.";

        const string SEED_INPUT_NAME = "Seed";
        const string SEED_KEY = "seed";
        const string SEED_INPUT_DESC = "A number that will be used to seed the PRT random number generator.";

        const string GEOM_OUTPUT_NAME = "Generated Models";
        const string GEOM_OUTPUT_NICK_NAME = "Models";
        const string GEOM_OUTPUT_DESC = "Generated model geometry per input shape.";

        const string MATERIAL_OUTPUT_NAME = "Materials";
        const string MATERIAL_OUTPUT_NICK_NAME = "Materials";
        const string MATERIAL_OUTPUT_DESC = "Material attributes per input shape.";

        const string REPORTS_OUTPUT_NAME = "CGA Reports";
        const string REPORTS_OUTPUT_NICK_NAME = "Reports";
        const string REPORTS_OUTPUT_DESC = "CGA report values per input shape.";

        const string CGA_PRINT_OUTPUT_NAME = "CGA Print Output";
        const string CGA_PRINT_OUTPUT_NICK_NAME = "Prints";
        const string CGA_PRINT_OUTPUT_DESC = "CGA print output per input shape.";

        const string CGA_ERROR_OUTPUT_NAME = "CGA and Asset Errors";
        const string CGA_ERROR_OUTPUT_NICK_NAME = "Errors";
        const string CGA_ERROR_OUTPUT_DESC = "CGA and asset errors encountered per input shape.";

        const string CITYENGINE_RESOURCES_URL = "https://doc.arcgis.com/en/cityengine";
        enum ParamType
        {
            GEOMETRY,
            FILEPATH,
            INTEGER,
            GENERIC
        }

        struct ParameterDescriptor
        {
            public ParamType type;
            public string name;
            public string nickName;
            public string desc;
        }

        enum InputParams
        {
            RPK,
            SHAPES,
            SEEDS
        }

        static readonly ParameterDescriptor[] INPUT_PARAM_DESC = new ParameterDescriptor[]{
            new ParameterDescriptor{ type = ParamType.FILEPATH, name = RPK_INPUT_NAME, nickName = RPK_INPUT_NICK_NAME, desc = RPK_INPUT_DESC },
            new ParameterDescriptor{ type = ParamType.GEOMETRY, name = GEOM_INPUT_NAME, nickName = GEOM_INPUT_NICK_NAME, desc = GEOM_INPUT_DESC },
            new ParameterDescriptor{ type = ParamType.INTEGER, name = SEED_KEY, nickName = SEED_INPUT_NAME, desc = SEED_INPUT_DESC },
        };

        enum OutputParams
        {
            MODELS,
            MATERIALS,
            REPORTS,
            PRINTS,
            ERRORS
        }

        static readonly ParameterDescriptor[] OUTPUT_PARAM_DESC = new ParameterDescriptor[]{
            new ParameterDescriptor{ type = ParamType.GEOMETRY, name = GEOM_OUTPUT_NAME, nickName = GEOM_OUTPUT_NICK_NAME, desc = GEOM_OUTPUT_DESC },
            new ParameterDescriptor{ type = ParamType.GENERIC, name = MATERIAL_OUTPUT_NAME, nickName = MATERIAL_OUTPUT_NICK_NAME, desc = MATERIAL_OUTPUT_DESC },
            new ParameterDescriptor{ type = ParamType.GENERIC, name = REPORTS_OUTPUT_NAME, nickName = REPORTS_OUTPUT_NICK_NAME, desc = REPORTS_OUTPUT_DESC },
            new ParameterDescriptor{ type = ParamType.GENERIC, name = CGA_PRINT_OUTPUT_NAME, nickName = CGA_PRINT_OUTPUT_NICK_NAME, desc = CGA_PRINT_OUTPUT_DESC },
            new ParameterDescriptor{ type = ParamType.GENERIC, name = CGA_ERROR_OUTPUT_NAME, nickName = CGA_ERROR_OUTPUT_NICK_NAME, desc = CGA_ERROR_OUTPUT_DESC },
        };

        /// Stores the optional input parameters
        RuleAttribute[] mRuleAttributes;

        bool mDoGenerateMaterials;

        public class RulePackage
        {
            public string path { get; set; }
            public DateTime timestamp { get; set; }
            
            public bool IsValid()
            {
                return (path != null) && (path.Length > 0) && (timestamp != null);
            }

            public bool IsSame(RulePackage other)
            {
                if (path == null || other.path == null)
                    return false; // default-initialized rule package paths are always different
                return (string.Compare(path, other.path) == 0) && (timestamp == other.timestamp);
            }
        };

        RulePackage mCurrentRPK;

        public ComponentPuma()
          : base(COMPONENT_NAME, COMPONENT_NICK_NAME,
              "Puma runs CityEngine CGA rules on input shapes and returns the generated models. (Version " + PRTWrapper.GetVersion() + ")",
              ComponentLibraryInfo.MainCategory, ComponentLibraryInfo.PumaSubCategory)
        {
            // Initialize PRT engine
            bool status = PRTWrapper.InitializeRhinoPRT();
            if (!status) throw new Exception("Fatal Error: PRT initialization failed.");

            mRuleAttributes = new RuleAttribute[0];

            mDoGenerateMaterials = true;
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            foreach (var param in Enum.GetValues(typeof(InputParams)).Cast<InputParams>())
            {
                var desc = INPUT_PARAM_DESC[(int)param];
                switch (desc.type)
                {
                    case ParamType.GEOMETRY:
                        pManager.AddGeometryParameter(desc.name, desc.nickName, desc.desc, GH_ParamAccess.tree);
                        break;
                    case ParamType.FILEPATH:
                        pManager.AddParameter(new RulePackageParam(), desc.name, desc.nickName, desc.desc, GH_ParamAccess.item);
                        break;
                    case ParamType.INTEGER:
                        pManager.AddIntegerParameter(desc.name, desc.nickName, desc.desc, GH_ParamAccess.tree, 0);
                        break;
                }
            }
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            foreach (var param in Enum.GetValues(typeof(OutputParams)).Cast<OutputParams>())
            {
                var desc = OUTPUT_PARAM_DESC[(int)param];
                switch (desc.type)
                {
                    case ParamType.GEOMETRY:
                        pManager.AddGeometryParameter(desc.name, desc.nickName, desc.desc, GH_ParamAccess.tree);
                        break;
                    case ParamType.GENERIC:
                        pManager.AddGenericParameter(desc.name, desc.nickName, desc.desc, GH_ParamAccess.tree);
                        break;
                }
            }
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            ClearRuntimeMessages();

            RulePackage rpk = GetRulePackage(DA);
            if (!rpk.IsValid())
                return;

            if (!CheckAndUpdateRulePackage(rpk))
                return;

            List<Mesh> inputMeshes = CreateInputMeshes(DA);
            if (inputMeshes == null || inputMeshes.Count == 0)
                return;

            PRTWrapper.ClearInitialShapes();
            if (!PRTWrapper.AddMesh(inputMeshes))
                return;

            FillAttributesFromNode(DA, inputMeshes.Count);

            var generatedMeshes = PRTWrapper.GenerateMesh();
            OutputGeometry(DA, generatedMeshes);
            OutputMaterials(DA, generatedMeshes);
            OutputReports(DA, generatedMeshes);
            OutputCGAPrint(DA, generatedMeshes);
            OutputCGAErrors(DA, generatedMeshes);
        }

        private RulePackage GetRulePackage(IGH_DataAccess dataAccess)
        {
            var result = new RulePackage();
            
            string rpkPath = "";
            if (!dataAccess.GetData(RPK_INPUT_NAME, ref rpkPath))
                return result;

            try
            {
                result.path = RulePackageParam.GetAbsoluteRulePackagePath(OnPingDocument(), rpkPath);

                FileInfo fi = new FileInfo(result.path);
                result.timestamp = fi.LastWriteTime;
            }
            catch (Exception e)
            {
                AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "Unable to compute absolute path to RPK: " + e.Message);
            }


            return result;
        }

        private bool SetRulePackage(RulePackage rulePackage)
        {
            var errorMsg = new StringWrapper();
            var pErrorMsg = errorMsg.NonConstPointer;
            PRTWrapper.SetPackage(rulePackage.path, pErrorMsg);
            if (errorMsg.ToString().Length > 0)
            {
                AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "Failed to set Rule Package: " + errorMsg);
                return false;
            }
            return true;
        }

        private bool CheckAndUpdateRulePackage(RulePackage potentiallyNewRulePackage)
        {
            if (mCurrentRPK == null || !mCurrentRPK.IsSame(potentiallyNewRulePackage))
            {
                mCurrentRPK = potentiallyNewRulePackage;
                if (!SetRulePackage(mCurrentRPK))
                    return false;
                mRuleAttributes = PRTWrapper.GetRuleAttributes();
            }
            return true;
        }

        private List<Mesh> CreateInputMeshes(IGH_DataAccess dataAccess)
        {
            if (!dataAccess.GetDataTree<IGH_GeometricGoo>(GEOM_INPUT_NAME, out GH_Structure<IGH_GeometricGoo> inputShapes))
                return null;

            var meshes = new List<Mesh>();

            int initShapeIdx = 0;
            foreach (IGH_GeometricGoo geom in inputShapes.AllData(true))
            {
                Mesh mesh = ConvertToMesh(geom);
                if (mesh != null)
                {
                    mesh.SetUserString(PRTWrapper.INIT_SHAPE_IDX_KEY, initShapeIdx.ToString());
                    meshes.Add(mesh);
                }
                initShapeIdx++;
            }

            return meshes;
        }

        private void FillAttributesFromNode(IGH_DataAccess DA, int shapeCount)
        {
            List<RuleAttribute> attributeList = mRuleAttributes.ToList();

            for (int idx = (int)InputParams.SEEDS; idx < Params.Input.Count; ++idx)
            {
                var param = Params.Input[idx];

                RuleAttribute attribute = attributeList.Find(x => x.mFullName == param.Name);

                bool expectArray = attribute != null && attribute.IsArray();

                SetAttributeOfShapes(DA, shapeCount, param, expectArray);
            }
        }

        protected override void AppendAdditionalComponentMenuItems(ToolStripDropDown menu)
        {
            base.AppendAdditionalComponentMenuItems(menu);

            Menu_AppendItem(menu, "Generate Materials", OnMaterialToggleClicked, true, mDoGenerateMaterials);
            Menu_AppendSeparator(menu);
            Menu_AppendItem(menu, "Go to CityEngine Resources", (object sender, EventArgs e) => { Process.Start(CITYENGINE_RESOURCES_URL); });
        }

        private void OnMaterialToggleClicked(object sender, EventArgs e)
        {
            mDoGenerateMaterials = !mDoGenerateMaterials;
            PRTWrapper.SetMaterialGenerationOption(mDoGenerateMaterials);

            ExpireSolution(true);
        }

        private void OutputGeometry(IGH_DataAccess dataAccess, List<Mesh[]> generatedMeshes)
        {
            var meshStructure = PRTWrapper.CreateMeshStructure(generatedMeshes);
            dataAccess.SetDataTree((int)OutputParams.MODELS, meshStructure);
        }

        private void OutputMaterials(IGH_DataAccess dataAccess, List<Mesh[]> generatedMeshes)
        {
            if (!mDoGenerateMaterials)
                return;

            GH_Structure<GH_Material> materials = PRTWrapper.GetAllMaterialIds(generatedMeshes);
            dataAccess.SetDataTree((int)OutputParams.MATERIALS, materials);
        }

        private void OutputReports(IGH_DataAccess dataAccess, List<Mesh[]> generatedMeshes)
        {
            GH_Structure<ReportAttribute> outputTree = new GH_Structure<ReportAttribute>();

            for (int shapeId = 0; shapeId < generatedMeshes.Count; shapeId++)
            {
                var reports = PRTWrapper.GetAllReports(shapeId);
                if (reports.Count > 0)
                {
                    GH_Path path = new GH_Path(shapeId);
                    reports.ForEach(x => outputTree.Append(x, path));
                }
            }

            dataAccess.SetDataTree((int)OutputParams.REPORTS, outputTree);
        }

        private void OutputCGAPrint(IGH_DataAccess dataAccess, List<Mesh[]> generatedMeshes)
        {
            var outputTree = new GH_Structure<GH_String>();

            for (int shapeId = 0; shapeId < generatedMeshes.Count; shapeId++)
            {
                List<String> printOutput = PRTWrapper.GetCGAPrintOutput(shapeId);
                if (printOutput.Count > 0)
                {
                    var shapePath = new GH_Path(shapeId);
                    printOutput.ForEach(o => outputTree.Append(new GH_String(o), shapePath));
                }
            }

            dataAccess.SetDataTree((int)OutputParams.PRINTS, outputTree);
        }

        private void OutputCGAErrors(IGH_DataAccess dataAccess, List<Mesh[]> generatedMeshes)
        {
            var outputTree = new GH_Structure<GH_String>();

            for (int shapeId = 0; shapeId < generatedMeshes.Count; shapeId++)
            {
                List<String> errorOutput = PRTWrapper.GetCGAErrorOutput(shapeId);
                if (errorOutput.Count > 0)
                {
                    var shapePath = new GH_Path(shapeId);
                    errorOutput.ForEach(o => outputTree.Append(new GH_String(o), shapePath));
                }
            }

            dataAccess.SetDataTree((int)OutputParams.ERRORS, outputTree);
        }

        private Mesh ConvertToMesh(IGH_GeometricGoo shape)
        {
            Mesh mesh = null;

            if (shape is GH_Brep)
            {
                Brep brepShape = null;
                if (!GH_Convert.ToBrep(shape, ref brepShape, GH_Conversion.Both))
                    return null;

                mesh = new Mesh();
                mesh.Append(Mesh.CreateFromBrep(brepShape, MeshingParameters.FastRenderMesh));
                mesh.Compact();
            }
            else if (shape is GH_Surface)
            {
                Surface surf = null;
                if (!GH_Convert.ToSurface(shape, ref surf, GH_Conversion.Both))
                    return null;
                mesh = Mesh.CreateFromSurface(surf, MeshingParameters.FastRenderMesh);
            }
            else
            {
                if (!GH_Convert.ToMesh(shape, ref mesh, GH_Conversion.Both))
                    return null;
            }

            mesh.Vertices.UseDoublePrecisionVertices = true;

            return mesh;
        }

        private void SetAttributeOfShapes(IGH_DataAccess DA, int shapeCount, IGH_Param attributeParam, bool expectArray)
        {
            if (attributeParam.Type == typeof(GH_Number))
            {
                if (DA.GetDataTree(attributeParam.Name, out GH_Structure<GH_Number> tree))
                    ExtractTreeValues(tree, attributeParam, shapeCount, expectArray);
            }
            else if (attributeParam.Type == typeof(GH_Integer))
            {
                if (DA.GetDataTree(attributeParam.Name, out GH_Structure<GH_Integer> tree))
                    ExtractTreeValues(tree, attributeParam, shapeCount, expectArray);
            }
            else if (attributeParam.Type == typeof(GH_Boolean))
            {
                if (DA.GetDataTree(attributeParam.Name, out GH_Structure<GH_Boolean> tree))
                    ExtractTreeValues(tree, attributeParam, shapeCount, expectArray);
            }
            else if (attributeParam.Type == typeof(GH_String))
            {
                if (DA.GetDataTree(attributeParam.Name, out GH_Structure<GH_String> tree))
                    ExtractTreeValues(tree, attributeParam, shapeCount, expectArray);
            }
            else if (attributeParam.Type == typeof(GH_Colour))
            {
                if (DA.GetDataTree(attributeParam.Name, out GH_Structure<GH_Colour> tree))
                    ExtractTreeValues(tree, attributeParam, shapeCount, expectArray);
            }
        }

        private void ExtractTreeValues<T>(GH_Structure<T> tree, IGH_Param attributeParam, int shapeCount, bool expectArray) where T : IGH_Goo
        {
            if (tree.IsEmpty)
                return;

            int shapeId = 0;

            if (expectArray)
            {
                foreach (List<T> branch in tree.Branches)
                {
                    if (shapeId >= shapeCount) return;

                    SetRuleAttributeArray(shapeId, attributeParam, branch);
                    shapeId++;
                }

                // Grasshopper behaviour: repeat last item to compensate mismatched list lengths
                while(shapeId < shapeCount)
                {
                    SetRuleAttributeArray(shapeId, attributeParam, tree.Branches.Last());
                    shapeId++;
                }
            }
            else
            {
                List<T> values = tree.ToList();

                // Grasshopper behaviour: repeat last item to compensate mismatched list lengths
                while (shapeCount > values.Count)
                {
                    values.Add(values.Last());
                }

                foreach (T value in values)
                {
                    if (value.IsValid)
                    {
                        if (shapeId >= shapeCount) return;

                        SetRuleAttribute(shapeId, attributeParam, value);
                    }
                    shapeId++;
                }
            }
        }

        private void SetRuleAttributeArray<T>(int shapeId, IGH_Param attributeParam, List<T> values) where T : IGH_Goo
        {
            if (attributeParam.Type == typeof(GH_Number))
            {
                List<double> doubleList = values.ConvertAll((x) => { x.CastTo<double>(out double d); return d; });
                PRTWrapper.SetRuleAttributeDoubleArray(shapeId, attributeParam.Name, doubleList);
            }
            else if (attributeParam.Type == typeof(GH_Integer))
            {
                AddRuntimeMessage(GH_RuntimeMessageLevel.Warning, "Integer arrays are not supported as input parameters, please use Number arrays.");
            }
            else if (attributeParam.Type == typeof(GH_Boolean))
            {
                List<bool> boolList = values.ConvertAll((x) => { x.CastTo<bool>(out bool b); return b; });
                PRTWrapper.SetRuleAttributeBoolArray(shapeId, attributeParam.Name, boolList);
            }
            else if (attributeParam.Type == typeof(GH_String))
            {
                List<string> stringList = values.ConvertAll((x) => { x.CastTo<string>(out string s); return s; });
                PRTWrapper.SetRuleAttributeStringArray(shapeId, attributeParam.Name, stringList);
            }
        }

        private void SetRuleAttribute<T>(int shapeId, IGH_Param attributeParam, T value) where T : IGH_Goo
        {
            var name = attributeParam.Name;
            if (attributeParam.Type == typeof(GH_Number))
            {
                if (value.CastTo(out double number))
                    PRTWrapper.SetRuleAttributeDouble(shapeId, name, number);
                else
                    throw new Exception(Utils.GetCastErrorMessage(name, "double"));
            }
            else if (attributeParam.Type == typeof(GH_Integer))
            {
                if (value.CastTo(out int integer))
                    PRTWrapper.SetRuleAttributeInteger(shapeId, name, integer);
                else
                    throw new Exception(Utils.GetCastErrorMessage(name, "integer"));
            }
            else if (attributeParam.Type == typeof(GH_Boolean))
            {
                if (value.CastTo(out bool boolean))
                    PRTWrapper.SetRuleAttributeBoolean(shapeId, name, boolean);
                else
                    throw new Exception(Utils.GetCastErrorMessage(name, "bool"));
            }
            else if (attributeParam.Type == typeof(GH_String))
            {
                if (value.CastTo(out string text))
                    PRTWrapper.SetRuleAttributeString(shapeId, name, text);
                else
                    throw new Exception(Utils.GetCastErrorMessage(name, "string"));
            }
            else if (attributeParam.Type == typeof(GH_Colour))
            {
                if (value.CastTo(out Color color))
                {
                    string text = Utils.HexColor(color);
                    PRTWrapper.SetRuleAttributeString(shapeId, name, text);
                }
                else
                    throw new Exception(Utils.GetCastErrorMessage(name, "Colour"));
            }
        }

        private List<RuleAttribute> GetEligibleAttributes()
        {
            var eligibleAttributes = new List<RuleAttribute>();
            foreach (var attr in mRuleAttributes)
            {
                if (!Params.Input.Exists(x => x.NickName == attr.mNickname))
                {
                    eligibleAttributes.Add(attr);
                }
            }
            return eligibleAttributes;
        }

        public bool CanInsertParameter(GH_ParameterSide side, int index)
        {
            if ((side != GH_ParameterSide.Input) || (index <= 2))
                return false;

            List<RuleAttribute> eligibleAttributes = GetEligibleAttributes();
            return (eligibleAttributes.Count > 0);
        }

        public bool CanRemoveParameter(GH_ParameterSide side, int index)
        {
            return (side == GH_ParameterSide.Input) && (index > 2);
        }

        public IGH_Param CreateParameter(GH_ParameterSide side, int index)
        {
            Debug.Assert(mRuleAttributes.Length > 0); // ensured by CanInsertParameter

            List<RuleAttribute> eligibleAttributes = GetEligibleAttributes();
            var attributeNames = new List<string>();
            foreach (var attr in eligibleAttributes)
                attributeNames.Add(attr.mNickname);

            var form = new AttributeForm(attributeNames, new System.Drawing.Point(Cursor.Position.X, Cursor.Position.Y));
            var result = form.ShowDialog();
            if (result == DialogResult.OK)
            {
                if (form.SelectedIndex >= 0 && form.SelectedIndex < mRuleAttributes.Length)
                {
                    var attr = eligibleAttributes[form.SelectedIndex];
                    return attr.CreateInputParameter();
                }
            }

            return null;
        }

        public bool DestroyParameter(GH_ParameterSide side, int index)
        {
            return true;
        }

        public void VariableParameterMaintenance()
        {
            return;
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Resources.gh_prt_main_component;
            }
        }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.ComponentPumaGuid; }
        }
    }
}
