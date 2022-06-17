using Rhino.Runtime.InteropWrappers;
using System;
using System.Collections.Generic;
using System.Linq;

namespace PumaGrasshopper
{
    public class AttributeInterop
    {
        public int Count;
        protected SimpleArrayInt Starts;
        protected ClassArrayString Keys;

        public AttributeInterop()
        {
            Starts = new SimpleArrayInt();
            Count = 0;
        }

        public AttributeInterop(int[] starts, int count)
        {
            Starts = new SimpleArrayInt(starts);
            Count = count;
        }

        public void Dispose()
        {
            Starts.Dispose();
            Keys.Dispose();
        }

        public IntPtr StartsPtr() => Starts.ConstPointer();
        public IntPtr KeysPtr() => Keys.ConstPointer();

        public IntPtr StartsNonConstPtr() => Starts.NonConstPointer();
        public IntPtr KeysNonConstPtr() => Keys.NonConstPointer();

        public int[] StartsToArray() => Starts.ToArray();
        public string[] KeysToArray() => Keys.ToArray();
    }

    public class InteropWrapperString : AttributeInterop
    {
        private ClassArrayString Values;

        public InteropWrapperString(): base()
        {
            Keys = new ClassArrayString();
            Values = new ClassArrayString();
        }

        public InteropWrapperString(int[] starts, ref List<string> keys, ref List<string> values) : base(starts, keys.Count)
        {
            Keys = new ClassArrayString();
            Values = new ClassArrayString();

            
            for (int i = 0; i < keys.Count; ++i)
            {
                Keys.Add(keys[i]);
                Values.Add(values[i]);
            }
        }

        public new void Dispose()
        {
            base.Dispose();
            Values.Dispose();
        }

        public IntPtr ValuesPtr() => Values.ConstPointer();
        public IntPtr ValuesNonConstPtr() => Values.NonConstPointer();

        public string[] ValuesToArray() => Values.ToArray();
    }

    public class InteropWrapperDouble: AttributeInterop
    {
        private SimpleArrayDouble Values;

        public InteropWrapperDouble(): base()
        {
            Keys = new ClassArrayString();
            Values = new SimpleArrayDouble();
        }

        public InteropWrapperDouble(int[] starts, ref List<string> keys, ref List<double> values): base(starts, keys.Count)
        {
            Keys = new ClassArrayString();
            Values = new SimpleArrayDouble(values);

            foreach(var key in keys)
            {
                Keys.Add(key);
            }
        }

        public new void Dispose()
        {
            base.Dispose();
            Values.Dispose();
        }

        public IntPtr ValuesPtr() => Values.ConstPointer();
        public IntPtr ValuesNonConstPtr() => Values.NonConstPointer();

        public double[] ValuesToArray() => Values.ToArray();
    }

    public class InteropWrapperBoolean: AttributeInterop
    {
        private SimpleArrayInt Values;

        public InteropWrapperBoolean(): base()
        {
            Keys = new ClassArrayString();
            Values = new SimpleArrayInt();
        }

        public InteropWrapperBoolean(int[] starts, ref List<string> keys, ref List<bool> values): base(starts, keys.Count)
        {
            Keys = new ClassArrayString();
            Values = new SimpleArrayInt(Array.ConvertAll<bool, int>(values.ToArray(), x => Convert.ToInt32(x)));

            foreach (var key in keys)
            {
                Keys.Add(key);
            }
        }

        public new void Dispose()
        {
            base.Dispose();
            Values.Dispose();
        }

        public IntPtr ValuesPtr() => Values.ConstPointer();
        public IntPtr ValuesNonConstPtr() => Values.NonConstPointer();

        public bool[] ValuesToArray() => Array.ConvertAll(Values.ToArray(), value => Convert.ToBoolean(value));
    }
}