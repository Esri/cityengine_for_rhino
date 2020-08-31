using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Types;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GrasshopperPRT
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
        private string mKey;
        private ReportTypes mType;

        public override bool IsValid => true;

        public override string TypeName => "ReportAttribute";

        public override string TypeDescription => "Contains a cga report {key : value} where key is the report name and value is either a double, a boolean or a string.";

        public int ShapeID { get; }
        public string StringValue { get; set; } = "";
        public double DoubleValue { get; set; } = 0;
        public bool BoolValue { get; set; } = false;

        public ReportAttribute(int shapeID, string key, ReportTypes type)
           : base()
        {
            this.ShapeID = shapeID;
            this.mKey = key;
            this.mType = type;
        }

        public ReportAttribute(int shapeID, string key, string nickname, ReportTypes type)
            : base()
        {
            this.ShapeID = shapeID;
            this.mKey = key;
            this.mType = type;
        }

        public string getFormatedValue()
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

        public string getKey() { return mKey; }
        public ReportTypes getType() { return mType; }

        public static ReportAttribute CreateReportAttribute(int shapeID, string name, string nickname, ReportTypes type, string value)
        {
            ReportAttribute report = new ReportAttribute(shapeID, name, nickname, type);
            report.StringValue = value;
            return report;
        }

        public static ReportAttribute CreateReportAttribute(int shapeID, string name, string nickname, ReportTypes type, double value)
        {
            ReportAttribute report = new ReportAttribute(shapeID, name, nickname, type);
            report.DoubleValue = value;
            return report;
        }

        public static ReportAttribute CreateReportAttribute(int shapeID, string name, string nickname, ReportTypes type, bool value)
        {
            ReportAttribute report = new ReportAttribute(shapeID, name, nickname, type);
            report.BoolValue = value;
            return report;
        }

        public override IGH_Goo Duplicate()
        {
            return this;
        }

        public override string ToString()
        {
            return "[ ShapeID: " + ShapeID + ", Key: " + mKey + ", Value: " + getFormatedValue() + " ]";
        }

        public string ToNiceString()
        {
            return "  " + mKey + " : " + getFormatedValue();
        }
    }
}
