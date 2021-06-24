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
using System.Runtime.InteropServices;
using System.Text;
using System.Drawing;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using Rhino.DocObjects;

using Rhino.Runtime.InteropWrappers;
using Rhino.Display;

namespace PumaGrasshopper
{
    /// <summary>
    /// Encapsulate PumaRhino library.
    /// </summary>
    class PRTWrapper
    {
        public static String INIT_SHAPE_IDX_KEY = "InitShapeIdx";

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void GetProductVersion([In, Out]IntPtr version_Str);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InitializeRhinoPRT();

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetPackage(string rpk_path);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool AddInitialMesh([In]IntPtr pMesh);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ClearInitialShapes();

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Generate();

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetMeshBundle(int initialShapeIndex, [In, Out]IntPtr pMeshArray);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetAllMeshIDs([In, Out]IntPtr pMeshIDs);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetMeshPartCount(int initialShapeIndex);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetRuleAttributesCount();

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRuleAttribute(int attrIdx, [In, Out]IntPtr pRule, [In, Out]IntPtr pName, [In, Out]IntPtr pNickname, ref Annotations.AnnotationArgumentType type, [In,Out]IntPtr pGroup);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeDouble(int initialShapeIndex, string fullName, double value);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeBoolean(int initialShapeIndex, string fullName, bool value);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeInteger(int initialShapeIndex, string fullName, int value);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeString(int initialShapeIndex, string fullName, string value);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeDoubleArray(int initialShapeIndex, string fullName, [In, Out]IntPtr pValueArray);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeBoolArray(int initialShapeIndex, string fullName, [In, Out]IntPtr pValueArray);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeStringArray(int initialShapeIndex, string fullName, [In, Out]IntPtr pValueArray);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetAnnotationTypes(int ruleIdx, [In, Out]IntPtr pAnnotTypeArray);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetEnumType(int ruleIdx, int enumIdx, ref Annotations.EnumAnnotationType type);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetAnnotationEnumDouble(int ruleIdx, int enumIdx, [In,Out]IntPtr pArray, ref bool restricted);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetAnnotationEnumString(int ruleIdx, int enumIdx, [In,Out]IntPtr pArray, ref bool restricted);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetAnnotationRange(int ruleIdx, int enumIdx, ref double min, ref double max, ref double stepsize, ref bool restricted);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetReports(int initialShapeIndex, [In, Out] IntPtr pKeysArray,
        [In, Out] IntPtr pDoubleReports,
        [In, Out] IntPtr pBoolReports,
        [In, Out] IntPtr pStringReports);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetMaterial(int initialShapeIndex, int shapeID, ref int pUvSet,
                                                [In, Out] IntPtr pTexKeys,
                                                [In, Out] IntPtr pTexPaths,
                                                [In, Out] IntPtr pDiffuseColor,
                                                [In, Out] IntPtr pAmbientColor,
                                                [In, Out] IntPtr pSpecularColor,
                                                ref double opacity,
                                                ref double shininess);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetMaterialGenerationOption(bool doGenerate);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValueBoolean(string key, ref bool value);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValueNumber(string key, ref double value);

        [DllImport(dllName: "PumaRhino.rhp", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValueText(string key, [In, Out] IntPtr pText);

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

        public static GH_Structure<GH_Mesh> GenerateMesh()
        {
            bool status = Generate();
            if (!status) return null;

            // GH_Structure is the data tree outputed by our component, it takes only GH_Mesh (which is a grasshopper wrapper class over the rhino Mesh), 
            // thus a conversion is necessary when adding Meshes.
            GH_Structure<GH_Mesh> mesh_struct = new GH_Structure<GH_Mesh>();

            // retrieve the meshBundle for each initial shape id
            int[] mesh_ids = null;
            using(var idsArray = new SimpleArrayInt())
            {
                var pIdsArray = idsArray.NonConstPointer();
                GetAllMeshIDs(pIdsArray);
                mesh_ids = idsArray.ToArray();
            }

            foreach(int id in mesh_ids)
            {
                using(var arr = new SimpleArrayMeshPointer())
                {
                    var ptr_array = arr.NonConstPointer();

                    status = GetMeshBundle(id, ptr_array);

                    if(status)
                    {
                        var meshBundle = arr.ToNonConstArray();

                        foreach(var mesh in meshBundle)
                        {
                            // Directly convert to to GH_Mesh and add it to the GH_Structure at the branch corresponding to the mesh id
                            GH_Mesh gh_mesh = null;
                            status = GH_Convert.ToGHMesh(mesh, GH_Conversion.Both, ref gh_mesh);

                            if (status)
                            {
                                GH_Path path = new GH_Path(id);
                                mesh_struct.Append(gh_mesh, path);
                            }
                        }
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

                Uri fileuri = new Uri(texPath);

                Texture tex = new Texture
                {
                    FileReference = Rhino.FileIO.FileReference.CreateFromFullPath(fileuri.AbsolutePath),
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

        public static GH_Structure<GH_Material> GetAllMaterialIds(int meshCount)
        {
            int[] initShapeIds = null;
            using(var meshIdsArray = new SimpleArrayInt())
            {
                var pMeshIDArray = meshIdsArray.NonConstPointer();

                GetAllMeshIDs(pMeshIDArray);
                initShapeIds = meshIdsArray.ToArray();
            }

            GH_Structure<GH_Material> material_struct = new GH_Structure<GH_Material>();

            foreach(int initShapeId in initShapeIds)
            {
                var mats = GetMaterialsOfMesh(initShapeId);

                GH_Path path = new GH_Path(initShapeId);

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
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeIndex, keysArray[kId], keysArray[kId], ReportTypes.PT_FLOAT, doubleReportsArray[i]));
                kId++;
            }
            for(int i = 0; i < boolReportsArray.Length; ++i)
            {
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeIndex, keysArray[kId], keysArray[kId], ReportTypes.PT_BOOL, Convert.ToBoolean(boolReportsArray[i])));
                kId++;    
            }
            for(int i = 0; i < stringReportsArray.Length; ++i)
            {
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeIndex, keysArray[kId], keysArray[kId], ReportTypes.PT_STRING, stringReportsArray[i]));
                kId++;
            }

            return ras;
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
                                annots.Add(new Annotations.Enum<bool>(boolArray, true));
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

            return new Annotations.Enum<double>(enumArray, restricted);
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

            return new Annotations.Enum<string>(enumArray, restricted);
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
    }
}
