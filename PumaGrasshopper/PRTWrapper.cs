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
    public class GenerationResult
    {
        public List<Mesh[]> meshes = new List<Mesh[]>();
        public List<GH_Material[]> materials = new List<GH_Material[]>();
        public List<ReportAttribute[]> reports = new List<ReportAttribute[]>();
    }

    /// <summary>
    /// Encapsulate PumaRhino library.
    /// </summary>
    public static class PRTWrapper
    {
        public static String INIT_SHAPE_IDX_KEY = "InitShapeIdx";
        private const String PUMA_RHINO_LIBRARY = "PumaRhino.rhp";

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void GetProductVersion([In, Out] IntPtr version_Str);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool InitializeRhinoPRT();

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int Generate(string rpk_path, [Out] IntPtr errorMsg,
            int shapeCount,
            [In] IntPtr pBoolStarts, int boolCount,
            [In] IntPtr pBoolKeys, [In] IntPtr pBoolVals,
            [In] IntPtr pDoubleStarts, int doubleCount,
            [In] IntPtr pDoubleKeys, [In] IntPtr pDoubleVals,
            [In] IntPtr pStringStarts, int stringCount,
            [In] IntPtr pStringKeys, [In] IntPtr pStringVals,
            [In] IntPtr pBoolArrayStarts, int boolArrayCount,
            [In] IntPtr pBoolArrayKeys, [In] IntPtr pBoolArrayVals,
            [In] IntPtr pDoubleArrayStarts, int doubleArrayCount,
            [In] IntPtr pDoubleArrayKeys, [In] IntPtr pDoubleArrayVals,
            [In] IntPtr pStringArrayStarts, int stringArrayCount,
            [In] IntPtr pStringArrayKeys, [In] IntPtr pStringArrayVals,
            [In] IntPtr pInitialMeshes, [Out] IntPtr pMeshCounts, [Out] IntPtr pMeshArray,
            [Out] IntPtr pColorsArray, [Out] IntPtr pTexIndices, [Out] IntPtr pTexKeys, [Out] IntPtr pTexPaths,
            [Out] IntPtr pReportCountArray, [Out] IntPtr pReportKeyArray, [Out] IntPtr pReportDoubleArray,
            [Out] IntPtr pReportBoolArray, [Out] IntPtr pReportStringArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetRuleAttributesCount();

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRuleAttribute(int attrIdx, [In, Out] IntPtr pRule, [In, Out] IntPtr pName, [In, Out] IntPtr pNickname, ref Annotations.AnnotationArgumentType type, [In, Out] IntPtr pGroup);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern int GetRuleAttributes(string rpk_path, [Out] IntPtr pAttributesBuffer, [Out] IntPtr pAttributesTypes);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetAnnotationTypes(int ruleIdx, [In, Out] IntPtr pAnnotTypeArray);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetEnumType(int ruleIdx, int enumIdx, ref Annotations.EnumAnnotationType type);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetAnnotationEnumDouble(int ruleIdx, int enumIdx, [In, Out] IntPtr pArray, ref bool restricted);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetAnnotationEnumString(int ruleIdx, int enumIdx, [In, Out] IntPtr pArray, ref bool restricted);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetAnnotationRange(int ruleIdx, int enumIdx, ref double min, ref double max, ref double stepsize, ref bool restricted);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetCGAPrintOutput(int initialShapeIndex, [In, Out] IntPtr pPrintOutput);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetCGAErrorOutput(int initialShapeIndex, [In, Out] IntPtr pErrorOutput);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetMaterialGenerationOption(bool doGenerate);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesBooleanArray(string key, [In, Out] IntPtr pValues, [In, Out] IntPtr pSizes);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesNumberArray(string key, [In, Out] IntPtr pValues, [In, Out] IntPtr pSizes);

        [DllImport(dllName: PUMA_RHINO_LIBRARY, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetDefaultValuesTextArray(string key, [In, Out] IntPtr pTexts, [In, Out] IntPtr pSizes);

        public static GenerationResult GenerateMesh(string rpkPath,
            ref RuleAttributesMap MM,
            List<Mesh> initialMeshes)
        {
            SimpleArrayMeshPointer initialMeshesArray = new SimpleArrayMeshPointer();
            foreach(var mesh in initialMeshes)
            {
                initialMeshesArray.Add(mesh, true);
            }
            var pMeshesArray = initialMeshesArray.ConstPointer();


            var meshCounts = new SimpleArrayInt();
            var pMeshCounts = meshCounts.NonConstPointer();

            var meshes = new SimpleArrayMeshPointer();
            var pMeshes = meshes.NonConstPointer();

            var stringWrapper = new InteropWrapperString(MM.GetStringStarts(), ref MM.stringKeys, ref MM.stringValues);
            var boolWrapper = new InteropWrapperBoolean(MM.GetBoolStarts(), ref MM.boolKeys, ref MM.boolValues);
            var doubleWrapper = new InteropWrapperDouble(MM.GetDoubleStarts(), ref MM.doubleKeys, ref MM.doubleValues);
            var stringArrayWrapper = new InteropWrapperString(MM.GetStringArrayStarts(), ref MM.stringArrayKeys, ref MM.stringArrayValues);
            var boolArrayWrapper = new InteropWrapperString(MM.GetBoolArrayStarts(), ref MM.boolArrayKeys, ref MM.boolArrayValues);
            var doubleArrayWrapper = new InteropWrapperString(MM.GetDoubleArrayStarts(), ref MM.doubleArrayKeys, ref MM.doubleArrayValues);

            StringWrapper errorMsg = new StringWrapper("");
            IntPtr pErrorMsg = errorMsg.NonConstPointer;

            // Materials
            var colorsArray = new SimpleArrayInt();
            IntPtr pColorsArray = colorsArray.NonConstPointer();
            var matIndices = new SimpleArrayInt();
            IntPtr pMatIndices = matIndices.NonConstPointer();
            var texKeys = new ClassArrayString();
            IntPtr pTexKeys = texKeys.NonConstPointer();
            var texPaths = new ClassArrayString();
            IntPtr pTexPaths = texPaths.NonConstPointer();

            // Reports
            var reportCountArray = new SimpleArrayInt();
            IntPtr pReportCountArray = reportCountArray.NonConstPointer();
            var reportKeyArray = new ClassArrayString();
            IntPtr pReportKeyArray = reportKeyArray.NonConstPointer();
            var reportDoubleArray = new SimpleArrayDouble();
            IntPtr pReportDoubleArray = reportDoubleArray.NonConstPointer();
            var reportBoolArray = new SimpleArrayInt();
            IntPtr pReportBoolArray = reportBoolArray.NonConstPointer();
            var reportStringArray = new ClassArrayString();
            IntPtr pReportStringArray = reportStringArray.NonConstPointer();

            Generate(rpkPath,
                     pErrorMsg,
                     initialMeshes.Count,
                     boolWrapper.StartsPtr(),
                     boolWrapper.Count,
                     boolWrapper.KeysPtr(),
                     boolWrapper.ValuesPtr(),
                     doubleWrapper.StartsPtr(),
                     doubleWrapper.Count,
                     doubleWrapper.KeysPtr(),
                     doubleWrapper.ValuesPtr(),
                     stringWrapper.StartsPtr(),
                     stringWrapper.Count,
                     stringWrapper.KeysPtr(),
                     stringWrapper.ValuesPtr(),
                     boolArrayWrapper.StartsPtr(),
                     boolArrayWrapper.Count,
                     boolArrayWrapper.KeysPtr(),
                     boolArrayWrapper.ValuesPtr(),
                     doubleArrayWrapper.StartsPtr(),
                     doubleArrayWrapper.Count,
                     doubleArrayWrapper.KeysPtr(),
                     doubleArrayWrapper.ValuesPtr(),
                     stringArrayWrapper.StartsPtr(),
                     stringArrayWrapper.Count,
                     stringArrayWrapper.KeysPtr(),
                     stringArrayWrapper.ValuesPtr(),
                     pMeshesArray,
                     pMeshCounts,
                     pMeshes,
                     pColorsArray,
                     pMatIndices,
                     pTexKeys,
                     pTexPaths,
                     pReportCountArray,
                     pReportKeyArray,
                     pReportDoubleArray,
                     pReportBoolArray,
                     pReportStringArray);

            initialMeshesArray.Dispose();
            boolWrapper.Dispose();
            doubleWrapper.Dispose();
            stringWrapper.Dispose();

            var meshCountsArray = meshCounts.ToArray();
            var meshesArray = meshes.ToNonConstArray();
            // Materials
            int[] colors = colorsArray.ToArray();
            int[] materialIndices = matIndices.ToArray();
            string[] textureKeys = texKeys.ToArray();
            string[] texturePaths = texPaths.ToArray();

            GenerationResult generationResult = new GenerationResult();

            int indexOffset = 0;
            int colorsOffset = 0;
            int textureOffset = 0;

            // Geometry
            for (int id = 0; id < meshCountsArray.Length; id++)
            {
                if (meshCountsArray[id] > 0)
                {
                    var meshesForShape = meshesArray.Skip(indexOffset).Take(meshCountsArray[id]).ToArray();
                    generationResult.meshes.Add(meshesForShape);
                }
                else
                    generationResult.meshes.Add(null);

                indexOffset += meshCountsArray[id];
            }

            // Materials
            for (int id = 0; id < materialIndices.Length;)
            {
                int meshCount = materialIndices[id];

                GH_Material[] materials = new GH_Material[meshCount];

                for(int meshId = 0; meshId < meshCount; meshId++)
                {
                    int texCount = materialIndices[id + meshId + 1];

                    var diffuse = colors.Skip(colorsOffset).Take(3).ToArray();
                    var ambient = colors.Skip(colorsOffset + 3).Take(3).ToArray();
                    var specular = colors.Skip(colorsOffset + 6).Take(3).ToArray();
                    var opacity = colors[colorsOffset + 9];
                    var shininess = colors[colorsOffset + 10];

                    colorsOffset += 11;

                    Material mat = new Material()
                    {
                        DiffuseColor = Color.FromArgb(diffuse[0], diffuse[1], diffuse[2]),
                        AmbientColor = Color.FromArgb(ambient[0], ambient[1],ambient[2]),
                        SpecularColor = Color.FromArgb(specular[0], specular[1], specular[2]),
                        Transparency = 1.0 - opacity,
                        Shine = shininess,
                        FresnelReflections = true,
                    };

                    string coloMap = "";

                    for (int texId = 0; texId < texCount; ++texId)
                    {
                        string texKey = textureKeys[textureOffset + texId];
                        string texPath = texturePaths[textureOffset + texId];

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
                                if (coloMap != tex.FileReference.FullPath)
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

                    textureOffset += texCount;

                    materials[meshId] = new GH_Material(Rhino.Render.RenderMaterial.CreateBasicMaterial(mat));
                }

                id += meshCount + 1;

                generationResult.materials.Add(materials);
            }

            colorsArray.Dispose();
            matIndices.Dispose();
            texKeys.Dispose();
            texPaths.Dispose();

            // Reports
            var reportCounts = reportCountArray.ToArray();
            var reportKeys = reportKeyArray.ToArray();
            var reportDouble = reportDoubleArray.ToArray();
            var reportBool = reportBoolArray.ToArray();
            var reportString = reportStringArray.ToArray();

            reportCountArray.Dispose();
            reportKeyArray.Dispose();
            reportDoubleArray.Dispose();
            reportBoolArray.Dispose();
            reportStringArray.Dispose();

            if (reportKeys.Length != reportDouble.Length + reportBool.Length + reportString.Length)
            {
                // Something went wrong, don't output any reports.
                return generationResult;
            }

            int reportKeyOffset = 0;
            int reportDoubleOffset = 0;
            int reportBoolOffset = 0;
            int reportStringOffset = 0;

            for(int meshId = 0; meshId < reportCounts.Length; meshId+=3) {
                int doubleReportCount = reportCounts[meshId];
                int boolReportCount = reportCounts[meshId + 1];
                int stringReportCount = reportCounts[meshId + 2];
                List<ReportAttribute> reportAttributes = new List<ReportAttribute>();

                var doubleKeys = reportKeys.Skip(reportKeyOffset).Take(doubleReportCount);
                var d = reportDouble.Skip(reportDoubleOffset).Take(doubleReportCount).Zip(doubleKeys, (value, key) => ReportAttribute.CreateReportAttribute(meshId / 3, key, ReportTypes.PT_FLOAT, value));
                reportAttributes.AddRange(d);
                reportKeyOffset += doubleReportCount;
                reportDoubleOffset += doubleReportCount;

                var boolKeys = reportKeys.Skip(reportKeyOffset).Take(boolReportCount);
                var b = reportBool.Skip(reportBoolOffset).Take(boolReportCount).Zip(boolKeys, (value, key) => ReportAttribute.CreateReportAttribute(meshId / 3, key, ReportTypes.PT_BOOL, Convert.ToBoolean(value)));
                reportAttributes.AddRange(b);
                reportKeyOffset += boolReportCount;
                reportBoolOffset += boolReportCount;

                var stringKeys = reportString.Skip(reportKeyOffset).Take(stringReportCount);
                var s = reportString.Skip(reportStringOffset).Take(stringReportCount).Zip(stringKeys, (value, key) => ReportAttribute.CreateReportAttribute(meshId / 3, key, ReportTypes.PT_STRING, value));
                reportAttributes.AddRange(s);
                reportKeyOffset += stringReportCount;
                reportStringOffset += stringReportCount;

                generationResult.reports.Add(reportAttributes.ToArray());
            }

            return generationResult;
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

        public static RuleAttribute[] GetRuleAttributes(string rpk)
        {
            ClassArrayString attributesBuffer = new ClassArrayString();
            ClassArrayString annotationsBuffer = new ClassArrayString();
            SimpleArrayInt attributesTypes = new SimpleArrayInt();

            int attribCount = GetRuleAttributes(rpk, attributesBuffer.NonConstPointer(), attributesTypes.NonConstPointer());

            if (attribCount == 0) return new RuleAttribute[0]{};

            var attributesArray = attributesBuffer.ToArray();
            var attributesTypesArray = attributesTypes.ToArray();

            RuleAttribute[] attributes = new RuleAttribute[attribCount];

            for(int i = 0; i < attribCount; i++)
            {
                string rule = attributesArray[i*4];
                string name = attributesArray[i*4 + 1];
                string nickname = attributesArray[i*4 + 2];
                string group = attributesArray[i*4 + 3];
                Annotations.AnnotationArgumentType type = (Annotations.AnnotationArgumentType)attributesTypesArray[i];
                attributes[i] = new RuleAttribute(name, nickname, rule, type, group == null ? "" : group);

                // TODO: Add annotations
                //List<Annotations.Base> annotations = GetAnnotations(i);
                //attributes[i].mAnnotations.AddRange(annotations);
            }

            attributesBuffer.Dispose();
            attributesTypes.Dispose();
            
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

        public static List<bool> GetDefaultValuesBoolean(string key)
        {
            SimpleArrayInt boolArray = new SimpleArrayInt();
            var pBoolArray = boolArray.NonConstPointer();
            bool hasDefault = false; //  GetDefaultValuesBoolean(key, pBoolArray);

            if (!hasDefault) return null;

            List<bool> boolList = new List<int>(boolArray.ToArray()).ConvertAll(x => Convert.ToBoolean(x));
            boolArray.Dispose();

            return boolList;
        }

        public static List<double> GetDefaultValuesNumber(string key)
        {
            SimpleArrayDouble doubleArray = new SimpleArrayDouble();
            var pDoubleArray = doubleArray.NonConstPointer();
            bool hasDefault = false; //GetDefaultValuesNumber(key, pDoubleArray);

            if (!hasDefault) return null;

            List<double> doubleList = new List<double>(doubleArray.ToArray());
            doubleArray.Dispose();

            return doubleList;
        }

        public static List<string> GetDefaultValuesText(string key)
        {
            ClassArrayString stringArray = new ClassArrayString();
            var pStringArray = stringArray.NonConstPointer();
            bool hasDefault = false; // GetDefaultValuesText(key, pStringArray);

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
