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

using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Types;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PumaGrasshopper
{
    public enum ReportTypes
    {
        PT_PT_UNDEFINED,		///< Undefined type.
		PT_STRING,				///< String type.
		PT_FLOAT,				///< Float type.
		PT_BOOL,				///< Boolean type.
		PT_INT,					///< Integer type.
		PT_STRING_ARRAY,		///< String array type.
		PT_FLOAT_ARRAY,			///< Float array type.
		PT_BOOL_ARRAY,			///< Boolean array type.
		PT_INT_ARRAY,			///< Integer array type.
		PT_BLIND_DATA,			///< Blind data type.
		PT_BLIND_DATA_ARRAY,	///< Blind data array type.
		PT_COUNT
    };

    public class ReportAttribute: GH_Goo<Param_GenericObject>
    {
        private readonly string mKey;
        private readonly ReportTypes mType;

        public override bool IsValid => true;

        public override string TypeName => "ReportAttribute";

        public override string TypeDescription => "Contains a cga report {key : value} where key is the report name and value is either a double, a boolean or a string.";

        public int InitialShapeIndex { get; }
        public string StringValue { get; set; } = "";
        public double DoubleValue { get; set; } = 0;
        public bool BoolValue { get; set; } = false;

        public ReportAttribute(int initialShapeIndex, string key, ReportTypes type)
           : base()
        {
            this.InitialShapeIndex = initialShapeIndex;
            this.mKey = key;
            this.mType = type;
        }

        public string GetFormatedValue()
        {
            switch(mType)
            {
                case ReportTypes.PT_FLOAT:
                    return DoubleValue.ToString();
                case ReportTypes.PT_STRING:
                    return StringValue;
                case ReportTypes.PT_BOOL:
                    return BoolValue.ToString();
                default:
                    return "UNDEFINED";
            }
        }

        public string GetReportKey() { return mKey; }

        public ReportTypes GetReportType() { return mType; }

        public static ReportAttribute CreateReportAttribute(int initialShapeIndex, string name, ReportTypes type, string value)
        {
            ReportAttribute report = new ReportAttribute(initialShapeIndex, name, type)
            {
                StringValue = value
            };
            return report;
        }

        public static ReportAttribute CreateReportAttribute(int initialShapeIndex, string name, ReportTypes type, double value)
        {
            ReportAttribute report = new ReportAttribute(initialShapeIndex, name, type)
            {
                DoubleValue = value
            };
            return report;
        }

        public static ReportAttribute CreateReportAttribute(int initialShapeIndex, string name, ReportTypes type, bool value)
        {
            ReportAttribute report = new ReportAttribute(initialShapeIndex, name, type)
            {
                BoolValue = value
            };
            return report;
        }

        public override IGH_Goo Duplicate()
        {
            return this;
        }

        public override string ToString()
        {
            return "[ InitialShapeIndex: " + InitialShapeIndex + ", Key: " + mKey + ", Value: " + GetFormatedValue() + " ]";
        }

        public string ToNiceString()
        {
            return "  " + mKey + " : " + GetFormatedValue();
        }
    }
}
