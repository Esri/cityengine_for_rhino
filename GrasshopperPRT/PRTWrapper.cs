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
        [DllImport(dllName: "RhinoPRT.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern bool InitializeRhinoPRT();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetPackage(string rpk_path);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool AddShape(double[] vertices, int vCount, int[] indices, int iCount, int[] faceCount, int faceCountCount);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool AddMeshTest([In]IntPtr pMesh);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ClearInitialShapes();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Generate(ref IntPtr vertices, ref int vCount, ref IntPtr indices, ref int iCount, ref IntPtr faceCount, ref int faceCountCount);

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

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GroupeReportsByKeys();

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool GetReportKeys([In, Out] IntPtr pKeysArray, [In, Out] IntPtr pKeyTypeArray);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetDoubleReports(string repKey, [In, Out] IntPtr reports);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetStringReports(string repKey, [In, Out] IntPtr reports);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void GetBoolReports(string repKey, [In, Out] IntPtr reports);

        /// <summary>
        /// NOT USED ANYMORE.
        /// THIS IS THE PREVIOUS ADD MESH FUNCTION, BEFORE THE "SimpleArrayMeshPointer" WAS USED.
        /// I left it there for now in case we want to stop using the rhino wrapper.
        /// </summary>
        public static bool AddMesh(Mesh initialMesh)
        {
            // Get the initial shape params in base types.
            initialMesh.Vertices.UseDoublePrecisionVertices = true;
            initialMesh.Faces.ConvertTrianglesToQuads( Rhino.RhinoMath.ToRadians(2), .875 );

            double[] vertices = Array.ConvertAll(initialMesh.Vertices.ToFloatArray(), x => (double)x);
            int[] indices = initialMesh.Faces.ToIntArray(false);
            int[] faceCount = Array.ConvertAll(initialMesh.Faces.ToArray(), x => x.IsQuad ? 4 : 3);
            
            return AddShape(vertices, vertices.Length, indices, indices.Length, faceCount, faceCount.Length);
        }

        public static bool AddMeshTestWrapper(List<Mesh> meshes)
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

        public static GH_Structure<GH_Mesh> GenerateMeshTestWrapper()
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

            foreach(var mesh in meshes) {
                GH_Mesh gh_mesh = null;
                bool status = GH_Convert.ToGHMesh(mesh, GH_Conversion.Both, ref gh_mesh);
                if (status) mesh_struct.Append(gh_mesh);
            }

            return mesh_struct;
        }

        public static List<ReportAttribute> GetReportKeys()
        {
            var keyArray = new ClassArrayString();
            var keyTypeArray = new SimpleArrayInt();

            var ptrKeyArray = keyArray.NonConstPointer();
            var ptrKeyTypeArray = keyTypeArray.NonConstPointer();
            
            bool success = PRTWrapper.GetReportKeys(ptrKeyArray, ptrKeyTypeArray);

            if (!success) return null;

            string[] keys = keyArray.ToArray();
            int[] types = keyTypeArray.ToArray();

            if (keys.Length != types.Length) return null;

            List<ReportAttribute> reports = new List<ReportAttribute>();
            for(int i = 0; i < keys.Length; ++i)
            {
                reports.Add(new ReportAttribute(keys[i], (ReportTypes)types[i]));
            }

            keyArray.Dispose();
            keyTypeArray.Dispose();

            return reports;
        }

        public static bool[] GetBoolReports(string key)
        {
            bool[] reports = null;

            using(var reportsArray = new SimpleArrayInt())
            {
                var pReportsArr = reportsArray.NonConstPointer();

                PRTWrapper.GetBoolReports(key, pReportsArr);
                reports = Array.ConvertAll<int, bool>(reportsArray.ToArray(), x => Convert.ToBoolean(x));
            }

            return reports;
        }

        public static double[] GetDoubleReports(string key)
        {
            double[] reports = null;

            using (var reportsArray = new SimpleArrayDouble()) {
                var pReportsArr = reportsArray.NonConstPointer();

                PRTWrapper.GetDoubleReports(key, pReportsArr);
                reports = reportsArray.ToArray();
            }

            return reports;
        }

        public static string[] GetStringReports(string key)
        {
            string[] reports = null;

            using (var reportsArray = new ClassArrayString())
            {
                var pReportsArr = reportsArray.NonConstPointer();

                PRTWrapper.GetStringReports(key, pReportsArr);
                reports = reportsArray.ToArray();
            }

            return reports;
        }

        /// <summary>
        /// NOT USED ANYMORE.
        /// THIS IS THE PREVIOUS GENERATE FUNCTION, BEFORE THE "SimpleArrayMeshPointer" WAS USED.
        /// I left it there for now in case we want to stop using the rhino wrapper.
        /// </summary>
        /// <returns>Mesh</returns>
        public static Mesh GenerateMesh()
        {
            //TODO: change that to allocate global memory in the c++ side and free it here after copy.
            double[] vertices = new double[2000];
            int[] indices = new int[2000];
            int[] faceCount = new int[2000];
            int vCount = vertices.Length;
            int iCount = indices.Length;
            int faceCountCount = faceCount.Length;

            IntPtr vBuffer = Utils.FromArrayToIntPtr(ref vertices);
            IntPtr iBuffer = Utils.FromArrayToIntPtr(ref indices);
            IntPtr fCountBuffer = Utils.FromArrayToIntPtr(ref faceCount);
           
            bool status = Generate(ref vBuffer, ref vCount, ref iBuffer, ref iCount, ref fCountBuffer, ref faceCountCount);
            if (!status) return new Mesh();

            // Generate was successfull
            double[] verticesResult = Utils.FromIntPtrToDoubleArray(vBuffer, vCount);
            int[] indicesResult = Utils.FromIntPtrToIntArray(iBuffer, iCount);
            int[] fCountResult = Utils.FromIntPtrToIntArray(fCountBuffer, faceCountCount);
            
            Mesh mesh = new Mesh();

            // Fill vertices
            int nbVertices = verticesResult.Length / 3;
            for(int i = 0; i < nbVertices; ++i)
            {
                mesh.Vertices.Add(verticesResult[i * 3], verticesResult[i * 3 + 1], verticesResult[i * 3 + 2]);
            }

            // Fill face indices
            int currID = 0;
            foreach(int face in fCountResult)
            {
                if (currID + face > indicesResult.Length) continue;

                if (face == 3)
                {
                    mesh.Faces.AddFace(new MeshFace(indicesResult[currID], indicesResult[currID + 1], indicesResult[currID + 2]));
                }
                else if( face == 4)
                {
                    
                    mesh.Faces.AddFace(new MeshFace(indicesResult[currID], indicesResult[currID+1], indicesResult[currID+2], indicesResult[currID+3]));
                }
                currID += face;
            }

            return mesh;
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
    }
}
