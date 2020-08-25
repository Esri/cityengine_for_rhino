using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace GrasshopperPRT
{
    public class GrasshopperPRTReports : GH_Component
    {
        const string REPORTS_INPUT_NAME = "Reports";

        /// <summary>
        /// Initializes a new instance of the MyComponent1 class.
        /// </summary>
        public GrasshopperPRTReports()
          : base("GrasshopperPRTReports", "CGA Reports display",
              "Helper component unpacking and displaying the Reports output from GrasshopperPRTComponent.",
              "Special", "Esri")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddGenericParameter(REPORTS_INPUT_NAME, REPORTS_INPUT_NAME, "", GH_ParamAccess.tree);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
        }

        /// <summary>
        /// Provides an Icon for the component.
        /// </summary>
        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                //You can add image files to your project resources and access them like this:
                // return Resources.IconForThisComponent;
                return null;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("316524f4-1c56-41e7-9315-10f60b35cd61"); }
        }
    }
}