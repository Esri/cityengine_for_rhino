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
        private List<int> doubleStarts = new List<int>();
        private List<int> stringStarts = new List<int>();
        private List<int> boolArrayStarts = new List<int>();
        private List<int> doubleArrayStarts = new List<int>();
        private List<int> stringArrayStarts = new List<int>();

        public List<string> boolKeys = new List<string>();
        public List<string> doubleKeys = new List<string>();
        public List<string> stringKeys = new List<string>();
        public List<string> boolArrayKeys = new List<string>();
        public List<string> doubleArrayKeys = new List<string>();
        public List<string> stringArrayKeys = new List<string>();

        public List<bool> boolValues = new List<bool>();
        public List<double> doubleValues = new List<double>();
        public List<string> stringValues = new List<string>();
        public List<string> boolArrayValues = new List<string>();
        public List<string> doubleArrayValues = new List<string>();
        public List<string> stringArrayValues = new List<string>();

        public RuleAttributesMap() { }

        public void StartNewSection()
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
