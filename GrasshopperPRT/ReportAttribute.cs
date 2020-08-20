using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;
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

    public class ReportAttribute: Param_GenericObject
    {
        public override GH_Exposure Exposure => GH_Exposure.hidden;

        string mKey;
        ReportTypes mType;

        // the value of the report will be one of these
        string mStringValue = "";
        double mDoubleValue = 0;
        bool mBoolValue = false;

        public ReportAttribute(string key, ReportTypes type)
        {
            this.Name = key;
            this.NickName = key;
            this.mKey = key;
            this.mType = type;
            this.Optional = true;
            this.Access = GH_ParamAccess.item;
        }

        public ReportAttribute(string key, string nickname, ReportTypes type)
        {
            this.Name = key;
            this.NickName = nickname;
            this.mKey = key;
            this.mType = type;
            this.Optional = true;
            this.Access = GH_ParamAccess.item;
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

        public string getKey() { return mKey; }
        public ReportTypes getType() { return mType; }



        public static ReportAttribute CreateReportAttribute(string name, string nickname, ReportTypes type)
        {
            ReportAttribute report = new ReportAttribute(name, nickname, type);

            return report;
        }
    }
}
