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
        const string MESH_INPUT = "Meshes";
        const string REPORTS_INPUT_NAME = "Reports";
        const string FILTER_SHAPE_ID = "Shape ID Filter";
        const string FILTER_REPORT_KEY = "Report Key Filter";
        const string FILTER_REPORT_VALUE = "Report Value Filter";
        const string REPORTS_DISPLAY_PARAM = "Reports Display";
        const string REPORTS_LOCATION = "Reports Location";
        const string REPORTS_FILTERED = "Reports";

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
            pManager.AddMeshParameter(MESH_INPUT, MESH_INPUT, "Mesh input is used to compute where to put the reports display.", GH_ParamAccess.list);
            pManager.AddGenericParameter(REPORTS_INPUT_NAME, REPORTS_INPUT_NAME, "The CGA Reports", GH_ParamAccess.tree);

            // The 3 filter options: list of GH_Interval on initial shape. String or list of String to filter keys.
            pManager.AddIntervalParameter(FILTER_SHAPE_ID, FILTER_SHAPE_ID, "An interval on shape IDs to select.", GH_ParamAccess.list, Interval.Unset);
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
            //There are 2 outputs: First one is the report preview location.
            //                     Second one is the report preview, to display the report data in rhino.
            pManager.AddPlaneParameter(REPORTS_LOCATION, REPORTS_LOCATION, "Point location indicating the text preview position.", GH_ParamAccess.list);
            pManager.AddTextParameter(REPORTS_DISPLAY_PARAM, REPORTS_DISPLAY_PARAM, 
                "Formated text reports that can be displayed in Rhino using a text preview component.",
                GH_ParamAccess.list);
            pManager.AddGenericParameter(REPORTS_FILTERED, REPORTS_FILTERED, "Filtered reports outputed for further processing.", GH_ParamAccess.tree);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            var meshList = new List<GH_Mesh>();
            bool locateFromMeshes = DA.GetDataList(0, meshList);

            var reportTree = new GH_Structure<IGH_Goo>();
            if(!DA.GetDataTree<IGH_Goo>(1, out reportTree))
            {
                return;
            }
            var reports = GetReports(ref reportTree);

            List<int> initialShapeIds = new List<int>();
            List<string> reportKeys = new List<string>();
            List<List<string>> valueKeys = new List<List<string>>();
            bool applyFilters = GetFilters(DA, ref initialShapeIds, ref reportKeys, ref valueKeys);

            var filteredReports = FilterReports(applyFilters, reports, initialShapeIds, reportKeys, valueKeys);

            // Format the text output
            var previewReports = GetFormatedReports(filteredReports, meshList.Count);

            if (locateFromMeshes)
            {
                var pointList = ComputeReportPositions(meshList, previewReports.Item2);

                DA.SetDataList(REPORTS_LOCATION, pointList);
            }

            DA.SetDataList(REPORTS_DISPLAY_PARAM, previewReports.Item1);

            GH_Structure<ReportAttribute> filteredReportAttributes = new GH_Structure<ReportAttribute>();
            foreach(var currShapeReports in filteredReports)
            {
                GH_Path path = new GH_Path(currShapeReports.Key);

                foreach(var report in currShapeReports.Value)
                {
                    filteredReportAttributes.Append(report.Value, path);
                }
            }
            DA.SetDataTree(2, filteredReportAttributes);
        }

        private List<Plane> ComputeReportPositions(List<GH_Mesh> meshList, List<int> reportCountList)
        {
            var pointList = new List<Plane>();
            int mesh_id = 0;

            foreach (var mesh in meshList)
            {
                if (mesh != null)
                {
                    var bbox = mesh.Value.GetBoundingBox(false);
                    var zTop = bbox.Corner(false, false, false).Z;
                    var xLeft = bbox.Corner(true, false, false).X;
                    var center = bbox.Center;
                    center.X = xLeft;
                    center.Z = zTop + reportCountList[mesh_id] * 2;

                    var plane = new Plane(center, Vector3d.XAxis, Vector3d.ZAxis);

                    pointList.Add(plane);
                }
                else
                {
                    pointList.Add(Plane.Unset);
                }

                mesh_id++;
            }

            return pointList;
        }

        private Tuple<List<string>, List<int>> GetFormatedReports(Dictionary<int, Dictionary<string, ReportAttribute>> reports, int initialReportCount)
        {
            List<string> report_output = new List<string>();
            List<int> line_count_output = new List<int>();

            foreach(var currShapeReps in reports)
            {
                int lineCount = currShapeReps.Value.Count + 1;

                List<string> reportArray = new List<string>();

                reportArray.Add("Reports of shape " + currShapeReps.Key.ToString());

                foreach(var report in currShapeReps.Value)
                {
                    reportArray.Add(report.Value.ToNiceString());
                }

                string formatedReports = string.Join("\n", reportArray.ToArray());

                while(report_output.Count < currShapeReps.Key)
                {
                    report_output.Add(String.Empty);
                    line_count_output.Add(0);
                }

                report_output.Add(formatedReports);
                line_count_output.Add(lineCount);
            }

            //Fill in empty reports to keep the synchronization.
            while(report_output.Count < initialReportCount)
            {
                report_output.Add(String.Empty);
                line_count_output.Add(0);
            }

            return Tuple.Create(report_output, line_count_output);
        }

        private Dictionary<int, Dictionary<string, ReportAttribute>> FilterReports(bool applyFilters,
                                                                                   List<Dictionary<string, ReportAttribute>> reports,
                                                                                   List<int> initialShapeIds,
                                                                                   List<string> reportKeys,
                                                                                   List<List<string>> valueKeys)
        {
            var filteredReports = new Dictionary<int, Dictionary<string, ReportAttribute>>();

            if (!applyFilters)
            {
                int i = 0;
                reports.ForEach(rep => { filteredReports.Add(i, rep);
                                         ++i; });
                return filteredReports;
            }

            // Filtering out reports that were not selected.
            if (initialShapeIds.Count > 0)
            {
                initialShapeIds.ForEach(i => filteredReports.Add(i, reports[i]));
            }
            else
            {
                // If the list of shape id is empty, keep all reports.
                for (int i = 0; i < reports.Count; i++)
                {
                    filteredReports.Add(i, reports[i]);
                }
            }

            //Filter out the report keys
            if (reportKeys.Count > 0 && reportKeys[0] != string.Empty)
            {
                // If there is only one string in the list, it could be a multiline string containing
                // the keys to keep.
                if (reportKeys.Count == 1 && reportKeys[0].Contains("\n"))
                {
                    char[] separators = { '\n', '\r' };
                    reportKeys = new List<string>(reportKeys[0].Split(separators, StringSplitOptions.RemoveEmptyEntries));
                }

                var tmpReports = new Dictionary<int, Dictionary<string, ReportAttribute>>();

                // Filter by report keys
                foreach(var currShape in filteredReports)
                {
                    foreach(var currReport in currShape.Value)
                    {
                        int keyId = reportKeys.FindIndex(x => x == currReport.Key);
                        if(keyId != -1)
                        {
                            if (!tmpReports.ContainsKey(currShape.Key))
                            {
                                tmpReports.Add(currShape.Key, new Dictionary<string, ReportAttribute>());
                            }
                            

                            // Filter by values
                            if(valueKeys.Count > keyId && valueKeys[keyId].Count > 0 && valueKeys[keyId][0] != string.Empty)
                            {
                                var values = valueKeys[keyId];
                                if(values.Contains(currReport.Value.getFormatedValue()))
                                {
                                    tmpReports[currShape.Key].Add(currReport.Key, currReport.Value);
                                }
                            }
                            else
                            {
                                tmpReports[currShape.Key].Add(currReport.Key, currReport.Value);
                            }
                        }
                    }
                }

                filteredReports = tmpReports;
            }
            
            return filteredReports;
        }

        private List<Dictionary<string, ReportAttribute>> GetReports(ref GH_Structure<IGH_Goo> reportTree)
        {
            var reports = new List<Dictionary<string, ReportAttribute>>();

            // extract the report tree to have a list of attribute by initial shape.
            foreach (var attr in reportTree.AllData(true))
            {
                var report = (ReportAttribute)attr;

                while (reports.Count <= report.ShapeID)
                {
                    reports.Add(new Dictionary<string, ReportAttribute>());
                }

                reports[report.ShapeID].Add(report.getKey(), report);
            }

            return reports;
        }

        private bool GetFilters(IGH_DataAccess DA, ref List<int> initialShapeIds, ref List<string> reportKeys, ref List<List<string>> valueKeys)
        {
            List<GH_Interval> id_intervals = new List<GH_Interval>();
            if(!DA.GetDataList(FILTER_SHAPE_ID, id_intervals))
            {
                return false;
            }

            // Going through the list of intervals to get every selected shape ids.
            foreach(var interval in id_intervals)
            {
                for(int i = (int)interval.Value.T0; i <= interval.Value.T1; ++i)
                {
                    initialShapeIds.Add(i);
                }
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
            var branches = valuesTree.Branches;

            foreach(var branch in branches)
            {
                var tmpList = new List<string>();

                branch.ForEach(x => tmpList.Add(x.Value));

                valueKeys.Add(tmpList);
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