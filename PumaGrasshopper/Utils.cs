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
using Grasshopper.Kernel.Data;
using System.Xml.Serialization;
using System.IO;

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

        public static GH_Structure<GH_Number> FromListToTree(List<double> valueList)
        {
            GH_Structure<GH_Number> tree = new GH_Structure<GH_Number>();
            for (int i = 0; i < valueList.Count; ++i)
            {
                tree.Insert(new GH_Number(valueList[i]), new GH_Path(i), 0);
            }
            return tree;
        }

        public static GH_Structure<GH_Boolean> FromListToTree(List<bool> valueList)
        {
            GH_Structure<GH_Boolean> tree = new GH_Structure<GH_Boolean>();
            for(int i = 0; i < valueList.Count; ++i)
            {
                tree.Insert(new GH_Boolean(valueList[i]), new GH_Path(i), 0);
            }
            return tree;
        }
        public static GH_Structure<GH_String> FromListToTree(List<string> valueList)
        {
            GH_Structure<GH_String> tree = new GH_Structure<GH_String>();
            for(int i = 0; i < valueList.Count; ++i)
            {
                tree.Insert(new GH_String(valueList[i]), new GH_Path(i), 0);
            }
            return tree;
        }

        public static GH_Structure<GH_Colour> HexListToColorTree(List<string> valueList)
        {
            GH_Structure<GH_Colour> tree = new GH_Structure<GH_Colour>();
            for(int i = 0; i < valueList.Count; ++i)
            {
                tree.Insert(new GH_Colour(FromHex(valueList[i])), new GH_Path(i), 0);
            }
            return tree;
        }

        public static GH_Structure<GH_Boolean> FromListToTree(List<List<bool>> valueList)
        {
            GH_Structure<GH_Boolean> tree = new GH_Structure<GH_Boolean>();
            for (int i = 0; i < valueList.Count; ++i)
            {
                tree.AppendRange(valueList[i].ConvertAll(val => new GH_Boolean(val)), new GH_Path(i));
            }
            return tree;
        }

        public static GH_Structure<GH_String> FromListToTree(List<List<string>> textList)
        {
            GH_Structure<GH_String> tree = new GH_Structure<GH_String>();
            for(int i = 0; i < textList.Count; ++i)
            {
                tree.AppendRange(textList[i].ConvertAll(t => new GH_String(t)), new GH_Path(i));
            }
            return tree;
        }

        public static GH_Structure<GH_Number> FromListToTree(List<List<double>> valueList)
        {
            GH_Structure<GH_Number> tree = new GH_Structure<GH_Number>();
            for(int i = 0; i < valueList.Count; ++i)
            {
                tree.AppendRange(valueList[i].ConvertAll(value => new GH_Number(value)), new GH_Path(i));
            }
            return tree;
        }

        public static List<List<T>> UnflattenList<T>(List<T> valueList, List<int> sizes)
        {
            int offset = 0;
            List<List<T>> returnList = new List<List<T>>(sizes.Count);
            sizes.ForEach(size => {
                returnList.Add(valueList.GetRange(offset, size));
                offset += size;
            });

            return returnList;
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

        public static void AddToGroup(GH_Document doc, Guid pumaUID, string groupNickname, Guid guid)
        {
            GH_Group grp;

            string groupUID = pumaUID + "_" + groupNickname;

            var group = doc.Objects.OfType<GH_Group>().Where(gr => gr.Name == groupUID);
            if (group.Count() == 0)
            {
                grp = new GH_Group();
                grp.CreateAttributes();
                grp.Name = groupUID;
                grp.NickName = groupNickname;
                grp.Colour = System.Drawing.Color.Aquamarine;
                grp.Border = GH_GroupBorder.Box;
                doc.AddObject(grp, false);
            }
            else
            {
                grp = group.First();
            }

            grp.AddObject(guid);
            grp.ExpireCaches();
            grp.ExpirePreview(true);
        }

        public static string ToXML(object obj)
        {
            XmlSerializer serializer = new XmlSerializer(obj.GetType());
            MemoryStream stream = new MemoryStream();
            serializer.Serialize(stream, obj);
            stream.Position = 0;
            StreamReader reader = new StreamReader(stream);
            return reader.ReadToEnd();
        }

        public static string GetCastErrorMessage(string attribute, string castTarget)
        {
            return "Could not cast attribute " + attribute + " to " + castTarget;
        }
    }
}
