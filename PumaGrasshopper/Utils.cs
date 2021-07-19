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
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;
using Grasshopper.Kernel.Types;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Special;
using System.Drawing;

namespace PumaGrasshopper
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
        public static string HexColor(Color color)
        {
            string hexStr = "#" + color.R.ToString("X2") +
                color.G.ToString("X2") +
                color.B.ToString("X2");

            return hexStr;
        }

        public static Color FromHex(string hexColor)
        {
            return ColorTranslator.FromHtml(hexColor);
        }

        public static bool IsInteger(double d)
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

        public static string GetCastErrorMessage(string attribute, string castTarget)
        {
            return "Could not cast attribute " + attribute + " to " + castTarget;
        }
    }
}
