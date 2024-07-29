/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
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

using Grasshopper.Kernel;
using Grasshopper.Kernel.Types;
using PumaGrasshopper.Properties;
using System;

namespace PumaGrasshopper
{
    public class ComponentReportsUnpacker : GH_Component
    {
        const string COMPONENT_NAME = "PumaReportsUnpacker";
        const string COMPONENT_NICK_NAME = "Puma Reports Unpacker";
        
        const string REPORT_INPUT_NAME = "Reports";
        const string REPORT_SHAPE_ID_NAME = "Shape IDs";
        const string REPORT_KEY_NAME = "Report Names";
        const string REPORT_VALUE_NAME = "Report Values";

        public ComponentReportsUnpacker()
          : base(COMPONENT_NAME, COMPONENT_NICK_NAME,
              "Unpacks Puma CGA reports into report names and values per input shape. (Version " + PRTWrapper.GetVersion() + ")",
              ComponentLibraryInfo.MainCategory, ComponentLibraryInfo.PumaSubCategory)
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddGenericParameter(REPORT_INPUT_NAME, REPORT_INPUT_NAME,
                "The Puma CGA reports to unpack.",
                GH_ParamAccess.item);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddNumberParameter(REPORT_SHAPE_ID_NAME, REPORT_SHAPE_ID_NAME,
                "The input shape IDs.",
                GH_ParamAccess.item);
            pManager.AddTextParameter(REPORT_KEY_NAME, REPORT_KEY_NAME,
                "The report names per input shape.",
                GH_ParamAccess.item);
            pManager.AddGenericParameter(REPORT_VALUE_NAME, REPORT_VALUE_NAME,
                "The report values per input shape.",
                GH_ParamAccess.item);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            IGH_Goo input = null;
            if(!DA.GetData(REPORT_INPUT_NAME, ref input))
            {
                return;
            }

            try
            {
                ReportAttribute report = (ReportAttribute)input;

                DA.SetData(REPORT_KEY_NAME, report.GetReportKey());
                DA.SetData(REPORT_SHAPE_ID_NAME, report.InitialShapeIndex);

                switch (report.GetReportType())
                {
                    case ReportTypes.PT_FLOAT:
                        DA.SetData(REPORT_VALUE_NAME, report.DoubleValue);
                        break;
                    case ReportTypes.PT_BOOL:
                        DA.SetData(REPORT_VALUE_NAME, report.BoolValue);
                        break;
                    case ReportTypes.PT_STRING:
                        DA.SetData(REPORT_VALUE_NAME, report.StringValue);
                        break;
                    default:
                        // Unsupported report type.
                        DA.SetData(REPORT_VALUE_NAME, "UNSUPPORTED REPORT TYPE");
                        break;
                }
            }
            catch (InvalidCastException e)
            {
                AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "Invalid input type: " + e.Message);
                return;
            }
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Resources.gh_prt_report_unpack;
            }
        }

        public override Guid ComponentGuid
        {
            get { return PumaUIDs.ComponentReportsUnpackerGuid; }
        }
    }
}
