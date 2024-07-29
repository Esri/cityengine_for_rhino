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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PumaGrasshopper
{
    public class RuleAttributesMap
    {
        private List<int> boolStarts = new List<int>();
        private List<int> integerStarts = new List<int>();
        private List<int> doubleStarts = new List<int>();
        private List<int> stringStarts = new List<int>();
        private List<int> boolArrayStarts = new List<int>();
        private List<int> integerArrayStarts = new List<int>();
        private List<int> doubleArrayStarts = new List<int>();
        private List<int> stringArrayStarts = new List<int>();

        public List<string> boolKeys = new List<string>();
        public List<string> integerKeys = new List<string>();
        public List<string> doubleKeys = new List<string>();
        public List<string> stringKeys = new List<string>();
        public List<string> boolArrayKeys = new List<string>();
        public List<string> integerArrayKeys = new List<string>();
        public List<string> doubleArrayKeys = new List<string>();
        public List<string> stringArrayKeys = new List<string>();

        public List<bool> boolValues = new List<bool>();
        public List<int> integerValues = new List<int>();
        public List<double> doubleValues = new List<double>();
        public List<string> stringValues = new List<string>();
        public List<string> boolArrayValues = new List<string>();
        public List<string> integerArrayValues = new List<string>();
        public List<string> doubleArrayValues = new List<string>();
        public List<string> stringArrayValues = new List<string>();

        public RuleAttributesMap() { }

        public void StartNewSection()
        {
            boolStarts.Add(boolKeys.Count);
            integerStarts.Add(integerKeys.Count);
            doubleStarts.Add(doubleKeys.Count);
            stringStarts.Add(stringKeys.Count);
            boolArrayStarts.Add(boolArrayKeys.Count);
            integerArrayStarts.Add(integerArrayKeys.Count);
            doubleArrayStarts.Add(doubleArrayKeys.Count);
            stringArrayStarts.Add(stringArrayKeys.Count);
        }

        public void AddDouble(string key, double value)
        {
            doubleKeys.Add(key);
            doubleValues.Add(value);
        }

        public void AddString(string key, string value)
        {
            stringKeys.Add(key);
            stringValues.Add(value);
        }

        public void AddBoolean(string key, bool value)
        {
            boolKeys.Add(key);
            boolValues.Add(value);
        }

        public void AddInteger(string key, int value)
        {
            integerKeys.Add(key);
            integerValues.Add(value);
        }

        public void AddBoolArray(string key, bool[] values)
        {
            boolArrayKeys.Add(key);
            boolArrayValues.Add(Utils.ToCeArray(values));
        }

        public void AddIntegerArray(string key, int[] values)
        {
            integerArrayKeys.Add(key);
            integerArrayValues.Add(Utils.ToCeArray(values));
        }

        public void AddDoubleArray(string key, double[] values)
        {
            doubleArrayKeys.Add(key);
            doubleArrayValues.Add(Utils.ToCeArray(values));
        }

        public void AddStringArray(string key, string[] values)
        {
            stringArrayKeys.Add(key);
            stringArrayValues.Add(Utils.ToCeArray(values));
        }

        public int[] GetStringStarts() => stringStarts.ToArray();
        public int[] GetBoolStarts() => boolStarts.ToArray();
        public int[] GetIntegerStarts() => integerStarts.ToArray();
        public int[] GetDoubleStarts() => doubleStarts.ToArray();

        public int[] GetStringArrayStarts() => stringArrayStarts.ToArray();
        public int[] GetDoubleArrayStarts() => doubleArrayStarts.ToArray();
        public int[] GetBoolArrayStarts() => boolArrayStarts.ToArray();
        public int[] GetIntegerArrayStarts() => integerArrayStarts.ToArray();
    }
}
