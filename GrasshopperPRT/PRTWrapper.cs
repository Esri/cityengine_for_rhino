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

namespace GrasshopperPRT
{
    /// <summary>
    /// Encasulate RhinoPRT library.
    /// </summary>
    class PRTWrapper
    {
        public static String INIT_SHAPE_IDX_KEY = "InitShapeIdx";

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern void GetProductVersion([In, Out]IntPtr version_Str);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool InitializeRhinoPRT();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetPackage(string rpk_path);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool AddInitialMesh([In]IntPtr pMesh);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ClearInitialShapes();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Generate();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetMeshBundle(int initShapeID, [In, Out]IntPtr pMeshArray);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetAllMeshIDs([In, Out]IntPtr pMeshIDs);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetMeshPartCount(int initShapeId);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetRuleAttributesCount();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetRuleAttribute(int attrIdx, StringBuilder rule, int rule_size, StringBuilder name, int name_size, StringBuilder nickname, int nickname_size, ref AnnotationArgumentType type);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeDouble(string rule, string fullName, double value);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeBoolean(string rule, string fullName, bool value);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeInteger(string rule, string fullName, int value);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeString(string rule, string fullName, string value);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetReports(int initialShapeId, [In, Out] IntPtr pKeysArray,
        [In, Out] IntPtr pDoubleReports,
        [In, Out] IntPtr pBoolReports,
        [In, Out] IntPtr pStringReports);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetMaterial(int initialShapeId, int shapeID, ref int pUvSet,
                                                [In, Out] IntPtr pTexKeys,
                                                [In, Out] IntPtr pTexPaths,
                                                [In, Out] IntPtr pDiffuseColor,
                                                [In, Out] IntPtr pAmbientColor,
                                                [In, Out] IntPtr pSpecularColor,
                                                ref double opacity,
                                                ref double shininess);

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

        public static GH_Material GetMaterialOfPartMesh(int initialShapeId, int meshID)
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

            bool status = PRTWrapper.GetMaterial(initialShapeId, meshID, ref uvSet, pTexKeys, pTexPaths, pDiffuseArray, pAmbientArray, pSpecularArray, ref opacity, ref shininess);
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
                //mat.Diffuse = Color.FromArgb(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
            }

            if(ambientColor.Length == 3)
            {
                mat.AmbientColor = Color.FromArgb(ambientColor[0], ambientColor[1], ambientColor[2]);
                //mat.Emission = Color.FromArgb(ambientColor[0], ambientColor[1], ambientColor[2]);
            }

            if(specularColor.Length == 3)
            {
                mat.SpecularColor = Color.FromArgb(specularColor[0], specularColor[1], specularColor[2]);
                //mat.Specular = Color.FromArgb(specularColor[0], specularColor[1], specularColor[2]);
            }

            mat.Transparency = 1.0 - opacity;
            mat.Shine = shininess;

            mat.FresnelReflections = true;

            mat.CommitChanges();
            
            return new GH_Material(mat.RenderMaterial);
        }

        public static List<GH_Material> GetMaterialsOfMesh(int initShapeId)
        {
            List<GH_Material> materials = new List<GH_Material>();

            int meshCount = GetMeshPartCount(initShapeId);

            for(int i = 0; i < meshCount; ++i)
            {
                materials.Add(GetMaterialOfPartMesh(initShapeId, i));
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

        public static List<ReportAttribute> GetAllReports(int initialShapeId)
        {
            var keys = new ClassArrayString();
            var stringReports = new ClassArrayString();
            var doubleReports = new SimpleArrayDouble();
            var boolReports = new SimpleArrayInt();

            var pKeys = keys.NonConstPointer();
            var pStrReps = stringReports.NonConstPointer();
            var pDblReps = doubleReports.NonConstPointer();
            var pBoolReps = boolReports.NonConstPointer();

            GetReports(initialShapeId, pKeys, pDblReps, pBoolReps, pStrReps);

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
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeId, keysArray[kId], keysArray[kId], ReportTypes.PT_FLOAT, doubleReportsArray[i]));
                kId++;
            }
            for(int i = 0; i < boolReportsArray.Length; ++i)
            {
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeId, keysArray[kId], keysArray[kId], ReportTypes.PT_BOOL, Convert.ToBoolean(boolReportsArray[i])));
                kId++;    
            }
            for(int i = 0; i < stringReportsArray.Length; ++i)
            {
                ras.Add(ReportAttribute.CreateReportAttribute(initialShapeId, keysArray[kId], keysArray[kId], ReportTypes.PT_STRING, stringReportsArray[i]));
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
                StringBuilder ruleBuilder = new StringBuilder(100);
                StringBuilder nameBuilder = new StringBuilder(100);
                StringBuilder nicknameBuilder = new StringBuilder(100);
                AnnotationArgumentType type = AnnotationArgumentType.AAT_INT;

                bool status = GetRuleAttribute(i, ruleBuilder, ruleBuilder.Capacity, nameBuilder, nameBuilder.Capacity, nicknameBuilder, nicknameBuilder.Capacity, ref type);
                if (!status) return new RuleAttribute[0] {};

                attributes[i] = new RuleAttribute(nameBuilder.ToString(), nicknameBuilder.ToString(), ruleBuilder.ToString(), type);
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
    }
}
