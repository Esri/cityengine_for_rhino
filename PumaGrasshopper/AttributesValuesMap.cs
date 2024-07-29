/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2024 Esri R&D Center Zurich
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

using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PumaGrasshopper
{
    public class AttributesValuesMap
    {
        private Dictionary<string, bool> mDefaultBooleans;
        private Dictionary<string, int> mDefaultIntegers;
        private Dictionary<string, string> mDefaultStrings;
        private Dictionary<string, double> mDefaultDoubles;
        private Dictionary<string, bool[]> mDefaultBoolArrays;
        private Dictionary<string, int[]> mDefaultIntegerArrays;
        private Dictionary<string, string[]> mDefaultStringArrays;
        private Dictionary<string, double[]> mDefaultDoubleArrays;

        public AttributesValuesMap(string[] boolKeys, bool[] boolValues, string[] integerKeys, int[] integerValues,
                                string[] stringKeys, string[] stringValues, string[] doubleKeys, double[] doubleValues,
                                string[] boolArrayKeys, string[] boolArrayValues, string[] integerArrayKeys, string[] integerArrayValues,
                                string[] stringArrayKeys, string[] stringArrayValues, string[] doubleArrayKeys, string[] doubleArrayValues)
        {
            mDefaultBooleans = new Dictionary<string, bool>();
            mDefaultIntegers = new Dictionary<string, int>();
            mDefaultStrings = new Dictionary<string, string>();
            mDefaultDoubles = new Dictionary<string, double>();
            mDefaultBoolArrays = new Dictionary<string, bool[]>();
            mDefaultIntegerArrays = new Dictionary<string, int[]>();
            mDefaultStringArrays = new Dictionary<string, string[]>();
            mDefaultDoubleArrays = new Dictionary<string, double[]>();

            for(int i = 0; i < boolKeys.Length; ++i)
            {
                mDefaultBooleans.Add(boolKeys[i], boolValues[i]);
            }

            for(int i = 0; i < integerKeys.Length; ++i)
            {
                mDefaultIntegers.Add(integerKeys[i], integerValues[i]);
            }

            for(int i = 0; i < stringKeys.Length; ++i)
            {
                mDefaultStrings.Add(stringKeys[i], stringValues[i]);
            }

            for(int i = 0; i < doubleKeys.Length; ++i)
            {
                mDefaultDoubles.Add(doubleKeys[i], doubleValues[i]);
            }

            for(int i = 0; i < boolArrayKeys.Length; ++i)
            {
                mDefaultBoolArrays.Add(boolArrayKeys[i], Utils.BoolFromCeArray(boolArrayValues[i]));
            }

            for(int i = 0; i < integerArrayKeys.Length; ++i)
            {
                mDefaultIntegerArrays.Add(integerArrayKeys[i], Utils.IntegerFromCeArray(integerArrayValues[i]));
            }

            for(int i = 0; i < stringArrayKeys.Length; ++i)
            {
                mDefaultStringArrays.Add(stringArrayKeys[i], Utils.StringFromCeArray(stringArrayValues[i]));
            }

            for(int i = 0; i < doubleArrayKeys.Length; ++i)
            {
                mDefaultDoubleArrays.Add(doubleArrayKeys[i], Utils.DoubleFromCeArray(doubleArrayValues[i]));
            }
        }

        public bool GetBool(string key, out bool value)
        {
            return mDefaultBooleans.TryGetValue(key, out value);
        }

        public bool GetInteger(string key, out int value)
        {
            return mDefaultIntegers.TryGetValue(key, out value);
        }

        public bool GetString(string key, out string value)
        {
            return mDefaultStrings.TryGetValue(key, out value);
        }

        public bool GetDouble(string key, out double value)
        {
            return mDefaultDoubles.TryGetValue(key, out value);
        }

        public bool GetBoolArray(string key, out bool[] values)
        {
            return mDefaultBoolArrays.TryGetValue(key, out values);
        }

        public bool GetIntegerArray(string key, out int[] values)
        {
            return mDefaultIntegerArrays.TryGetValue(key, out values);
        }

        public bool GetStringArray(string key, out string[] values)
        {
            return mDefaultStringArrays.TryGetValue(key, out values);
        }

        public bool GetDoubleArray(string key, out double[] values)
        {
            return mDefaultDoubleArrays.TryGetValue(key, out values);
        }

        public static AttributesValuesMap[] FromInteropWrappers(int shapeCount, ref InteropWrapperBoolean boolWrapper,
                                                           ref InteropWrapperInteger integerWrapper,
                                                           ref InteropWrapperString stringWrapper,
                                                           ref InteropWrapperDouble doubleWrapper,
                                                           ref InteropWrapperString boolArrayWrapper,
                                                           ref InteropWrapperString integerArrayWrapper,
                                                           ref InteropWrapperString stringArrayWrapper,
                                                           ref InteropWrapperString doubleArrayWrapper)
        {
            int[] boolStarts = boolWrapper.StartsToArray();
            string[] boolKeys = boolWrapper.KeysToArray();
            bool[] boolValues = boolWrapper.ValuesToArray();
            int[] integerStarts = integerWrapper.StartsToArray();
            string[] integerKeys = integerWrapper.KeysToArray();
            int[] integerValues = integerWrapper.ValuesToArray();
            int[] stringStarts = stringWrapper.StartsToArray();
            string[] stringKeys = stringWrapper.KeysToArray();
            string[] stringValues = stringWrapper.ValuesToArray();
            int[] doubleStarts = doubleWrapper.StartsToArray();
            string[] doubleKeys = doubleWrapper.KeysToArray();
            double[] doubleValues = doubleWrapper.ValuesToArray();
            int[] boolArrayStarts = boolArrayWrapper.StartsToArray();
            string[] boolArrayKeys = boolArrayWrapper.KeysToArray();
            string[] boolArrayValues = boolArrayWrapper.ValuesToArray();
            int[] integerArrayStarts = integerArrayWrapper.StartsToArray();
            string[] integerArrayKeys = integerArrayWrapper.KeysToArray();
            string[] integerArrayValues = integerArrayWrapper.ValuesToArray();
            int[] stringArrayStarts = stringArrayWrapper.StartsToArray();
            string[] stringArrayKeys = stringArrayWrapper.KeysToArray();
            string[] stringArrayValues = stringArrayWrapper.ValuesToArray();
            int[] doubleArrayStarts = doubleArrayWrapper.StartsToArray();
            string[] doubleArrayKeys = doubleArrayWrapper.KeysToArray();
            string[] doubleArrayValues = doubleArrayWrapper.ValuesToArray();

            AttributesValuesMap[] defaultValues = new AttributesValuesMap[shapeCount];
            for (int i = 0; i < shapeCount; ++i)
            {
                int boolStart = boolStarts[i];
                int integerStart = integerStarts[i];
                int stringStart = stringStarts[i];
                int doubleStart = doubleStarts[i];
                int boolArrayStart = boolArrayStarts[i];
                int integerArrayStart = integerArrayStarts[i];
                int stringArrayStart = stringArrayStarts[i];
                int doubleArrayStart = doubleArrayStarts[i];
                int boolCount = GetIntervalCount(i, shapeCount, boolStarts, boolKeys.Length);
                int integerCount = GetIntervalCount(i, shapeCount, integerStarts, integerKeys.Length);
                int stringCount = GetIntervalCount(i, shapeCount, stringStarts, stringKeys.Length);
                int doubleCount = GetIntervalCount(i, shapeCount, doubleStarts, doubleKeys.Length);
                int boolArrayCount = GetIntervalCount(i, shapeCount, boolArrayStarts, boolArrayKeys.Length);
                int integerArrayCount = GetIntervalCount(i, shapeCount, integerArrayStarts, integerArrayKeys.Length);
                int stringArrayCount = GetIntervalCount(i, shapeCount, stringArrayStarts, stringArrayKeys.Length);
                int doubleArrayCount = GetIntervalCount(i, shapeCount, doubleArrayStarts, doubleArrayKeys.Length);

                defaultValues[i] = new AttributesValuesMap(boolKeys.Skip(boolStart).Take(boolCount).ToArray(),
                    boolValues.Skip(boolStart).Take(boolCount).ToArray(),
                    integerKeys.Skip(integerStart).Take(integerCount).ToArray(),
                    integerValues.Skip(integerStart).Take(integerCount).ToArray(),
                    stringKeys.Skip(stringStart).Take(stringCount).ToArray(), 
                    stringValues.Skip(stringStart).Take(stringCount).ToArray(),
                    doubleKeys.Skip(doubleStart).Take(doubleCount).ToArray(),
                    doubleValues.Skip(doubleStart).Take(doubleCount).ToArray(),
                    boolArrayKeys.Skip(boolArrayStart).Take(boolArrayCount).ToArray(),
                    boolArrayValues.Skip(boolArrayStart).Take(boolArrayCount).ToArray(),
                    integerArrayKeys.Skip(integerArrayStart).Take(integerArrayCount).ToArray(),
                    integerArrayValues.Skip(integerArrayStart).Take(integerArrayCount).ToArray(),
                    stringArrayKeys.Skip(stringArrayStart).Take(stringArrayCount).ToArray(),
                    stringArrayValues.Skip(stringArrayStart).Take(stringArrayCount).ToArray(),
                    doubleArrayKeys.Skip(doubleArrayStart).Take(doubleArrayCount).ToArray(),
                    doubleArrayValues.Skip(doubleArrayStart).Take(doubleArrayCount).ToArray());
            }
            return defaultValues;
        }

        public static int GetIntervalCount(int currentIndex, int shapeCount,  int[] starts, int itemCount)
        {
            return (currentIndex < shapeCount - 1 ? starts[currentIndex + 1] : itemCount) - starts[currentIndex];
        }

        public static GH_Structure<GH_Boolean> GetDefaultBooleans(string key, AttributesValuesMap[] defaultValues, bool isArray)
        {
            if (isArray)
            {
                List<List<bool>> values = new List<List<bool>>(defaultValues.Length);
                for(int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetBoolArray(key, out bool[] value)) values.Add(value.ToList());
                    else values.Add(new List<bool>());
                }
                return Utils.FromListToTree(values);
            }
            else
            {
                List<bool> values = new List<bool>(defaultValues.Length);
                for (int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetBool(key, out bool value)) values.Add(value);
                    else values.Add(false);
                }
                
                 return Utils.FromListToTree(values);
            }
        }

        public static GH_Structure<GH_String> GetDefaultStrings(string key, AttributesValuesMap[] defaultValues, bool isArray)
        {
            if (isArray)
            {
                List<List<string>> values = new List<List<string>>(defaultValues.Length);
                for (int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetStringArray(key, out string[] value)) values.Add(value.ToList());
                    else values.Add(new List<string>());
                }
                return Utils.FromListToTree(values);
            }
            else
            {
                List<string> values = new List<string>(defaultValues.Length);
                for (int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetString(key, out string value)) values.Add(value);
                    else values.Add("");
                }

                return Utils.FromListToTree(values);
            }
        }

        public static GH_Structure<GH_Integer> GetDefaultIntegers(string key, AttributesValuesMap[] defaultValues, bool isArray)
        {
            if (isArray)
            {
                List<List<int>> values = new List<List<int>>(defaultValues.Length);

                for (int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetIntegerArray(key, out int[] value)) values.Add(value.ToList());
                    else values.Add(new List<int>());
                }
                return Utils.FromListToTree(values);
            }
            else
            {
                List<int> values = new List<int>(defaultValues.Length);

                for (int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetInteger(key, out int value)) values.Add(value);
                    else values.Add(0);
                }

                return Utils.FromListToTree(values);
            }
        }

        public static GH_Structure<GH_Number> GetDefaultDoubles(string key, AttributesValuesMap[] defaultValues, bool isArray)
        {
            if (isArray)
            {
                List<List<double>> values = new List<List<double>>(defaultValues.Length);

                for (int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetDoubleArray(key, out double[] value)) values.Add(value.ToList());
                    else values.Add(new List<double>());
                }
                return Utils.FromListToTree(values);
            }
            else
            {
                List<double> values = new List<double>(defaultValues.Length);

                for (int i = 0; i < defaultValues.Length; ++i)
                {
                    if (defaultValues[i].GetDouble(key, out double value)) values.Add(value);
                    else values.Add(Double.NaN);
                }

                return Utils.FromListToTree(values);
            }
        }

        public static GH_Structure<GH_Colour> GetDefaultColors(string key, AttributesValuesMap[] defaultValues)
        {
            List<string> values = new List<string>(defaultValues.Length);
            for (int i = 0; i < defaultValues.Length; ++i)
            {
                if (defaultValues[i].GetString(key, out string value)) values.Add(value);
                else values.Add("");
            }

            return Utils.HexListToColorTree(values);
        }
    }
}
