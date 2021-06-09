using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Special;

namespace PumaGrasshopper
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

        public virtual string GetDescription(string ruleName) { return ""; }

        public virtual bool IsColor() { return false; }

        public virtual IGH_Param GetGhSpecializedParam() { return new Param_GenericObject(); }

        public AttributeAnnotation GetAnnotationType() { return mAnnotType; }

        protected AttributeAnnotation mAnnotType;
    }

    public class AnnotationRange: Annotation
    {
        public AnnotationRange(double min, double max, double stepsize = 0, bool restricted=true): base(AttributeAnnotation.A_RANGE)
        {
            mMin = min;
            mMax = max;
            mStepSize = stepsize;
            mRestricted = restricted;
        }

        public override string GetDescription(string ruleName)
        {
            return "Range: Min: " + mMin + " Max: " + mMax + ((mStepSize > 0)? " StepSize: " + mStepSize.ToString() : "") + (mRestricted? " Restricted" : "");
        }

        public override IGH_Param GetGhSpecializedParam()
        {
            GH_NumberSlider slider = new GH_NumberSlider();
            
            slider.Slider.Minimum = (decimal)mMin;
            slider.Slider.Maximum = (decimal)mMax;
            slider.Slider.Value = (decimal)((mMin + mMax) * 0.5);
            
            slider.Slider.Type = Grasshopper.GUI.Base.GH_SliderAccuracy.Float;

            if (Utils.isInteger(mStepSize) && Utils.isInteger(mMin) && mStepSize != 0)
                slider.Slider.Type = Grasshopper.GUI.Base.GH_SliderAccuracy.Integer;

            return slider;
        }

        private double mMax;
        private double mMin;
        private double mStepSize;
        private bool mRestricted;
    }

    public class AnnotationEnum<T>: Annotation
    {
        public AnnotationEnum(T[] enumList, bool restricted): base(AttributeAnnotation.A_ENUM)
        {
            mEnumList = enumList;
            mRestricted = restricted;
        }

        public override string GetDescription(string ruleName)
        {
            string concatenation = mEnumList.Aggregate<T, string>("",
                (accu, val) => {
                    return val == null ? accu : accu.ToString() + " - " + val.ToString() + "\n";
                });

            return "Enumeration, allowed values are:\n" + concatenation;
        }

        public override IGH_Param GetGhSpecializedParam()
        {
            GH_ValueList vList = new GH_ValueList();
            vList.ListItems.Clear();
            foreach (var item in mEnumList)
            {
                if(item != null)
                    vList.ListItems.Add(new GH_ValueListItem(item.ToString(), String.Format("\"{0}\"",item.ToString())));
            }

            return vList;
        }

        private bool mRestricted;
        private T[] mEnumList;
    }

    public class AnnotationColor: Annotation
    {
        public AnnotationColor(): base(AttributeAnnotation.A_COLOR) { }

        public override string GetDescription(string ruleName)
        {
            return "This attribute accepts a color object.";
        }

        public override bool IsColor() { return true; }
    }

    public class AnnotationFile: Annotation
    {
        public AnnotationFile(): base(AttributeAnnotation.A_FILE) { }

        public override string GetDescription(string ruleName)
        {
            return "This attribute accepts a file path.";
        }

        public override IGH_Param GetGhSpecializedParam()
        {
            return new Param_FilePath();
        }
    }

    public class AnnotationDir: Annotation
    {
        public AnnotationDir():  base(AttributeAnnotation.A_DIR) { }

        public override string GetDescription(string ruleName)
        {
            return "This attribute accepts a folder path.";
        }

        public override IGH_Param GetGhSpecializedParam()
        {
            return new Param_FilePath();
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
                        BooleanRuleAttribute param_bool = new BooleanRuleAttribute(mAnnotations, mGroup)
                        {
                            Name = mFullName,
                            NickName = mNickname,
                            Description = GetDescriptions(),
                            Optional = true,
                            Access = GetAccess()
                        };

                        return param_bool;
                    }
                case AnnotationArgumentType.AAT_FLOAT_ARRAY:
                case AnnotationArgumentType.AAT_INT:
                case AnnotationArgumentType.AAT_FLOAT:
                    {
                        NumberRuleAttribute param_number = new NumberRuleAttribute(mAnnotations, mGroup)
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
                            ColourRuleAttribute param_color = new ColourRuleAttribute(mAnnotations, mGroup)
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
                            StringRuleAttribute param_str = new StringRuleAttribute(mAnnotations, mGroup)
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

        public bool IsColor()
        {
            return mAnnotations.Any(x => x.IsColor());
        }

        private GH_ParamAccess GetAccess()
        {
            return GH_ParamAccess.tree;
        }

        private string GetDescriptions()
        {
            string description = "";
            if(mGroup.Length > 0)
            {
                description = "Group " + mGroup + "\n";
            }

            return description + mAnnotations.Aggregate<Annotation, string>("", (left, right) => left + " " + right.GetDescription(mFullName));
        }
    }
}
