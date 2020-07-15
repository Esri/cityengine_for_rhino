using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
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

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ShutdownRhinoPRT();

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
        public static extern bool GetRuleAttribute(int attrIdx, StringBuilder rule, int rule_size, StringBuilder name, int name_size, ref AnnotationArgumentType type);

        [DllImport(dllName: "RhinoPRT.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern void SetRuleAttributeDouble(string rule, string fullName, double value);

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

            GH_Structure<GH_Mesh> mesh_struct = new GH_Structure<GH_Mesh>();

            foreach(var mesh in meshes) {
                GH_Mesh gh_mesh = null;
                bool status = GH_Convert.ToGHMesh(mesh, GH_Conversion.Both, ref gh_mesh);
                if (status) mesh_struct.Append(gh_mesh);
            }

            return mesh_struct;
        }

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

            // To test the rhino wrappers
            //SimpleArrayMeshPointer meshes = new SimpleArrayMeshPointer();
            //IntPtr ptr_array = meshes.NonConstPointer();
           
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
                // With some initial shapes (like Surface), the generates returns a number of face that is greater than the buffer of indices,
                // leading to index out of bounds exceptions. This is a workaround to avoid a crash.
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
                AnnotationArgumentType type = AnnotationArgumentType.AAT_INT;

                bool status = GetRuleAttribute(i, ruleBuilder, ruleBuilder.Capacity, nameBuilder, nameBuilder.Capacity, ref type);
                if (!status) return new RuleAttribute[0] {};

                attributes[i] = new RuleAttribute(nameBuilder.ToString(), ruleBuilder.ToString(), type);
            }
            
            return attributes;
        }
    }
}
