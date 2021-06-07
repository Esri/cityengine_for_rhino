using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;
using Grasshopper.Kernel.Types;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Special;
using System.Drawing;

namespace GrasshopperPRT
{

    class Utils
    {
        public static IntPtr CreateIntArrayPtr(int size)
        {
            return Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(int)) * size);
        }

        public static IntPtr CreateStringArrayPtr(int size)
        {
            return Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(string)) * size);
        }

        public static IntPtr FromArrayToIntPtr(ref double[] array)
        {
            IntPtr buffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(double)) * array.Length);
            Marshal.Copy(array, 0, buffer, array.Length);
            return buffer;
        }

        public static IntPtr FromArrayToIntPtr(ref int[] array)
        {
            IntPtr buffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(array.Length) * array.Length);
            Marshal.Copy(array, 0, buffer, array.Length);
            return buffer;
        }

        public static double[] FromIntPtrToDoubleArray(IntPtr buffer, int size)
        {
            double[] array = new double[size];
            Marshal.Copy(buffer, array, 0, size);
            Marshal.FreeCoTaskMem(buffer);
            return array;
        }

        public static int[] FromIntPtrToIntArray(IntPtr buffer, int size)
        {
            int[] array = new int[size];
            Marshal.Copy(buffer, array, 0, size);
            Marshal.FreeCoTaskMem(buffer);
            return array;
        }

        /// <summary>
        /// Returns a string representation of a hex color given a GH_Colour object
        /// </summary>
        /// <param name="ghColor">the grasshopper color to convert</param>
        /// <returns>a hex color string</returns>
        public static string hexColor(Color color)
        {
            string hexStr = "#" + color.R.ToString("X2") +
                color.G.ToString("X2") +
                color.B.ToString("X2");

            return hexStr;
        }

        public static bool isInteger(double d)
        {
            return Math.Abs(d % 1) <= (Double.Epsilon * 100);
        }

        public static void AddToGroup(GH_Document doc, string groupName, Guid guid)
        {
            GH_Group grp;

            var group = doc.Objects.OfType<GH_Group>().Where(gr => gr.Name == groupName);
            if (group.Count() == 0)
            {
                grp = new GH_Group();
                grp.CreateAttributes();
                grp.Name = groupName;
                grp.NickName = groupName;
                grp.Colour = System.Drawing.Color.Aquamarine;
                grp.Border = GH_GroupBorder.Box;
                doc.AddObject(grp, false);
            }
            else if (group.Count() == 1)
            {
                grp = group.First();
            }
            else return;

            grp.AddObject(guid);
            grp.ExpireCaches();
        }

        public static string GetCastErrorMessage(RuleAttribute attribute, string castTarget)
        {
            return "Could not cast attribute " + attribute.mFullName + " to " + castTarget;
        }
    }
}
