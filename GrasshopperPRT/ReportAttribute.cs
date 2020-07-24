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

    class ReportAttribute
    {
        string mKey;
        ReportTypes mType;

        public ReportAttribute(string key, ReportTypes type)
        {
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
    }
}
