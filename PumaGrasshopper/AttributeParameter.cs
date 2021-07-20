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
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Special;
using Grasshopper.Kernel.Types;
using Rhino.Runtime.InteropWrappers;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace PumaGrasshopper.AttributeParameter
{
    public class Boolean : Param_Boolean
    {
        private readonly string mGroupName;
        private readonly bool mExpectsArray;

        public Boolean(string groupName = "", bool expectsArray = false): base()
        {
            mGroupName = groupName;
            mExpectsArray = expectsArray;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param;

            if (mExpectsArray)
            {
                SimpleArrayInt boolArray = new SimpleArrayInt();
                var pBoolArray = boolArray.NonConstPointer();
                bool hasDefault = PRTWrapper.GetDefaultValueBooleanArray(Name, pBoolArray);

                param = new Param_Boolean();

                if(hasDefault)
                {
                    List<bool> boolList = new List<int>(boolArray.ToArray()).ConvertAll(x => Convert.ToBoolean(x));
                    param.AddVolatileDataList(new GH_Path(0), boolList);
                }

                boolArray.Dispose();
            }
            else
            {
                var toggle = new GH_BooleanToggle();
                toggle.CreateAttributes();
                toggle.Attributes.Pivot = new PointF(Attributes.Bounds.Location.X - toggle.Attributes.Bounds.Width - 20, 
                                                                    Attributes.Pivot.Y - toggle.Attributes.Bounds.Height/2);
                bool val = false;
                if(PRTWrapper.GetDefaultValueBoolean(Name, ref val))
                {
                    toggle.Value = val;
                }

                param = toggle;
            }

            param.Description = Description;
            param.Name = Name;
            param.NickName = NickName;

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if (mGroupName.Length > 0)
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);

            ExpireSolution(true);
        }
    }

    public class Number: Param_Number
    {
        private readonly List<Annotations.Base> mAnnotations;
        private readonly string mGroupName;
        private readonly bool mExpectsArray;

        public Number(List<Annotations.Base> annots, string groupName, bool expectsArray)
        {
            mAnnotations = annots;
            mGroupName = groupName;
            mExpectsArray = expectsArray;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            var annot = mAnnotations.Find(x => x.GetAnnotationType() == Annotations.AttributeAnnotation.A_RANGE ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM);

            if(mExpectsArray)
            {
                param = new Param_Number();

                SimpleArrayDouble defaultValues = new SimpleArrayDouble();
                var pDefaultValues = defaultValues.NonConstPointer();
                if(PRTWrapper.GetDefaultValueNumberArray(Name, pDefaultValues))
                {
                    param.AddVolatileDataList(new GH_Path(0), defaultValues.ToArray());
                    param.CollectData();
                }

                defaultValues.Dispose();
            } 
            else if(annot != null)
            {
                param = annot.GetGhSpecializedParam();

                double value = 0;
                if (PRTWrapper.GetDefaultValueNumber(Name, ref value))
                {
                    if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
                    {
                        int itemIndex = ((GH_ValueList)param).ListItems.FindIndex(x => x.Name == value.ToString());
                        ((GH_ValueList)param).SelectItem(itemIndex);
                    }
                    else if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_RANGE)
                    {
                        ((GH_NumberSlider)param).SetSliderValue((decimal)value);
                    }
                }
            } 
            else
            {
                param = new Param_Number();

                double value = 0;
                if (PRTWrapper.GetDefaultValueNumber(Name, ref value))
                {
                    var nb_val = new GH_Number(value);
                    param.AddVolatileData(new GH_Path(), 0, nb_val);
                }
            }

            param.CreateAttributes();
            param.Attributes.Pivot = new PointF(Attributes.Bounds.Location.X - param.Attributes.Bounds.Width - 20,
                                                                Attributes.Pivot.Y - param.Attributes.Bounds.Height / 2);
            param.Description = Description;
            param.Name = Name;
            param.NickName = NickName;

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if (mGroupName.Length > 0)
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);

            ExpireSolution(true);
        }
    }

    public class String: Param_String
    {
        private readonly List<Annotations.Base> mAnnotations;
        private readonly string mGroupName;
        private readonly bool mExpectsArray;

        public String(List<Annotations.Base> annots, string groupName = "", bool expectsArray = false): base()
        {
            mAnnotations = annots;
            mGroupName = groupName;
            mExpectsArray = expectsArray;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            // annots: file, dir, enum, enum.
            var annot = mAnnotations.Find(x => x.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_DIR ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_FILE);
            
            if(mExpectsArray)
            {
                param = new Param_String();

                ClassArrayString defaultStrings = new ClassArrayString();
                var pDefaultStrings = defaultStrings.NonConstPointer();
                if (PRTWrapper.GetDefaultValueTextArray(Name, pDefaultStrings))
                {
                    if(annot == null || annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
                        param.AddVolatileDataList(new GH_Path(0), defaultStrings.ToArray());

                    // For files and documents, keeping data empty will force PRT to use the default values.
                }
                defaultStrings.Dispose();
            }
            else
            {
                // find default string value
                StringWrapper value = new StringWrapper();
                var pValue = value.NonConstPointer;
                string defaultText = string.Empty;
                bool hasDefault = PRTWrapper.GetDefaultValueText(Name, pValue);
                if(hasDefault) defaultText = value.ToString();

                if (annot != null)
                {
                    param = annot.GetGhSpecializedParam();
                    if (hasDefault)
                    {
                        if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
                        {
                            int itemIndex = ((GH_ValueList)param).ListItems.FindIndex(x => x.Name == defaultText);
                            ((GH_ValueList)param).SelectItem(itemIndex);
                        }
                    }

                    // For files and documents, keeping data empty will force PRT to use the default values.
                }
                else
                {
                    param = new Param_String();
                    if (hasDefault)
                        param.AddVolatileData(new GH_Path(0), 0, defaultText);
                }
            }

            param.CreateAttributes();
            param.Attributes.Pivot = new PointF(Attributes.Bounds.Location.X - param.Attributes.Bounds.Width - 20,
                                                                Attributes.Pivot.Y - param.Attributes.Bounds.Height / 2);
            param.Description = Description;
            param.Name = Name;
            param.NickName = NickName;

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if (mGroupName.Length > 0)
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);

            ExpireSolution(true);
        }
    }

    public class Colour: Param_Colour
    {
        private readonly string mGroupName;

        public Colour(string groupName= ""): base()
        {
            mGroupName = groupName;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param;
         
            StringWrapper value = new StringWrapper();
            var pValue = value.NonConstPointer;
            Color defaultColor = new Color();
            bool hasDefault = PRTWrapper.GetDefaultValueText(Name, pValue);
            if (hasDefault) defaultColor = Utils.FromHex(value.ToString());

            param = new GH_ColourPickerObject
            {
                Colour = defaultColor
            };

            param.CreateAttributes();
            param.Attributes.Pivot = new PointF(Attributes.Bounds.Location.X - param.Attributes.Bounds.Width - 20,
                                                                Attributes.Pivot.Y - param.Attributes.Bounds.Height / 2);
            param.Description = Description;
            param.Name = Name;
            param.NickName = NickName;

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if(mGroupName.Length > 0)
            {
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);
            }

            ExpireSolution(true);
        }
    }
}
