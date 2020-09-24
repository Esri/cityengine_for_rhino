using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;

using Rhino.Runtime.InteropWrappers;


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
        public static extern bool AddMeshTest([In]IntPtr pMesh);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ClearInitialShapes();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GenerateTest([In,Out]IntPtr pMeshArray);

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
                status = AddMeshTest(ptr_array);
            }

            return status;
        }

        public static GH_Structure<GH_Mesh> GenerateMesh()
        {
            Mesh[] meshes = null;

            using(var arr = new SimpleArrayMeshPointer())
            {
                var ptr_array = arr.NonConstPointer();

                // Start the geometry generation
                bool status = GenerateTest(ptr_array);
                if (!status) return null;
                meshes = arr.ToNonConstArray();
            }

            // GH_Structure is the data tree outputed by our component, it takes only GH_Mesh (which is a grasshopper wrapper class over the rhino Mesh), 
            // thus a conversion is necessary.
            GH_Structure<GH_Mesh> mesh_struct = new GH_Structure<GH_Mesh>();

            int currIsID = 0;

            foreach(var mesh in meshes) {
                //if some of the initial shapes failed to be processed, add empty meshes to keep the synchronization between input and output.
                int id = Convert.ToInt32(mesh.GetUserString(INIT_SHAPE_IDX_KEY));
                if (id != -1) // If id == -1, we don't check syncronization since mesh id were wrongly setup at the beginning.
                { 
                    while (currIsID < id)
                    {
                        mesh_struct.Append(null);
                        currIsID++;
                    }
                }
                currIsID++;

                GH_Mesh gh_mesh = null;
                bool status = GH_Convert.ToGHMesh(mesh, GH_Conversion.Both, ref gh_mesh);
                if (status) mesh_struct.Append(gh_mesh);
            }

            return mesh_struct;
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
