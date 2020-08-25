using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;

namespace GrasshopperPRT
{
    public class GrasshopperPRTReports : GH_Component
    {
        const string REPORTS_INPUT_NAME = "Reports";
        const string FILTER_SHAPE_ID = "Shape ID Filter";
        const string FILTER_REPORT_KEY = "Report Key Filter";
        const string FILTER_REPORT_VALUE = "Report Value Filter";

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
            pManager.AddGenericParameter(REPORTS_INPUT_NAME, REPORTS_INPUT_NAME, "The CGA Reports", GH_ParamAccess.tree);

            // The 3 filter options: GH_Interval on initial shape. String or list of String to filter keys.
            pManager.AddIntervalParameter(FILTER_SHAPE_ID, FILTER_SHAPE_ID, "An interval on shape IDs to select.", GH_ParamAccess.item, Interval.Unset);
            pManager.AddTextParameter(FILTER_REPORT_KEY, FILTER_REPORT_KEY, "The report keys to select.", GH_ParamAccess.list, string.Empty);
            pManager.AddTextParameter(FILTER_REPORT_VALUE, FILTER_REPORT_VALUE, "The report values to select. " +
                "Each branch of the tree must represent the chosen values for the corresponding key given in the key filter.",
                GH_ParamAccess.tree, string.Empty);
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
            var reportTree = new GH_Structure<IGH_Goo>();
            if(!DA.GetDataTree<IGH_Goo>(0, out reportTree))
            {
                return;
            }

            List<int> initialShapeIds = new List<int>();
            List<string> reportKeys = new List<string>();
            List<List<string>> valueKeys = new List<List<string>>();
            GetFilters(DA, ref initialShapeIds, ref reportKeys, ref valueKeys);
            
            var reports = new List<Dictionary<string, ReportAttribute>>();

            // extract the report tree to have a list of attribute by initial shape.
            foreach(var attr in reportTree.AllData(true))
            {
                var report = (ReportAttribute)attr;

                while(reports.Count <= report.ShapeID)
                {
                    reports.Add(new Dictionary<string, ReportAttribute>());
                }

                reports[report.ShapeID].Add(report.getKey(), report);
            }
        }

        private bool GetFilters(IGH_DataAccess DA, ref List<int> initialShapeIds, ref List<string> reportKeys, ref List<List<string>> valueKeys)
        {
            GH_Interval id_interval = new GH_Interval();
            if(!DA.GetData(FILTER_SHAPE_ID, ref id_interval))
            {
                return false;
            }

            if(!DA.GetDataList(FILTER_REPORT_KEY, reportKeys))
            {
                return false;
            }

            GH_Structure<GH_String> valuesTree = new GH_Structure<GH_String>();
            if(!DA.GetDataTree(FILTER_REPORT_VALUE, out valuesTree))
            {
                return false;
            }

            return true;
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