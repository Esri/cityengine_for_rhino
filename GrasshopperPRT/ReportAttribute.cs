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
        int mShapeID;
        string mKey;
        ReportTypes mType;

        // the value of the report will be one of these
        private string mStringValue = "";
        private double mDoubleValue = 0;
        private bool mBoolValue = false;

        public override bool IsValid => true;

        public override string TypeName => "ReportAttribute";

        public override string TypeDescription => "Contains a cga report {key : value} where key is the report name and value is either a double, a boolean or a string.";

        public int ShapeID { get => mShapeID; }
        public string StringValue { get => mStringValue; set => mStringValue = value; }
        public double DoubleValue { get => mDoubleValue; set => mDoubleValue = value; }
        public bool BoolValue { get => mBoolValue; set => mBoolValue = value; }

        public ReportAttribute(int shapeID, string key, ReportTypes type)
           : base()
        {
            this.mShapeID = shapeID;
            this.mKey = key;
            this.mType = type;
        }

        public ReportAttribute(int shapeID, string key, string nickname, ReportTypes type)
            : base()
        {
            this.mShapeID = shapeID;
            this.mKey = key;
            this.mType = type;
        }

        public IGH_Param ToIGH_Param()
        {
            switch(mType)
            {
                case ReportTypes.PT_BOOL:
                    return new Param_Boolean { Name=mKey, NickName=mKey, Access=GH_ParamAccess.list, Optional=true };
                case ReportTypes.PT_FLOAT:
                    return new Param_Number { Name = mKey, NickName = mKey, Access = GH_ParamAccess.list, Optional = true };
                case ReportTypes.PT_STRING:
                    return new Param_String { Name = mKey, NickName = mKey, Access = GH_ParamAccess.list, Optional = true };
                default:
                    return null;
            }
        }

        public string getFormatedValue()
        {
            switch(mType)
            {
                case ReportTypes.PT_FLOAT:
                    return mDoubleValue.ToString();
                case ReportTypes.PT_STRING:
                    return mStringValue;
                case ReportTypes.PT_BOOL:
                    return mBoolValue.ToString();
                default:
                    return "Undefined";
            }
        }

        public string getKey() { return mKey; }
        public ReportTypes getType() { return mType; }

        public static ReportAttribute CreateReportAttribute(int shapeID, string name, string nickname, ReportTypes type, string value)
        {
            ReportAttribute report = new ReportAttribute(shapeID, name, nickname, type);
            report.mStringValue = value;
            return report;
        }

        public static ReportAttribute CreateReportAttribute(int shapeID, string name, string nickname, ReportTypes type, double value)
        {
            ReportAttribute report = new ReportAttribute(shapeID, name, nickname, type);
            report.mDoubleValue = value;
            return report;
        }

        public static ReportAttribute CreateReportAttribute(int shapeID, string name, string nickname, ReportTypes type, bool value)
        {
            ReportAttribute report = new ReportAttribute(shapeID, name, nickname, type);
            report.mBoolValue = value;
            return report;
        }

        public override IGH_Goo Duplicate()
        {
            return this;
        }

        public override string ToString()
        {
            string value;

            if (mType == ReportTypes.PT_FLOAT) value = mDoubleValue.ToString();
            else if (mType == ReportTypes.PT_BOOL) value = mBoolValue.ToString();
            else if (mType == ReportTypes.PT_STRING) value = mStringValue;
            else value = "UNDEFINED";

            return "[ ShapeID: " + mShapeID + ", Key: " + mKey + ", Value: " +  value + " ]";
        }

        public string ToNiceString()
        {
            string value;

            if (mType == ReportTypes.PT_FLOAT) value = mDoubleValue.ToString();
            else if (mType == ReportTypes.PT_BOOL) value = mBoolValue.ToString();
            else if (mType == ReportTypes.PT_STRING) value = mStringValue;
            else value = "UNDEFINED";

            return "  " + mKey + " : " + value;
        }
    }
}
