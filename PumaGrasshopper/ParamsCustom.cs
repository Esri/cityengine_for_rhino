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

using GH_IO.Serialization;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Special;
using Grasshopper.Kernel.Types;
using Rhino.Runtime.InteropWrappers;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace PumaGrasshopper
{
    public class BooleanRuleAttribute : Param_Boolean
    {
        private List<Annotations.Base> mAnnotation;
        private string mGroupName;

        public BooleanRuleAttribute(List<Annotations.Base> annot, string groupName = ""): base()
        {
            mAnnotation = annot;
            mGroupName = groupName;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            var toggle = new GH_BooleanToggle();
            toggle.CreateAttributes();
            toggle.Attributes.Pivot = new System.Drawing.PointF(Attributes.Bounds.Location.X - toggle.Attributes.Bounds.Width - 20, 
                                                                Attributes.Pivot.Y - toggle.Attributes.Bounds.Height/2);
            toggle.Description = Description;

            IGH_Param param;

            //Get the default value from the Rhino side.
            if (Access == GH_ParamAccess.item)
            {
                bool val = false;
                if (PRTWrapper.GetDefaultValueBoolean(Name, ref val))
                    toggle.Value = val;

                param = toggle;
            }
            else
            {
                param = new Param_Boolean();
            }

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if (mGroupName.Length > 0)
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);

            ExpireSolution(true);
        }
    }

    public class NumberRuleAttribute: Param_Number
    {
        private List<Annotations.Base> mAnnotations;
        private string mGroupName;

        public NumberRuleAttribute(List<Annotations.Base> annots, string groupName)
        {
            mAnnotations = annots;
            mGroupName = groupName;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            double value = 0;
            bool hasDefault = PRTWrapper.GetDefaultValueNumber(Name, ref value);

            IGH_Param param = null;

            var annot = mAnnotations.Find(x => x.GetAnnotationType() == Annotations.AttributeAnnotation.A_RANGE ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM);

            if (annot != null)
            {
                param = annot.GetGhSpecializedParam();

                if (hasDefault)
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
                
                if (hasDefault)
                {
                    GH_Number nb_val = new GH_Number();
                    nb_val.Value = value;
                    param.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(0), 0, nb_val);
                }
            }

            param.CreateAttributes();
            param.Attributes.Pivot = new System.Drawing.PointF(Attributes.Bounds.Location.X - param.Attributes.Bounds.Width - 20,
                                                                Attributes.Pivot.Y - param.Attributes.Bounds.Height / 2);
            param.Description = Description;

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if (mGroupName.Length > 0)
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);

            ExpireSolution(true);
        }
    }

    public class StringRuleAttribute: Param_String
    {
        private List<Annotations.Base> mAnnotations;
        private string mGroupName;

        public StringRuleAttribute(List<Annotations.Base> annots, string groupName = ""): base()
        {
            mAnnotations = annots;
            mGroupName = groupName;
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
            
            // find default string value
            StringWrapper value = new StringWrapper();
            var pValue = value.NonConstPointer;
            string defaultText = string.Empty;
            bool hasDefault = PRTWrapper.GetDefaultValueText(Name, pValue);
            if(hasDefault) defaultText = pValue.ToString();

            if (annot != null)
            {
                param = annot.GetGhSpecializedParam();
                if (hasDefault)
                {
                    if(annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
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
                if(hasDefault)
                    param.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(),0, defaultText);
            }

            param.CreateAttributes();
            param.Attributes.Pivot = new System.Drawing.PointF(Attributes.Bounds.Location.X - param.Attributes.Bounds.Width - 20,
                                                                Attributes.Pivot.Y - param.Attributes.Bounds.Height / 2);
            param.Description = Description;

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if (mGroupName.Length > 0)
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);

            ExpireSolution(true);
        }
    }

    public class ColourRuleAttribute: Param_Colour
    {
        private List<Annotations.Base> mAnnotations;
        private string mGroupName;

        public ColourRuleAttribute(List<Annotations.Base> annots, string groupName= ""): base()
        {
            mAnnotations = annots;
            mGroupName = groupName;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            GH_ColourPickerObject param = new GH_ColourPickerObject();

            param.CreateAttributes();
            param.Attributes.Pivot = new System.Drawing.PointF(Attributes.Bounds.Location.X - param.Attributes.Bounds.Width - 20,
                                                                Attributes.Pivot.Y - param.Attributes.Bounds.Height / 2);
            param.Description = Description;

            // find default string value
            StringWrapper value = new StringWrapper();
            var pValue = value.NonConstPointer;

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
