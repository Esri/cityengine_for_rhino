using System.Runtime.InteropServices;

namespace GrasshopperPRT
{
    public enum AnnotationArgumentType { AAT_VOID, AAT_BOOL, AAT_FLOAT, AAT_STR, AAT_INT, AAT_UNKNOWN, AAT_BOOL_ARRAY, AAT_FLOAT_ARRAY, AAT_STR_ARRAY};

    public struct RuleAttribute
    {
        public string mRuleFile;
        public string mFullName;
        public string mNickname;
        public AnnotationArgumentType attribType;

        public RuleAttribute(string name = "Default$DefaultName", string nickname = "DefaultName", string ruleFile = "rule", AnnotationArgumentType type = AnnotationArgumentType.AAT_INT)
        {
            this.mRuleFile = ruleFile;
            this.mFullName = name;
            this.mNickname = nickname;
            this.attribType = type;
        }
    }
}
