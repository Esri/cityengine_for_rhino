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
using Grasshopper.Kernel.Attributes;
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
    public class PumaParameter<T> : GH_PersistentParam<T> where T : class, IGH_Goo
    {
        protected string mGroupName;
        protected bool mExpectsArray;
        protected List<Annotations.Base> mAnnotations;

        public PumaParameter() : base("PumaParam", "PumaParam", "Default puma parameter", ComponentLibraryInfo.MainCategory, ComponentLibraryInfo.PumaSubCategory)
        {
            mGroupName = "";
            mExpectsArray = false;
            mAnnotations = new List<Annotations.Base>();
        }

        public PumaParameter(string groupName, bool expectsArray, List<Annotations.Base> annotations) : base("PumaParam", "PumaParam", "Default puma parameter", ComponentLibraryInfo.MainCategory, ComponentLibraryInfo.PumaSubCategory)
        {
            mGroupName = groupName;
            mExpectsArray = expectsArray;
            mAnnotations = annotations;
        }

        public override Guid ComponentGuid => throw new NotImplementedException();

        public override GH_Exposure Exposure { get { return GH_Exposure.hidden; } }

        protected override void Menu_AppendPromptOne(ToolStripDropDown menu) { }

        protected override void Menu_AppendPromptMore(ToolStripDropDown menu) { }

        protected override GH_GetterResult Prompt_Plural(ref List<T> values)
        {
            return GH_GetterResult.cancel;
        }

        protected override GH_GetterResult Prompt_Singular(ref T value)
        {
            return GH_GetterResult.cancel;
        }

        public override bool Write(GH_IWriter writer)
        {
            writer.SetInt32(SerializationIds.VERSION, SerializationIds.SERIALIZATION_VERSION);
            writer.SetString(SerializationIds.GROUP_NAME, mGroupName);
            writer.SetBoolean(SerializationIds.EXPECTS_ARRAY, mExpectsArray);
            AnnotationSerialization.WriteAnnotations(writer, mAnnotations);
            
            return base.Write(writer);
        }

        public override bool Read(GH_IReader reader)
        {
            int serializationVersion = 0;
            if (reader.TryGetInt32(SerializationIds.VERSION, ref serializationVersion)
                && serializationVersion == SerializationIds.SERIALIZATION_VERSION)
            {
                mGroupName = reader.GetString(SerializationIds.GROUP_NAME);
                mExpectsArray = reader.GetBoolean(SerializationIds.EXPECTS_ARRAY);
                mAnnotations = AnnotationSerialization.ReadAnnotations(reader);
            }

            return base.Read(reader);
        }

        protected void DisplayExtractedParam(IGH_Param param)
        {
            param.CreateAttributes();
            param.Attributes.Pivot = new PointF(Attributes.Bounds.Location.X - param.Attributes.Bounds.Width - 20,
                                                                Attributes.Pivot.Y - param.Attributes.Bounds.Height / 2);
            param.Description = Description;
            param.Name = Name;
            param.NickName = NickName;

            var doc = OnPingDocument();
            doc.AddObject(param, false);

            AddSource(param);

            if (Attributes.IsTopLevel)
                throw new Exception("Puma parameters can only be used within a Puma component");

            if (mGroupName.Length > 0)
            {
                // Get the guid of the parent object (i.e. a Puma component)
                GH_ComponentAttributes parent = (GH_ComponentAttributes)Attributes.GetTopLevel;
                Utils.AddToGroup(doc, parent.InstanceGuid, mGroupName, param.InstanceGuid);
            }

            ExpireSolution(true);
        }
    } 

    public class Boolean : PumaParameter<GH_Boolean>
    {
        public Boolean() : base() { }

        public Boolean(string groupName = "", bool expectsArray = false): base(groupName, expectsArray, new List<Annotations.Base>()) { }

        public override Guid ComponentGuid {
            get { return PumaUIDs.AttributeParameterBooleanGuid; }
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private GH_Structure<GH_Boolean> GetData()
        {
            if (PersistentDataCount > 0) return PersistentData;

            if (mExpectsArray)
            {
                //TODO refactor -> get value from local attribute list
                List<List<bool>> defaultValues = PRTWrapper.GetDefaultValuesBooleanArray(Name);
                if (defaultValues != null)
                {
                    return Utils.FromListToTree(defaultValues);
                }
            }
            else
            {
                List<bool> defaultValues = PRTWrapper.GetDefaultValuesBoolean(Name);
                if (defaultValues != null)
                {
                    return Utils.FromListToTree(defaultValues);
                }
            }

            return new GH_Structure<GH_Boolean>();
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            GH_Structure<GH_Boolean> data = GetData();
            
            if(!mExpectsArray && data.DataCount == 1)
            {
                var toggle = new GH_BooleanToggle()
                {
                    Value = data.get_FirstItem(true).Value
                };
                    
                param = toggle;
            }

            if(param == null)
            {
                var paramPersistent = new Param_Boolean();
                paramPersistent.SetPersistentData(data);
                param = paramPersistent;
            }

            DisplayExtractedParam(param);
        }
    }

    public class Number: PumaParameter<GH_Number>
    {
        public Number() : base() { }

        public Number(List<Annotations.Base> annots, string groupName, bool expectsArray) : base(groupName, expectsArray, annots) { }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.AttributeParameterNumberGuid; }
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private GH_Structure<GH_Number> GetData()
        {
            if (PersistentDataCount > 0) return PersistentData;

            if (mExpectsArray)
            {
                //TODO refactor -> get value from local attribute list
                List<List<double>> defaultValues = PRTWrapper.GetDefaultValuesNumberArray(Name);
                if (defaultValues != null)
                {
                    return Utils.FromListToTree(defaultValues);
                }
            }
            else
            {
                //TODO refactor -> get value from local attribute list
                List<double> defaultValues = PRTWrapper.GetDefaultValuesNumber(Name);
                if(defaultValues != null)
                {
                    return Utils.FromListToTree(defaultValues);
                }
            }

            return new GH_Structure<GH_Number>();
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            GH_Structure<GH_Number> data = GetData();
            
            var annot = mAnnotations.Find(x => x.GetAnnotationType() == Annotations.AttributeAnnotation.A_RANGE ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM);

            if (!mExpectsArray && data.DataCount == 1 && annot != null)
            {
                param = annot.GetGhSpecializedParam();

                if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
                {
                    int itemIndex = ((GH_ValueList)param).ListItems.FindIndex(x => x.Name == data.get_FirstItem(true).ToString());
                    ((GH_ValueList)param).SelectItem(itemIndex);
                }
                else if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_RANGE)
                {
                    ((GH_NumberSlider)param).SetSliderValue((decimal)data.get_FirstItem(true).Value);
                }
            }

            if (param == null)
            {
                var paramPersistent = new Param_Number();
                paramPersistent.SetPersistentData(data);
                param = paramPersistent;
            }

            DisplayExtractedParam(param);
        }
    }

    public class String: PumaParameter<GH_String>
    {
        public String() : base() { }

        public String(List<Annotations.Base> annots, string groupName = "", bool expectsArray = false) : base(groupName, expectsArray, annots) { }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.AttributeParameterStringGuid; }
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private GH_Structure<GH_String> GetData()
        {
            if (PersistentDataCount > 0) return PersistentData;

            if (mExpectsArray)
            {
                //TODO refactor -> get value from local attribute list
                List<List<string>> defaultValues = PRTWrapper.GetDefaultValuesTextArray(Name);
                if (defaultValues != null)
                {
                    return Utils.FromListToTree(defaultValues);
                }
            }
            else
            {
                //TODO refactor -> get value from local attribute list
                List<string> defaultValues = PRTWrapper.GetDefaultValuesText(Name);
                if (defaultValues != null)
                {
                    return Utils.FromListToTree(defaultValues);
                }
            }

            return new GH_Structure<GH_String>();
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            GH_Structure<GH_String> data = GetData();

            // annots: file, dir, enum.
            var annot = mAnnotations.Find(x => x.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_DIR ||
                                               x.GetAnnotationType() == Annotations.AttributeAnnotation.A_FILE);
            
            if(mExpectsArray)
            {
                var paramPersistent = new Param_String();
                
                if((annot == null || annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM))
                {
                    paramPersistent.SetPersistentData(data);
                    // For files and documents, keeping data empty will force PRT to use the default values.
                }
 
                param = paramPersistent;
            }
            else if (data.DataCount == 1 && annot != null)
            {
                param = annot.GetGhSpecializedParam();
                if (annot.GetAnnotationType() == Annotations.AttributeAnnotation.A_ENUM)
                {
                    int itemIndex = ((GH_ValueList)param).ListItems.FindIndex(x => x.Name == data.get_FirstItem(true).Value);
                    ((GH_ValueList)param).SelectItem(itemIndex);
                }
            }
            
            if(param == null)
            {
                var paramPersistent = new Param_String();
                paramPersistent.SetPersistentData(data);
                param = paramPersistent;
            }

            DisplayExtractedParam(param);
        }
    }

    public class Colour: PumaParameter<GH_Colour>
    {
        public Colour() : base() { }

        public Colour(string groupName = "") : base(groupName, false, new List<Annotations.Base>()) { }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.AttributeParameterColourGuid; }
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private GH_Structure<GH_Colour> GetData()
        {
            if (PersistentDataCount > 0) return PersistentData;

            //TODO refactor -> get value from local attribute list
            List<string> defaultValues = PRTWrapper.GetDefaultValuesText(Name);
            if (defaultValues != null)
            {
                return Utils.HexListToColorTree(defaultValues);
            }
            
            return new GH_Structure<GH_Colour>();
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            IGH_Param param = null;

            GH_Structure<GH_Colour> data = GetData();

            if(data.DataCount == 1)
            {
                param = new GH_ColourPickerObject
                {
                    Colour = data.get_FirstItem(true).Value
                };
            }
            else
            {
                var paramPersistent = new Param_Colour();
                paramPersistent.SetPersistentData(data);
                param = paramPersistent;
            }

            DisplayExtractedParam(param);
        }
    }
}
