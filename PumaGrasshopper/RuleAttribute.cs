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

using System;
using System.Collections.Generic;
using System.Linq;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;

namespace PumaGrasshopper
{
    public class RuleAttribute
    {
        public string mRuleFile;
        public string mFullName;
        public string mNickname;
        public Annotations.AnnotationArgumentType mAttribType;
        public List<Annotations.Base> mAnnotations;
        public string mGroup;

        public RuleAttribute(string name, string nickname, string ruleFile, Annotations.AnnotationArgumentType type, string group)
        {
            this.mRuleFile = ruleFile;
            this.mFullName = name;
            this.mNickname = nickname;
            this.mAttribType = type;
            this.mAnnotations = new List<Annotations.Base>();
            this.mGroup = group;
        }

        public IGH_Param CreateInputParameter(DefaultValuesMap[] defaultValuesMap)
        {
            switch (this.mAttribType)
            {
                case Annotations.AnnotationArgumentType.AAT_BOOL_ARRAY:
                case Annotations.AnnotationArgumentType.AAT_BOOL:
                    {
                        var param_bool = new AttributeParameter.Boolean(mGroup, IsArray())
                        {
                            Name = mFullName,
                            NickName = mNickname,
                            Description = GetDescriptions(),
                            Optional = true,
                            Access = GetAccess()
                        };
                        param_bool.SetPersistentData(DefaultValuesMap.GetDefaultBooleans(mFullName, defaultValuesMap, IsArray()));

                        return param_bool;
                    }
                case Annotations.AnnotationArgumentType.AAT_FLOAT_ARRAY:
                case Annotations.AnnotationArgumentType.AAT_INT:
                case Annotations.AnnotationArgumentType.AAT_FLOAT:
                    {
                        var param_number = new AttributeParameter.Number(mAnnotations, mGroup, IsArray())
                        {
                            Name = mFullName,
                            NickName = mNickname,
                            Description = GetDescriptions(),
                            Optional = true,
                            Access = GetAccess()
                        };
                        param_number.SetPersistentData(DefaultValuesMap.GetDefaultDoubles(mFullName, defaultValuesMap, IsArray()));
                        
                        return param_number;
                    }
                case Annotations.AnnotationArgumentType.AAT_STR_ARRAY:
                case Annotations.AnnotationArgumentType.AAT_STR:
                    {
                        // check for color parameter
                        if (mAnnotations.Any(x => x.IsColor()))
                        {
                            var param_color = new AttributeParameter.Colour(mGroup)
                            {
                                Name = mFullName,
                                NickName = mNickname,
                                Description = GetDescriptions(),
                                Optional = true,
                                Access = GetAccess()
                            };
                            param_color.SetPersistentData(DefaultValuesMap.GetDefaultColors(mFullName, defaultValuesMap));
                            return param_color;
                        }
                        else
                        {
                            var param_str = new AttributeParameter.String(mAnnotations, mGroup, IsArray())
                            {
                                Name = mFullName,
                                NickName = mNickname,
                                Description = GetDescriptions(),
                                Optional = true,
                                Access = GetAccess()
                            };
                            param_str.SetPersistentData(DefaultValuesMap.GetDefaultStrings(mFullName, defaultValuesMap, IsArray()));
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

        public bool IsArray()
        {
            return mAttribType == Annotations.AnnotationArgumentType.AAT_BOOL_ARRAY 
                || mAttribType == Annotations.AnnotationArgumentType.AAT_FLOAT_ARRAY 
                || mAttribType == Annotations.AnnotationArgumentType.AAT_STR_ARRAY;
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

            if(this.IsArray())
            {
                description += "Expects a list of items by initial shape.\n";
            } else
            {
                description += "Expects a single item by initial shape.\n";
            }

            return description + mAnnotations.Aggregate("", (left, right) => left + " " + right.GetDescription(mFullName));
        }
    }
}
