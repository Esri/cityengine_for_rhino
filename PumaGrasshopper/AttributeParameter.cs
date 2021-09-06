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
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Special;
using Grasshopper.Kernel.Types;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace PumaGrasshopper.AttributeParameter
{
    static class SerializationIds {
        public const string GROUP_NAME = "GROUP_NAME";
        public const string EXPECTS_ARRAY = "EXPECTS_ARRAY";
        public const string ANNOTATION_COUNT = "ANNOTATION_COUNT";
        public const string ANNOTATION_TYPE = "ANNOTATION_TYPE";
        public const string ANNOTATION_ENUM_TYPE = "ANNOTATION_ENUM_TYPE";
        public const string ANNOTATION = "ANNOTATION";
    }

    public class Boolean : Param_Boolean
    {
        private string mGroupName;
        private bool mExpectsArray;

        public Boolean() : base() { }

        public Boolean(string groupName = "", bool expectsArray = false): base()
        {
            mGroupName = groupName;
            mExpectsArray = expectsArray;
        }

        public override Guid ComponentGuid {
            get { return PumaUIDs.AttributeParameterBooleanGuid; }
        }

        public override GH_Exposure Exposure { get { return GH_Exposure.hidden; } }

        public override bool Write(GH_IWriter writer)
        {
            writer.SetString(SerializationIds.GROUP_NAME, mGroupName);
            writer.SetBoolean(SerializationIds.EXPECTS_ARRAY, mExpectsArray);
            return base.Write(writer);
        }
        public override bool Read(GH_IReader reader)
        {
            mGroupName = reader.GetString(SerializationIds.GROUP_NAME);
            mExpectsArray = reader.GetBoolean(SerializationIds.EXPECTS_ARRAY);
            return base.Read(reader);
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
                var defaultValues = PRTWrapper.GetDefaultValuesBooleanArray(Name);

                var paramPersistent = new Param_Boolean();

                if(defaultValues != null)
                {
                    GH_Structure<GH_Boolean> tree = Utils.FromListToTree(defaultValues);
                    paramPersistent.SetPersistentData(tree);
                }

                param = paramPersistent;
            }
            else
            {
                List<bool> defaultValues = PRTWrapper.GetDefaultValuesBoolean(Name);

                if (defaultValues == null)
                {
                    param = new Param_Boolean();
                }
                else if (defaultValues.Count == 1)
                {
                    var toggle = new GH_BooleanToggle()
                    {
                        Value = defaultValues[0]
                    };
                    
                    param = toggle;
                }
                else
                {
                    GH_Structure<GH_Boolean> tree = new GH_Structure<GH_Boolean>();

                    for (int i = 0; i < defaultValues.Count; ++i)
                    {
                        tree.Insert(new GH_Boolean(defaultValues[i]), new GH_Path(i), 0);
                    }

                    var paramPersistent = new Param_Boolean();
                    paramPersistent.SetPersistentData(tree);
                    param = paramPersistent;
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

    public class Number: Param_Number
    {
        private List<Annotations.Base> mAnnotations;
        private string mGroupName;
        private bool mExpectsArray;

        public Number() : base() {
            mAnnotations = new List<Annotations.Base>();
        }

        public Number(List<Annotations.Base> annots, string groupName, bool expectsArray) : base()
        {
            mAnnotations = annots;
            mGroupName = groupName;
            mExpectsArray = expectsArray;
        }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.AttributeParameterNumberGuid; }
        }

        public override GH_Exposure Exposure { get { return GH_Exposure.hidden; } }

        public override bool Write(GH_IWriter writer)
        {
            writer.SetString(SerializationIds.GROUP_NAME, mGroupName);
            writer.SetBoolean(SerializationIds.EXPECTS_ARRAY, mExpectsArray);
            AnnotationSerialization.WriteAnnotations(writer, mAnnotations);

            return base.Write(writer);
        }

        public override bool Read(GH_IReader reader)
        {
            mGroupName = reader.GetString(SerializationIds.GROUP_NAME);
            mExpectsArray = reader.GetBoolean(SerializationIds.EXPECTS_ARRAY);
            mAnnotations = AnnotationSerialization.ReadAnnotations(reader);

            return base.Read(reader);
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            if(mExpectsArray)
            {
                var defaultValues = PRTWrapper.GetDefaultValuesNumberArray(Name);
                
                var paramPersistent = new Param_Number();

                if(defaultValues != null)
                {
                    GH_Structure<GH_Number> tree = Utils.FromListToTree(defaultValues);
                    paramPersistent.SetPersistentData(tree);
                }

                param = paramPersistent;
            } 
            else
            {
                List<double> defaultValues = PRTWrapper.GetDefaultValuesNumber(Name);

                var annot = mAnnotations.Find(x => x.GetAnnotationType() == Annotations.AttributeAnnotation.A_RANGE ||
                                                       x.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM);

                if (defaultValues == null)
                {
                    param = new Param_Number();
                }
                else if(defaultValues.Count == 1 && annot != null)
                {
                    param = annot.GetGhSpecializedParam();

                    if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
                    {
                        int itemIndex = ((GH_ValueList)param).ListItems.FindIndex(x => x.Name == defaultValues[0].ToString());
                        ((GH_ValueList)param).SelectItem(itemIndex);
                    }
                    else if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_RANGE)
                    {
                        ((GH_NumberSlider)param).SetSliderValue((decimal)defaultValues[0]);
                    }
                }
                else
                {
                    GH_Structure<GH_Number> tree = new GH_Structure<GH_Number>();

                    for (int i = 0; i < defaultValues.Count; ++i)
                    {
                        tree.Insert(new GH_Number(defaultValues[i]), new GH_Path(i), 0);
                    }

                    var paramPersistent = new Param_Number();
                    paramPersistent.SetPersistentData(tree);
                    param = paramPersistent;
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
        private List<Annotations.Base> mAnnotations;
        private string mGroupName;
        private bool mExpectsArray;

        public String() : base() {
            mAnnotations = new List<Annotations.Base>();
        }

        public String(List<Annotations.Base> annots, string groupName = "", bool expectsArray = false) : base()
        {
            mAnnotations = annots;
            mGroupName = groupName;
            mExpectsArray = expectsArray;
        }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.AttributeParameterStringGuid; }
        }
        
        public override GH_Exposure Exposure { get { return GH_Exposure.hidden; } }

        public override bool Write(GH_IWriter writer)
        {
            writer.SetString(SerializationIds.GROUP_NAME, mGroupName);
            writer.SetBoolean(SerializationIds.EXPECTS_ARRAY, mExpectsArray);
            AnnotationSerialization.WriteAnnotations(writer, mAnnotations);

            return base.Write(writer);
        }

        public override bool Read(GH_IReader reader)
        {
            mExpectsArray = reader.GetBoolean(SerializationIds.EXPECTS_ARRAY);
            mGroupName = reader.GetString(SerializationIds.GROUP_NAME);
            mAnnotations = AnnotationSerialization.ReadAnnotations(reader);

            return base.Read(reader);
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            // annots: file, dir, enum.
            var annot = mAnnotations.Find(x => x.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_DIR ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_FILE);
            
            if(mExpectsArray)
            {
                var defaultValues = PRTWrapper.GetDefaultValuesTextArray(Name);

                var paramPersistent = new Param_String();
                
                if(defaultValues != null && (annot == null || annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM))
                {
                    var tree = Utils.FromListToTree(defaultValues);
                    paramPersistent.SetPersistentData(tree);
                    // For files and documents, keeping data empty will force PRT to use the default values.
                }
 
                param = paramPersistent;
            }
            else
            {
                List<string> defaultValues = PRTWrapper.GetDefaultValuesText(Name);

                if (defaultValues == null)
                {
                    param = new Param_String();
                }
                else if(defaultValues.Count == 1 && annot != null)
                {
                    param = annot.GetGhSpecializedParam();
                    if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
                    {
                        int itemIndex = ((GH_ValueList)param).ListItems.FindIndex(x => x.Name == defaultValues[0]);
                        ((GH_ValueList)param).SelectItem(itemIndex);
                    }
                }
                else
                {
                    GH_Structure<GH_String> tree = new GH_Structure<GH_String>();

                    for (int i = 0; i < defaultValues.Count; ++i)
                    {
                        tree.Insert(new GH_String(defaultValues[i]), new GH_Path(i), 0);
                    }

                    var paramPersistent = new Param_String();
                    paramPersistent.SetPersistentData(tree);
                    param = paramPersistent;
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
        private string mGroupName;

        public Colour() : base() { }

        public Colour(string groupName= ""): base()
        {
            mGroupName = groupName;
        }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.AttributeParameterColourGuid; }
        }

        public override GH_Exposure Exposure { get { return GH_Exposure.hidden; } }

        public override bool Write(GH_IWriter writer)
        {
            writer.SetString(SerializationIds.GROUP_NAME, mGroupName);
            return base.Write(writer);
        }

        public override bool Read(GH_IReader reader)
        {
            mGroupName = reader.GetString(SerializationIds.GROUP_NAME);
            return base.Read(reader);
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {   
            List<string> defaultValues = PRTWrapper.GetDefaultValuesText(Name);

            IGH_Param param = null;

            if (defaultValues == null)
            {
                param = new Param_Colour();
            }
            else if(defaultValues.Count == 1)
            {
                param = new GH_ColourPickerObject
                {
                    Colour = Utils.FromHex(defaultValues[0])
                };
            }
            else
            {
                List<Color> defaultColor = defaultValues.ConvertAll(t => Utils.FromHex(t));

                GH_Structure<GH_Colour> tree = new GH_Structure<GH_Colour>();

                for (int i = 0; i < defaultColor.Count; ++i)
                {
                    tree.Insert(new GH_Colour(defaultColor[i]), new GH_Path(i), 0);
                }

                var paramPersistent = new Param_Colour();
                paramPersistent.SetPersistentData(tree);
                param = paramPersistent;
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

            if(mGroupName.Length > 0)
            {
                Utils.AddToGroup(doc, mGroupName, param.InstanceGuid);
            }

            ExpireSolution(true);
        }
    }
}
