using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;

namespace GrasshopperPRT
{
    public enum AnnotationArgumentType { AAT_VOID, AAT_BOOL, AAT_FLOAT, AAT_STR, AAT_INT, AAT_UNKNOWN, AAT_BOOL_ARRAY, AAT_FLOAT_ARRAY, AAT_STR_ARRAY};
    public enum AttributeAnnotation { A_COLOR = 0, A_RANGE, A_ENUM, A_FILE, A_DIR, A_NOANNOT };
    public enum EnumAnnotationType { ENUM_DOUBLE = 0, ENUM_BOOL, ENUM_STRING };


    public class Annotation
    {
        public Annotation(AttributeAnnotation annot = AttributeAnnotation.A_NOANNOT)
        {
            this.mAnnotType = annot;
        }

        public virtual string GetDescription() { return ""; }

        public virtual bool IsColor() { return false; }

        protected AttributeAnnotation mAnnotType;
    }

    public class AnnotationRange: Annotation
    {
        public AnnotationRange(double min, double max, double stepsize = 0, bool restricted=true): base(AttributeAnnotation.A_RANGE)
        {
            this.mMin = min;
            this.mMax = max;
            this.mStepSize = stepsize;
            this.mRestricted = restricted;
        }

        public override string GetDescription()
        {
            return "Range: Min: " + mMin + " Max: " + mMax + ((mStepSize > 0)? " StepSize: " + mStepSize.ToString() : "") + (mRestricted? " Restricted" : "");
        }

        double mMin;
        double mMax;
        double mStepSize;
        bool mRestricted;
    }

    public class AnnotationEnum<T>: Annotation
    {
        public AnnotationEnum(T[] enumList): base(AttributeAnnotation.A_ENUM)
        {
            mEnumList = enumList;
        }

        public override string GetDescription()
        {
            string concatenation = mEnumList.Aggregate<T, string>("", (accu, val) => accu.ToString() + " - " + val.ToString() + "\n");

            return "Enumeration, allowed values are:\n" + concatenation;
        }

        private T[] mEnumList;
    }

    public class AnnotationColor: Annotation
    {
        public AnnotationColor(): base(AttributeAnnotation.A_COLOR) { }

        public override string GetDescription()
        {
            return "This attribute accepts a color object.";
        }

        public override bool IsColor() { return true; }
    }

    public class AnnotationFile: Annotation
    {
        public AnnotationFile(): base(AttributeAnnotation.A_FILE) { }

        public override string GetDescription()
        {
            return "This attribute accepts a file path.";
        }
    }

    public class AnnotationDir: Annotation
    {
        public AnnotationDir():  base(AttributeAnnotation.A_DIR) { }

        public override string GetDescription()
        {
            return "This attribute accepts a folder path.";
        }
    }


    public class RuleAttribute
    {
        public string mRuleFile;
        public string mFullName;
        public string mNickname;
        public AnnotationArgumentType mAttribType;
        public List<Annotation> mAnnotations;
        public string mGroup;

        public RuleAttribute(string name = "Default$DefaultName", string nickname = "DefaultName", string ruleFile = "rule", AnnotationArgumentType type = AnnotationArgumentType.AAT_INT, string group = "")
        {
            this.mRuleFile = ruleFile;
            this.mFullName = name;
            this.mNickname = nickname;
            this.mAttribType = type;
            this.mAnnotations = new List<Annotation>();
            this.mGroup = group;
        }

        public IGH_Param GetInputParameter()
        {

            switch (this.mAttribType)
            {
                case AnnotationArgumentType.AAT_BOOL_ARRAY:
                case AnnotationArgumentType.AAT_BOOL:
                    {
                        Param_Boolean param_bool = new Param_Boolean
                        {
                            Name = mFullName,
                            NickName = mNickname,
                            Description = GetDescriptions(),
                            Optional = true,
                            Access = GetAccess(),
                        };

                        return param_bool;
                    }
                case AnnotationArgumentType.AAT_FLOAT_ARRAY:
                case AnnotationArgumentType.AAT_INT:
                case AnnotationArgumentType.AAT_FLOAT:
                    {
                        Param_Number param_number = new Param_Number
                        {
                            Name = mFullName,
                            NickName = mNickname,
                            Description = GetDescriptions(),
                            Optional = true,
                            Access = GetAccess()
                        };
                        
                        return param_number;
                    }
                case AnnotationArgumentType.AAT_STR_ARRAY:
                case AnnotationArgumentType.AAT_STR:
                    {
                        // check for color parameter
                        if (mAnnotations.Any(x => x.IsColor()))
                        {
                            Param_Colour param_color = new Param_Colour
                            {
                                Name = mFullName,
                                NickName = mNickname,
                                Description = GetDescriptions(),
                                Optional = true,
                                Access = GetAccess()
                            };
                            return param_color;
                        }
                        else
                        {
                            Param_String param_str = new Param_String
                            {
                                Name = mFullName,
                                NickName = mNickname,
                                Description = GetDescriptions(),
                                Optional = true,
                                Access = GetAccess()
                            };
                            return param_str;
                        }
                    }
                default:
                    Param_GenericObject param = new Param_GenericObject
                    {
                        Name = mFullName,
                        NickName = mNickname,
                        Description = GetDescriptions(),
                        Optional = true,
                        Access = GetAccess()
                    };
                    
                    return param;
            }
        }

        private GH_ParamAccess GetAccess()
        {
            switch (this.mAttribType)
            {
                case AnnotationArgumentType.AAT_BOOL_ARRAY:
                case AnnotationArgumentType.AAT_FLOAT_ARRAY:
                case AnnotationArgumentType.AAT_STR_ARRAY:
                    return GH_ParamAccess.list;
                default:
                    return GH_ParamAccess.item;
            }
        }

        private string GetDescriptions()
        {
            string description = "";
            if(mGroup.Length > 0)
            {
                description = "Group " + mGroup + "\n";
            }

            return description + mAnnotations.Aggregate<Annotation, string>("", (left, right) => left + " " + right.GetDescription());
        }
    }
}
