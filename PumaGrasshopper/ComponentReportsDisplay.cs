﻿/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
 *
 * Copyright (c) 2021-2024 Esri R&D Center Zurich
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
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using PumaGrasshopper.Properties;
using Rhino.Geometry;

namespace PumaGrasshopper
{
    public class ComponentReportsDisplay : GH_Component
    {
        const string COMPONENT_NAME = "PumaReportsDisplay";
        const string COMPONENT_NICK_NAME = "Puma Reports Display";

        const string MESH_INPUT_NAME = "Models";
        const string REPORTS_INPUT_NAME = "Reports";
        const string FILTER_SHAPE_ID_NAME = "Shape ID Filter";
        const string FILTER_REPORT_KEY_NAME = "Report Key Filter";
        const string FILTER_REPORT_VALUE_NAME = "Report Value Filter";
        const string REPORTS_DISPLAY_PARAM_NAME = "Reports Display";
        const string REPORTS_LOCATION_NAME = "Reports Location";
        const string REPORTS_FILTERED_NAME = "Reports";

        public ComponentReportsDisplay()
          : base(COMPONENT_NAME, COMPONENT_NICK_NAME,
              "Filters and prepares Puma CGA reports for display in the viewport. (Version " + PRTWrapper.GetVersion() + ")",
              ComponentLibraryInfo.MainCategory, ComponentLibraryInfo.PumaSubCategory)
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter(MESH_INPUT_NAME, MESH_INPUT_NAME,
                "The Puma models are used to compute where to display the report values.",
                GH_ParamAccess.tree);
            pManager.AddGenericParameter(REPORTS_INPUT_NAME, REPORTS_INPUT_NAME,
                "The CGA reports generated by Puma.",
                GH_ParamAccess.tree);

            // The 3 filter options: list of GH_Interval on initial shape. String or list of String to filter keys.
            pManager.AddIntervalParameter(FILTER_SHAPE_ID_NAME, FILTER_SHAPE_ID_NAME,
                "Index interval(s) of desired input shape IDs.",
                GH_ParamAccess.list, Interval.Unset);
            pManager.AddTextParameter(FILTER_REPORT_KEY_NAME, FILTER_REPORT_KEY_NAME,
                "The Puma CGA report keys to select.",
                GH_ParamAccess.list, string.Empty);
            pManager.AddTextParameter(FILTER_REPORT_VALUE_NAME, FILTER_REPORT_VALUE_NAME,
                "The Puma CGA report values to select.",
                GH_ParamAccess.tree, string.Empty);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            //There are 2 outputs: First one is the report preview location.
            //                     Second one is the report preview, to display the report data in rhino.
            pManager.AddPlaneParameter(REPORTS_LOCATION_NAME, REPORTS_LOCATION_NAME,
                "Point location indicating the text preview position.",
                GH_ParamAccess.list);
            pManager.AddTextParameter(REPORTS_DISPLAY_PARAM_NAME, REPORTS_DISPLAY_PARAM_NAME, 
                "Formated text reports that can be displayed in Rhino using a text preview component.",
                GH_ParamAccess.list);
            pManager.AddGenericParameter(REPORTS_FILTERED_NAME, REPORTS_FILTERED_NAME,
                "Filtered reports outputed for further processing.",
                GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            bool locateFromMeshes = DA.GetDataTree(0, out GH_Structure<GH_Mesh> meshTree);

            if(!DA.GetDataTree<IGH_Goo>(1, out GH_Structure<IGH_Goo> reportTree))
                return;
            var reports = GetReports(ref reportTree);

            List<int> initialShapeIds = new List<int>();
            List<string> reportKeys = new List<string>();
            List<List<string>> valueKeys = new List<List<string>>();
            bool applyFilters = GetFilters(DA, ref initialShapeIds, ref reportKeys, ref valueKeys);

            var filteredReports = FilterReports(applyFilters, reports, initialShapeIds, reportKeys, valueKeys);

            // Format the text output
            var previewReports = GetFormatedReports(filteredReports, meshTree.PathCount);

            if (locateFromMeshes)
            {
                var pointList = ComputeReportPositions(meshTree, previewReports.Item2);

                DA.SetDataList(REPORTS_LOCATION_NAME, pointList);
            }

            DA.SetDataList(REPORTS_DISPLAY_PARAM_NAME, previewReports.Item1);

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

        private List<Plane> ComputeReportPositions(GH_Structure<GH_Mesh> meshTree, List<int> reportCountList)
        {
            var pointList = new List<Plane>();
            int mesh_id = 0;

            // for each mesh branch, locate the highest point.
            foreach (var meshBundle in meshTree.Branches)
            {
                if (meshBundle != null && meshBundle.Count > 0)
                {
                    // find highest point
                    BoundingBox bbox;
                    double zTop;
                    double xLeft;
                    Point3d center = meshBundle[0].Value.GetBoundingBox(false).Center;

                    foreach (var m in meshBundle)
                    {
                        bbox = m.Value.GetBoundingBox(false);
                        zTop = bbox.Corner(false, false, false).Z;
                        xLeft = bbox.Corner(true, false, false).X;
                        Point3d new_center = bbox.Center;
                        center.X = (center.X + xLeft) * 0.5;
                        center.Z = Math.Max(center.Z, zTop);
                    }
                    center.Z += reportCountList[mesh_id] * 2;

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

                var reportArray = new List<string>();

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
                                if(values.Contains(currReport.Value.GetFormatedValue()))
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

                while (reports.Count <= report.InitialShapeIndex)
                {
                    reports.Add(new Dictionary<string, ReportAttribute>());
                }

                reports[report.InitialShapeIndex].Add(report.GetReportKey(), report);
            }

            return reports;
        }

        private bool GetFilters(IGH_DataAccess DA, ref List<int> initialShapeIds, ref List<string> reportKeys, ref List<List<string>> valueKeys)
        {
            List<GH_Interval> id_intervals = new List<GH_Interval>();
            if(!DA.GetDataList(FILTER_SHAPE_ID_NAME, id_intervals))
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

            if(!DA.GetDataList(FILTER_REPORT_KEY_NAME, reportKeys))
            {
                return false;
            }

            GH_Structure<GH_String> valuesTree = new GH_Structure<GH_String>();
            if(!DA.GetDataTree(FILTER_REPORT_VALUE_NAME, out valuesTree))
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

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Resources.gh_prt_report_filter;
            }
        }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.ComponentReportsDislayGuid; }
        }
    }
}
