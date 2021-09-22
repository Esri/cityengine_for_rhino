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
using System.Runtime.InteropServices;
using System.Drawing;

using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using Rhino.DocObjects;

using Rhino.Runtime.InteropWrappers;
using System.Linq;

namespace PumaGrasshopper
{
    /// <summary>
    /// Encapsulate PumaRhino library.
    /// </summary>
    class PRTWrapper
    {
        public static String INIT_SHAPE_IDX_KEY = "InitShapeIdx";
        private const String PUMA_RHINO_LIBRARY = "PumaRhino.rhp";

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void GetProductVersion([In, Out]IntPtr version_Str);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InitializeRhinoPRT();

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetPackage(string rpk_path, [In, Out] IntPtr errorMsg);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool AddInitialMesh([In]IntPtr pMesh);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ClearInitialShapes();

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Generate([In, Out] IntPtr pMeshCounts, [In, Out] IntPtr pMeshArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetMeshBundle(int initialShapeIndex, [In, Out]IntPtr pMeshArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetMeshPartCount(int initialShapeIndex);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetRuleAttributesCount();

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRuleAttribute(int attrIdx, [In, Out]IntPtr pRule, [In, Out]IntPtr pName, [In, Out]IntPtr pNickname, ref Annotations.AnnotationArgumentType type, [In,Out]IntPtr pGroup);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeDouble(int initialShapeIndex, string fullName, double value);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeBoolean(int initialShapeIndex, string fullName, bool value);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeInteger(int initialShapeIndex, string fullName, int value);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeString(int initialShapeIndex, string fullName, string value);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeDoubleArray(int initialShapeIndex, string fullName, [In, Out]IntPtr pValueArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeBoolArray(int initialShapeIndex, string fullName, [In, Out]IntPtr pValueArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeStringArray(int initialShapeIndex, string fullName, [In, Out]IntPtr pValueArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetAnnotationTypes(int ruleIdx, [In, Out]IntPtr pAnnotTypeArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetEnumType(int ruleIdx, int enumIdx, ref Annotations.EnumAnnotationType type);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetAnnotationEnumDouble(int ruleIdx, int enumIdx, [In,Out]IntPtr pArray, ref bool restricted);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetAnnotationEnumString(int ruleIdx, int enumIdx, [In,Out]IntPtr pArray, ref bool restricted);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetAnnotationRange(int ruleIdx, int enumIdx, ref double min, ref double max, ref double stepsize, ref bool restricted);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetReports(int initialShapeIndex, [In, Out] IntPtr pKeysArray,
        [In, Out] IntPtr pDoubleReports,
        [In, Out] IntPtr pBoolReports,
        [In, Out] IntPtr pStringReports);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetCGAPrintOutput(int initialShapeIndex, [In, Out] IntPtr pPrintOutput);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetCGAErrorOutput(int initialShapeIndex, [In, Out] IntPtr pErrorOutput);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetMaterial(int initialShapeIndex, int shapeID, ref int pUvSet,
                                                [In, Out] IntPtr pTexKeys,
                                                [In, Out] IntPtr pTexPaths,
                                                [In, Out] IntPtr pDiffuseColor,
                                                [In, Out] IntPtr pAmbientColor,
                                                [In, Out] IntPtr pSpecularColor,
                                                ref double opacity,
                                                ref double shininess);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetMaterialGenerationOption(bool doGenerate);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesBoolean(string key, [In, Out] IntPtr pValues);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesNumber(string key, [In, Out] IntPtr pValues);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesText(string key, [In, Out] IntPtr pTexts);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesBooleanArray(string key, [In, Out] IntPtr pValues, [In, Out] IntPtr pSizes);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesNumberArray(string key, [In, Out] IntPtr pValues, [In, Out] IntPtr pSizes);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesTextArray(string key, [In, Out] IntPtr pTexts, [In, Out] IntPtr pSizes);

        public static bool AddMesh(List<Mesh> meshes)
        {
            bool status;

            using (var arr = new SimpleArrayMeshPointer())
            {
                foreach (var mesh in meshes)
                {
                    arr.Add(mesh, true);
                }

                var ptr_array = arr.ConstPointer();
                status = AddInitialMesh(ptr_array);
            }

            return status;
        }

        public static List<Mesh[]> GenerateMesh()
        {
            var meshCounts = new SimpleArrayInt();
            var pMeshCounts = meshCounts.NonConstPointer();

            var meshes = new SimpleArrayMeshPointer();
            var pMeshes = meshes.NonConstPointer();

            Generate(pMeshCounts, pMeshes);

            var meshCountsArray = meshCounts.ToArray();
            var meshesArray = meshes.ToNonConstArray();
            var generatedMeshes = new List<Mesh[]>();
            int indexOffset = 0;
            for (int id = 0; id < meshCountsArray.Length; id++)
            {
                if (meshCountsArray[id] > 0)
                {
                    var meshesForShape = meshesArray.Skip(indexOffset).Take(meshCountsArray[id]).ToArray();
                    generatedMeshes.Add(meshesForShape);
                }
                else
                    generatedMeshes.Add(null);

                indexOffset += meshCountsArray[id];
            }

            return generatedMeshes;
        }

        public static GH_Structure<GH_Mesh> CreateMeshStructure(List<Mesh[]> generatedMeshes)
        {
            // GH_Structure is the data tree outputed by our component, it takes only GH_Mesh (which is a grasshopper wrapper class over the rhino Mesh), 
            // thus a conversion is necessary when adding Meshes.
            GH_Structure<GH_Mesh> mesh_struct = new GH_Structure<GH_Mesh>();

            for (int shapeId = 0; shapeId < generatedMeshes.Count; shapeId++)
            {
                if (generatedMeshes[shapeId] == null)
                    continue;

                GH_Path path = new GH_Path(shapeId);
                var meshBundle = generatedMeshes[shapeId];
                foreach (var mesh in meshBundle)
                {
                    GH_Mesh gh_mesh = null;
                    var status = GH_Convert.ToGHMesh(mesh, GH_Conversion.Both, ref gh_mesh);
                    if (status)
                    {
                        mesh_struct.Append(gh_mesh, path);
                    }
                }
            }

            return mesh_struct;
        }

        public static GH_Material GetMaterialOfPartMesh(int initialShapeIndex, int meshID)
        {
            int uvSet = 0;

            ClassArrayString texKeys = new ClassArrayString();
            var pTexKeys = texKeys.NonConstPointer();

            ClassArrayString texPaths = new ClassArrayString();
            var pTexPaths = texPaths.NonConstPointer();

            SimpleArrayInt diffuseArray = new SimpleArrayInt();
            var pDiffuseArray = diffuseArray.NonConstPointer();

            SimpleArrayInt ambientArray = new SimpleArrayInt();
            var pAmbientArray = ambientArray.NonConstPointer();

            SimpleArrayInt specularArray = new SimpleArrayInt();
            var pSpecularArray = specularArray.NonConstPointer();

            double opacity = 1;
            double shininess = 1;

            bool status = PRTWrapper.GetMaterial(initialShapeIndex, meshID, ref uvSet, pTexKeys, pTexPaths, pDiffuseArray, pAmbientArray, pSpecularArray, ref opacity, ref shininess);
            if (!status) return null;

            var texKeysArray = texKeys.ToArray();
            var texPathsArray = texPaths.ToArray();

            string coloMap = "";

            Material mat = new Material();
            
            for (int i = 0; i < texKeysArray.Length; ++i)
            {
                string texKey = texKeysArray[i];
                string texPath = texPathsArray[i];

                Texture tex = new Texture
                {
                    FileReference = Rhino.FileIO.FileReference.CreateFromFullPath(texPath),
                    TextureCombineMode = TextureCombineMode.Modulate,
                    TextureType = TextureType.Bitmap
                };

                switch (texKey)
                {
                    case "diffuseMap":
                    case "colorMap":
                        mat.SetBitmapTexture(tex);
                        coloMap = tex.FileReference.FullPath;
                        break;
                    case "opacityMap":
                        // if the color and opacity textures are the same, no need to set it again, it is only needed to activate alpha transparency.
                        if(coloMap != tex.FileReference.FullPath)
                        {
                            tex.TextureCombineMode = TextureCombineMode.Modulate;
                            tex.TextureType = TextureType.Transparency;
                            mat.SetTransparencyTexture(tex);
                        }
                        
                        mat.AlphaTransparency = true;
                        
                        break;
                    case "bumpMap":
                        tex.TextureCombineMode = TextureCombineMode.None;
                        tex.TextureType = TextureType.Bump;
                        mat.SetBumpTexture(tex);
                        break;
                    default:
                        break;
                }
            }

            var diffuseColor = diffuseArray.ToArray();
            var ambientColor = ambientArray.ToArray();
            var specularColor = specularArray.ToArray();
            diffuseArray.Dispose();
            ambientArray.Dispose();
            specularArray.Dispose();

            if(diffuseColor.Length == 3)
            {
                mat.DiffuseColor = Color.FromArgb(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
            }

            if(ambientColor.Length == 3)
            {
                mat.AmbientColor = Color.FromArgb(ambientColor[0], ambientColor[1], ambientColor[2]);
            }

            if(specularColor.Length == 3)
            {
                mat.SpecularColor = Color.FromArgb(specularColor[0], specularColor[1], specularColor[2]);
            }

            mat.Transparency = 1.0 - opacity;
            mat.Shine = shininess;

            mat.FresnelReflections = true;

            mat.CommitChanges();

            var renderMat = Rhino.Render.RenderMaterial.CreateBasicMaterial(mat);

            return new GH_Material(renderMat);
        }

        public static List<GH_Material> GetMaterialsOfMesh(int initialShapeIndex)
        {
            List<GH_Material> materials = new List<GH_Material>();

            int meshCount = GetMeshPartCount(initialShapeIndex);

            for(int i = 0; i < meshCount; ++i)
            {
                materials.Add(GetMaterialOfPartMesh(initialShapeIndex, i));
            }

            return materials;
        }

        public static GH_Structure<GH_Material> GetAllMaterialIds(List<Mesh[]> generatedMeshes)
        {
            GH_Structure<GH_Material> material_struct = new GH_Structure<GH_Material>();

            for (int shapeId = 0; shapeId < generatedMeshes.Count; shapeId++)
            {
                if (generatedMeshes[shapeId] == null)
                    continue;
                var mats = GetMaterialsOfMesh(shapeId);
                GH_Path path = new GH_Path(shapeId);
                material_struct.AppendRange(mats, path);
            }

            return material_struct;
        }

        public static List<ReportAttribute> GetAllReports(int initialShapeIndex)
        {
            var keys = new ClassArrayString();
            var stringReports = new ClassArrayString();
            var doubleReports = new SimpleArrayDouble();
            var boolReports = new SimpleArrayInt();

            var pKeys = keys.NonConstPointer();
            var pStrReps = stringReports.NonConstPointer();
            var pDblReps = doubleReports.NonConstPointer();
            var pBoolReps = boolReports.NonConstPointer();

            GetReports(initialShapeIndex, pKeys, pDblReps, pBoolReps, pStrReps);

            var keysArray = keys.ToArray();
            var stringReportsArray = stringReports.ToArray();
            var doubleReportsArray = doubleReports.ToArray();
            var boolReportsArray = boolReports.ToArray();

            keys.Dispose();
            stringReports.Dispose();
            doubleReports.Dispose();
            boolReports.Dispose();

            if (keysArray.Length != stringReportsArray.Length + doubleReportsArray.Length + boolReportsArray.Length)
            {
                // Something went wrong, don't output anything.
                return null;
            }

            List<ReportAttribute> ras = new List<ReportAttribute>(keysArray.Length);
            int kId = 0;

            for(int i = 0; i < doubleReportsArray.Length; ++i)
            {
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeIndex, keysArray[kId], ReportTypes.PT_FLOAT, doubleReportsArray[i]));
                kId++;
            }
            for(int i = 0; i < boolReportsArray.Length; ++i)
            {
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeIndex, keysArray[kId], ReportTypes.PT_BOOL, Convert.ToBoolean(boolReportsArray[i])));
                kId++;    
            }
            for(int i = 0; i < stringReportsArray.Length; ++i)
            {
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeIndex, keysArray[kId], ReportTypes.PT_STRING, stringReportsArray[i]));
                kId++;
            }

            return ras;
        }

        public static List<String> GetCGAPrintOutput(int initialShapeIndex)
        {
            var printOutput = new ClassArrayString();
            var pPrintOutput = printOutput.NonConstPointer();

            GetCGAPrintOutput(initialShapeIndex, pPrintOutput);

            var printOutputArray = printOutput.ToArray();
            printOutput.Dispose();

            return new List<String>(printOutputArray);
        }

        public static List<String> GetCGAErrorOutput(int initialShapeIndex)
        {
            var errorOutput = new ClassArrayString();
            var pErrorOutput = errorOutput.NonConstPointer();

            GetCGAErrorOutput(initialShapeIndex, pErrorOutput);

            var errorOutputArray = errorOutput.ToArray();
            errorOutput.Dispose();

            return new List<String>(errorOutputArray);
        }

        public static RuleAttribute[] GetRuleAttributes()
        {
            int attribCount = GetRuleAttributesCount();

            if (attribCount == 0) return new RuleAttribute[0]{};

            RuleAttribute[] attributes = new RuleAttribute[attribCount];

            for(int i = 0; i < attribCount; ++i)
            {
                StringWrapper ruleBuilder = new StringWrapper();
                StringWrapper nameBuilder = new StringWrapper();
                StringWrapper nicknameBuilder = new StringWrapper();
                StringWrapper group = new StringWrapper();

                var pRuleBuilder = ruleBuilder.NonConstPointer;
                var pNameBuilder = nameBuilder.NonConstPointer;
                var pNickNameBuilder = nicknameBuilder.NonConstPointer;
                var pGroup = group.NonConstPointer;

                Annotations.AnnotationArgumentType type = Annotations.AnnotationArgumentType.AAT_INT;

                bool status = GetRuleAttribute(i, pRuleBuilder, pNameBuilder, pNickNameBuilder, ref type, pGroup);
                if (!status) return new RuleAttribute[0] {};

                attributes[i] = new RuleAttribute(nameBuilder.ToString(), nicknameBuilder.ToString(), ruleBuilder.ToString(), type, group.ToString());

                // get the potential annotations
                List<Annotations.Base> annotations = GetAnnotations(i);
                attributes[i].mAnnotations.AddRange(annotations);
            }
            
            return attributes;
        }

        public static String GetVersion()
        {
            string version;
            using (StringWrapper str = new StringWrapper())
            {
                var str_ptr = str.NonConstPointer;

                GetProductVersion(str_ptr);

                version = str.ToString();
            }

            return version;
        }

        public static List<Annotations.Base> GetAnnotations(int ruleIdx)
        {
            int[] intArray = null;
            using(var array = new SimpleArrayInt())
            {
                var pArray = array.NonConstPointer();

                PRTWrapper.GetAnnotationTypes(ruleIdx, pArray);

                intArray = array.ToArray();
            }

            var annots = new List<Annotations.Base>();

            for(int enumIdx = 0; enumIdx < intArray.Length; ++enumIdx)
            {
                Annotations.AttributeAnnotation type = (Annotations.AttributeAnnotation)intArray[enumIdx];
                switch (type)
                {
                    case Annotations.AttributeAnnotation.A_COLOR:
                        annots.Add(new Annotations.Color());
                        break;
                    case Annotations.AttributeAnnotation.A_ENUM:
                        Annotations.EnumAnnotationType enumType = Annotations.EnumAnnotationType.ENUM_DOUBLE;
                        bool status = GetEnumType(ruleIdx, enumIdx, ref enumType);
                        if (!status) break;

                        switch (enumType)
                        {
                            case Annotations.EnumAnnotationType.ENUM_BOOL:
                                bool[] boolArray = { true, false };
                                annots.Add(new Annotations.Enum<bool>(boolArray, enumType, true));
                                break;
                            case Annotations.EnumAnnotationType.ENUM_DOUBLE:
                                annots.Add(GetAnnotationEnumDouble(ruleIdx, enumIdx));
                                break;
                            case Annotations.EnumAnnotationType.ENUM_STRING:
                                annots.Add(GetAnnotationEnumString(ruleIdx, enumIdx));
                                break;
                            default:
                                break;
                        }
                        
                        break;
                    case Annotations.AttributeAnnotation.A_RANGE:
                        annots.Add(GetAnnotationRange(ruleIdx, enumIdx));
                        break;
                    case Annotations.AttributeAnnotation.A_DIR:
                        annots.Add(new Annotations.Directory());
                        break;
                    case Annotations.AttributeAnnotation.A_FILE:
                        annots.Add(new Annotations.File());
                        break;
                    default:
                        continue;
                }
            }

            return annots;
        }

        public static Annotations.Enum<double> GetAnnotationEnumDouble(int ruleIdx, int enumIdx)
        {
            double[] enumArray = null;
            bool restricted = true;
            using (var array = new SimpleArrayDouble())
            {
                var pArray = array.NonConstPointer();
                
                bool status = GetAnnotationEnumDouble(ruleIdx, enumIdx, pArray, ref restricted);
                if (!status) return null;

                enumArray = array.ToArray();
            }

            return new Annotations.Enum<double>(enumArray, Annotations.EnumAnnotationType.ENUM_DOUBLE, restricted);
        }

        public static Annotations.Enum<string> GetAnnotationEnumString(int ruleIdx, int enumIdx)
        {
            string[] enumArray = null;
            bool restricted = true;
            using (var array = new ClassArrayString())
            {
                var pArray = array.NonConstPointer();
                bool status = GetAnnotationEnumString(ruleIdx, enumIdx, pArray, ref restricted);
                if (!status) return null;

                enumArray = array.ToArray();
            }

            return new Annotations.Enum<string>(enumArray, Annotations.EnumAnnotationType.ENUM_STRING, restricted);
        }

        public static Annotations.Range GetAnnotationRange(int ruleIdx, int enumIdx)
        {
            double min = 0;
            double max = 0;
            double stepsize = 0;
            bool restricted = true;

            bool status = GetAnnotationRange(ruleIdx, enumIdx, ref min, ref max, ref stepsize, ref restricted);
            if (status)
            {
                return new Annotations.Range(min, max, stepsize, restricted);
            }
            return null;
        }

        public static void SetRuleAttributeDoubleArray(int initialShapeIndex, string fullName, List<double> doubleList)
        {
            if (doubleList.Count == 0) return;

            using (SimpleArrayDouble array = new SimpleArrayDouble(doubleList))
            {
                var pArray = array.ConstPointer();
                PRTWrapper.SetRuleAttributeDoubleArray(initialShapeIndex, fullName, pArray);
            }
        }

        public static void SetRuleAttributeBoolArray(int initialShapeIndex, string fullName, List<Boolean> boolList)
        {
            if (boolList.Count == 0) return;

            using(SimpleArrayInt array = new SimpleArrayInt(Array.ConvertAll<bool, int>(boolList.ToArray(), x => Convert.ToInt32(x))))
            {
                var pArray = array.ConstPointer();
                PRTWrapper.SetRuleAttributeBoolArray(initialShapeIndex, fullName, pArray);
            }
        }

        public static void SetRuleAttributeStringArray(int initialShapeIndex, string fullName, List<string> stringList)
        {
            if (stringList.Count == 0) return;

            using(ClassArrayString array = new ClassArrayString())
            {
                stringList.ForEach(x => array.Add(x));

                var pArray = array.ConstPointer();
                PRTWrapper.SetRuleAttributeStringArray(initialShapeIndex, fullName, pArray);
            }
        }

        public static List<bool> GetDefaultValuesBoolean(string key)
        {
            SimpleArrayInt boolArray = new SimpleArrayInt();
            var pBoolArray = boolArray.NonConstPointer();
            bool hasDefault = GetDefaultValuesBoolean(key, pBoolArray);

            if (!hasDefault) return null;

            List<bool> boolList = new List<int>(boolArray.ToArray()).ConvertAll(x => Convert.ToBoolean(x));
            boolArray.Dispose();

            return boolList;
        }

        public static List<double> GetDefaultValuesNumber(string key)
        {
            SimpleArrayDouble doubleArray = new SimpleArrayDouble();
            var pDoubleArray = doubleArray.NonConstPointer();
            bool hasDefault = GetDefaultValuesNumber(key, pDoubleArray);

            if (!hasDefault) return null;

            List<double> doubleList = new List<double>(doubleArray.ToArray());
            doubleArray.Dispose();

            return doubleList;
        }

        public static List<string> GetDefaultValuesText(string key)
        {
            ClassArrayString stringArray = new ClassArrayString();
            var pStringArray = stringArray.NonConstPointer();
            bool hasDefault = GetDefaultValuesText(key, pStringArray);

            if (!hasDefault) return null;

            List<string> stringList = new List<string>(stringArray.ToArray());
            stringArray.Dispose();

            return stringList;
        }

        public static List<List<bool>> GetDefaultValuesBooleanArray(string key)
        {
            // Use a single array to pass the data flattened,
            // and a second array of int to pass the size of the sub-array of each initial shape.
            SimpleArrayInt valueArray = new SimpleArrayInt();
            SimpleArrayInt sizeArray = new SimpleArrayInt();

            var pValueArray = valueArray.NonConstPointer();
            var pSizeArray = sizeArray.NonConstPointer();

            bool hasDefault = GetDefaultValuesBooleanArray(key, pValueArray, pSizeArray);

            if (!hasDefault) return null;

            List<bool> valueList = new List<int>(valueArray.ToArray()).ConvertAll(x => Convert.ToBoolean(x));
            List<int> sizeList = new List<int>(sizeArray.ToArray());

            valueArray.Dispose();
            sizeArray.Dispose();

            return Utils.UnflattenList(valueList, sizeList);
        }

        public static List<List<double>> GetDefaultValuesNumberArray(string key)
        {
            SimpleArrayDouble valueArray = new SimpleArrayDouble();
            SimpleArrayInt sizeArray = new SimpleArrayInt();

            var pValueArray = valueArray.NonConstPointer();
            var pSizeArray = sizeArray.NonConstPointer();

            bool hasDefault = GetDefaultValuesNumberArray(key, pValueArray, pSizeArray);

            if (!hasDefault) return null;

            List<double> valueList = new List<double>(valueArray.ToArray());
            List<int> sizeList = new List<int>(sizeArray.ToArray());

            valueArray.Dispose();
            sizeArray.Dispose();

            return Utils.UnflattenList(valueList, sizeList);
        }

        public static List<List<string>> GetDefaultValuesTextArray(string key)
        {
            ClassArrayString stringArray = new ClassArrayString();
            SimpleArrayInt sizeArray = new SimpleArrayInt();

            var pStringArray = stringArray.NonConstPointer();
            var pSizeArray = sizeArray.NonConstPointer();

            bool hasDefault = GetDefaultValuesTextArray(key, pStringArray, pSizeArray);

            if (!hasDefault) return null;

            List<string> stringList = new List<string>(stringArray.ToArray());
            List<int> sizeList = new List<int>(sizeArray.ToArray());

            stringArray.Dispose();
            sizeArray.Dispose();

            return Utils.UnflattenList(stringList, sizeList);
        }
  
    }
}
