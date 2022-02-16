using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PumaGrasshopper
{
    public class RuleAttributesMap
    {
        private List<int> boolStarts;
        private List<int> doubleStarts;
        private List<int> stringStarts;
        private List<int> boolArrayStarts;
        private List<int> doubleArrayStarts;
        private List<int> stringArrayStarts;

        public List<string> boolKeys;
        public List<string> doubleKeys;
        public List<string> stringKeys;
        public List<string> boolArrayKeys;
        public List<string> doubleArrayKeys;
        public List<string> stringArrayKeys;

        public List<bool> boolValues;
        public List<double> doubleValues;
        public List<string> stringValues;
        public List<string> boolArrayValues;
        public List<string> doubleArrayValues;
        public List<string> stringArrayValues;

        public RuleAttributesMap() {
            boolStarts = new List<int>();
            doubleStarts = new List<int>();
            stringStarts = new List<int>();
            boolArrayStarts = new List<int>();
            doubleArrayStarts = new List<int>();
            stringArrayStarts = new List<int>();
        }

        public void StartShape()
        {
            boolStarts.Add(boolKeys.Count);
            doubleStarts.Add(doubleKeys.Count);
            stringStarts.Add(stringKeys.Count);
            boolArrayStarts.Add(boolArrayKeys.Count);
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

        public void AddBoolArray(string key, bool[] values)
        {
            boolArrayKeys.Add(key);
            boolArrayValues.Add(Utils.ToCeArray(values));
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
        public int[] GetDoubleStarts() => doubleStarts.ToArray();

        public int[] GetStringArrayStarts() => stringArrayStarts.ToArray();
        public int[] GetDoubleArrayStarts() => doubleArrayStarts.ToArray();
        public int[] GetBoolArrayStarts() => boolArrayStarts.ToArray();
    }
}
