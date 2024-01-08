/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2023 Esri R&D Center Zurich
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
using System.Linq;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Types;
using PumaGrasshopper.Properties;
using Rhino.Geometry;

namespace PumaGrasshopper
{
    
    public class ComponentPumaBulkInput : ComponentPumaShared
    {
        const string BULK_INPUT_NAME = "Rule attributes";
        const string BULK_INPUT_NICK_NAME = "Attributes";
        const string BULK_INPUT_DESC = "Attributes passed to the rule. Expects a tree of key:value pairs.";
        enum InputParams
        {
            RPK,
            SHAPES,
            SEEDS,
            BULK,
        }

        static readonly ParameterDescriptor[] INPUT_PARAM_DESC = new ParameterDescriptor[]{
            new ParameterDescriptor{ type = ParamType.FILEPATH, name = RPK_INPUT_NAME, nickName = RPK_INPUT_NICK_NAME, desc = RPK_INPUT_DESC },
            new ParameterDescriptor{ type = ParamType.GEOMETRY, name = GEOM_INPUT_NAME, nickName = GEOM_INPUT_NICK_NAME, desc = GEOM_INPUT_DESC },
            new ParameterDescriptor{ type = ParamType.INTEGER, name = SEED_KEY, nickName = SEED_INPUT_NAME, desc = SEED_INPUT_DESC },
            new ParameterDescriptor{ type = ParamType.GENERIC, name = BULK_INPUT_NAME, nickName = BULK_INPUT_NICK_NAME, desc = BULK_INPUT_DESC},
        };

        public ComponentPumaBulkInput()
          : base(COMPONENT_NAME, COMPONENT_NICK_NAME) { }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            foreach( var param in Enum.GetValues(typeof(InputParams)).Cast<InputParams>())
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
                    case ParamType.GENERIC:
                        pManager.AddGenericParameter(desc.name, desc.nickName, desc.desc, GH_ParamAccess.tree);
                        pManager[3].Optional = true;
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

            List<Mesh> inputMeshes = CreateInputMeshes(DA);
            if (inputMeshes == null || inputMeshes.Count == 0)
                return;

            if (mCurrentRpk == null || !mCurrentRpk.IsSame(rpk))
            {
                mCurrentRpk = rpk;
                mRuleAttributes = PRTWrapper.GetRuleAttributes(rpk.path);
            }

            RuleAttributesMap MM = ParseBulkInputTree(DA, inputMeshes.Count);

            var generatedMeshes = PRTWrapper.Generate(rpk.path, ref MM, inputMeshes);
            OutputGeometry(DA, generatedMeshes.meshes);
            OutputMaterials(DA, generatedMeshes.materials);
            OutputReports(DA, generatedMeshes.reports);
            OutputCGAPrint(DA, generatedMeshes.prints);
            OutputCGAErrors(DA, generatedMeshes.errors);
        }

        private RuleAttributesMap ParseBulkInputTree(IGH_DataAccess DA, int shapeCount)
        {
            RuleAttributesMap MM = new RuleAttributesMap();

            GH_Structure<IGH_Goo> inputTree;
            if (!DA.GetDataTree(BULK_INPUT_NAME, out inputTree))
                return null;

            List<RuleAttribute> attributesList = mRuleAttributes.ToList();

            for(int shapeId = 0; shapeId < shapeCount; ++shapeId)
            {
                MM.StartNewSection();

                if(shapeId < inputTree.PathCount)
                {
                    List<IGH_Goo> branch = (List<IGH_Goo>)inputTree.get_Branch(shapeId);

                    branch.ForEach((input) => {
                        var pair = Utils.ParseInputPair(input);
                        var attribute = attributesList.Find(attr => attr.mFullName.Equals(pair.Key));

                        if (attribute == null) return;

                        if (attribute.IsArray())
                        {
                            SetRuleAttributeArray(attribute, (object[])pair.Value, ref MM);
                        }
                        else
                        {
                            SetRuleAttribute(attribute, pair.Value, ref MM);
                        }
                    });
                } 
            }

            return MM;
        }

        private void SetRuleAttribute(RuleAttribute attribute, object value, ref RuleAttributesMap MM)
        {
            var name = attribute.mFullName;
            switch(attribute.mAttribType)
            {
                case Annotations.AnnotationArgumentType.AAT_BOOL: 
                    if(value is string)
                        MM.AddBoolean(name, Boolean.Parse((string)value));
                    else if(value is bool)
                        MM.AddBoolean(name, (bool)value);
                    else
                        throw new Exception(Utils.GetCastErrorMessage(name, "boolean"));
                    break;
                case Annotations.AnnotationArgumentType.AAT_INT:
                    if (value is int)
                        MM.AddInteger(name, (int)value);
                    else if (value is string)
                        MM.AddInteger(name, int.Parse((string)value));
                    else
                        throw new Exception(Utils.GetCastErrorMessage(name, "integer"));
                    break;
                case Annotations.AnnotationArgumentType.AAT_FLOAT:
                    if (value is double)
                        MM.AddDouble(name, (double)value);
                    else if (value is string)
                        MM.AddDouble(name, double.Parse((string)value));
                    else
                        throw new Exception(Utils.GetCastErrorMessage(name, "double"));
                    break;
                case Annotations.AnnotationArgumentType.AAT_STR:
                    if(value is string)
                        MM.AddString(name, (string)value);
                    else
                        throw new Exception(Utils.GetCastErrorMessage(name, "string"));
                    break;
                default:
                    return;
            }
        }

        private void SetRuleAttributeArray(RuleAttribute attribute, object value, ref RuleAttributesMap MM)
        {
            var name = attribute.mFullName;

            switch(attribute.mAttribType)
            {
                case Annotations.AnnotationArgumentType.AAT_BOOL_ARRAY:
                    if(value is Array)
                    {
                        bool[] boolList = Array.ConvertAll<object, bool>((object[])value, (x) =>
                        {
                            if (x is bool || x is int) return (bool)x;
                            if (x is string) return Boolean.Parse((string)x);
                            return false;
                        });
                        MM.AddBoolArray(name, boolList);
                    } else if(value is string)
                    {
                        bool[] boolList = Utils.BoolFromCeArray((string)value);
                        MM.AddBoolArray(name, boolList);
                    } else
                    {
                        throw new Exception(Utils.GetCastErrorMessage(name, "bool array"));
                    }
                    break;
                case Annotations.AnnotationArgumentType.AAT_FLOAT_ARRAY:
                    if (value is Array)
                    {
                        double[] doubleList = Array.ConvertAll<object, double>((object[])value, (x) =>
                        {
                            if (x is float || x is double) return (double)x;
                            if (x is string) return Double.Parse((string)x);
                            return 0;
                        });
                        MM.AddDoubleArray(name, doubleList);
                    }
                    else if (value is string)
                    {
                        double[] doubleList = Utils.DoubleFromCeArray((string)value);
                        MM.AddDoubleArray(name, doubleList);
                    }
                    else
                    {
                        throw new Exception(Utils.GetCastErrorMessage(name, "double array"));
                    }
                    break;
                case Annotations.AnnotationArgumentType.AAT_STR_ARRAY:
                    // TODO
                case Annotations.AnnotationArgumentType.AAT_INT_ARRAY:
                    // TODO
                default:
                    return;
            }
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
            get { return PumaUIDs.ComponentPumaBulkInputGuid; }
        }
    }
}