using Rhino.Runtime.InteropWrappers;
using System;
using System.Collections.Generic;
using System.Linq;

namespace PumaGrasshopper
{
    class AttributeInterop
    {
        public int Count;
        protected SimpleArrayInt Starts;
        protected ClassArrayString Keys;

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
    }

    class InteropWrapperString : AttributeInterop
    {
        private ClassArrayString Values;

        public InteropWrapperString(int[] starts, Dictionary<string, string> attributes) : base(starts, attributes.Count)
        {
            Keys = new ClassArrayString();
            Values = new ClassArrayString();

            foreach (var attribute in attributes)
            {
                Keys.Add(attribute.Key);
                Values.Add(attribute.Value);
            }
        }

        public new void Dispose()
        {
            base.Dispose();
            Values.Dispose();
        }

        public IntPtr ValuesPtr() => Values.ConstPointer();
    }

    class InteropWrapperDouble: AttributeInterop
    {
        private SimpleArrayDouble Values;

        public InteropWrapperDouble(int[] starts, Dictionary<string, double> attributes): base(starts, attributes.Count)
        {
            Keys = new ClassArrayString();
            Values = new SimpleArrayDouble(attributes.Values);

            foreach(var key in attributes.Keys)
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
    }

    class InteropWrapperBoolean: AttributeInterop
    {
        private SimpleArrayInt Values;

        public InteropWrapperBoolean(int[] starts, Dictionary<string, bool> attributes): base(starts, attributes.Count)
        {
            Keys = new ClassArrayString();
            Values = new SimpleArrayInt(Array.ConvertAll<bool, int>(attributes.Values.ToArray(), x => Convert.ToInt32(x)));

            foreach (var attribute in attributes)
            {
                Keys.Add(attribute.Key);
            }
        }

        public new void Dispose()
        {
            base.Dispose();
            Values.Dispose();
        }

        public IntPtr ValuesPtr() => Values.ConstPointer();
    }
}