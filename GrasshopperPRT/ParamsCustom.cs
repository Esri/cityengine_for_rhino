using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Special;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace GrasshopperPRT
{
    public class ParamBoolCustom : Param_Boolean
    {
        private List<Annotation> mAnnotation;
        private string mGroupName;

        public ParamBoolCustom(List<Annotation> annot, string groupName = ""): base()
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
            toggle.Value = false;
            toggle.Description = Description;

            var doc = OnPingDocument();
            doc.AddObject(toggle, false);

            AddSource(toggle);

            if (mGroupName.Length > 0)
                Utils.AddToGroup(doc, mGroupName, toggle.InstanceGuid);

            ExpireSolution(true);
        }
    }

    public class ParamNumberCustom: Param_Number
    {
        private List<Annotation> mAnnotations;
        private string mGroupName;

        public ParamNumberCustom(List<Annotation> annots, string groupName = ""): base()
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

            var annot = mAnnotations.Find(x => x.GetAnnotationType() == AttributeAnnotation.A_RANGE ||
                                               x.GetAnnotationType() == AttributeAnnotation.A_ENUM);
            if (annot != null)
                param = annot.GetGhSpecializedParam();
            else
                param = new Param_Number();

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

    public class ParamStringCustom: Param_String
    {
        private List<Annotation> mAnnotations;
        private string mGroupName;

        public ParamStringCustom(List<Annotation> annots, string groupName = ""): base()
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
            var annot = mAnnotations.Find(x => x.GetAnnotationType() == AttributeAnnotation.A_ENUM ||
                                               x.GetAnnotationType() == AttributeAnnotation.A_DIR ||
                                               x.GetAnnotationType() == AttributeAnnotation.A_FILE);
            if (annot != null)
                param = annot.GetGhSpecializedParam();
            else
                param = new Param_String();

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

    public class ParamColourCustom: Param_Colour
    {
        private List<Annotation> mAnnotations;
        private string mGroupName;

        public ParamColourCustom(List<Annotation> annots, string groupName= ""): base()
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
