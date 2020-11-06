using Grasshopper.Kernel.Parameters;
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace GrasshopperPRT
{
    public class ParamBoolCustom : Param_Boolean
    {
        private List<Annotation> mAnnotation;

        public ParamBoolCustom(List<Annotation> annot): base()
        {
            mAnnotation = annot;
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Extract parameter", OnExtractParamClicked);
        }

        private void OnExtractParamClicked(object sender, EventArgs e)
        {
            var toggle = new Grasshopper.Kernel.Special.GH_BooleanToggle();
            toggle.CreateAttributes();
            toggle.Attributes.Pivot = new System.Drawing.PointF(Attributes.Bounds.Location.X - toggle.Attributes.Bounds.Width - 20, 
                                                                Attributes.Pivot.Y - toggle.Attributes.Bounds.Height/2);
            toggle.Value = false;
            toggle.Description = Description;

            var doc = OnPingDocument();
            doc.AddObject(toggle, false);

            AddSource(toggle);

            ExpireSolution(true);
        }
    }
}
