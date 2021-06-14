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

using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Grasshopper.Kernel.Types;
using PumaGrasshopper.Properties;
using Rhino.Geometry;

namespace PumaGrasshopper
{
    public class PumaGrasshopperReportsOutput : GH_Component
    {
        const string REPORT_INPUT = "Reports";
        const string REPORT_SHAPE_ID = "Shape ID";
        const string REPORT_KEY = "Report Name";
        const string REPORT_VALUE = "Report Value";

        /// <summary>
        /// Initializes a new instance of the PumaGrasshopperReportsOutput class.
        /// </summary>
        public PumaGrasshopperReportsOutput()
          : base("PumaCgaReportsOutput", "Reports Outputs",
              "Version: " + PRTWrapper.GetVersion() + ". " +
              "Unpacks the report attributes objects",
              "Special", "Esri")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddGenericParameter(REPORT_INPUT, REPORT_INPUT, "The report attributes to process.", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddTextParameter(REPORT_KEY, REPORT_KEY, "The report name", GH_ParamAccess.item);
            pManager.AddNumberParameter(REPORT_SHAPE_ID, REPORT_SHAPE_ID, "The initial shape id of this report.", GH_ParamAccess.item);
            pManager.AddGenericParameter(REPORT_VALUE, REPORT_VALUE, "The value associated with this report.", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// Takes the given ReportAttribute and unpack the values.
        /// </summary>
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            IGH_Goo input = null;
            if(!DA.GetData(REPORT_INPUT, ref input))
            {
                return;
            }

            try
            {
                ReportAttribute report = (ReportAttribute)input;

                DA.SetData(REPORT_KEY, report.getKey());
                DA.SetData(REPORT_SHAPE_ID, report.InitialShapeIndex);

                switch (report.getType())
                {
                    case ReportTypes.PT_FLOAT:
                        DA.SetData(REPORT_VALUE, report.DoubleValue);
                        break;
                    case ReportTypes.PT_BOOL:
                        DA.SetData(REPORT_VALUE, report.BoolValue);
                        break;
                    case ReportTypes.PT_STRING:
                        DA.SetData(REPORT_VALUE, report.StringValue);
                        break;
                    default:
                        // Unsupported report type.
                        DA.SetData(REPORT_VALUE, "UNSUPPORTED REPORT TYPE");
                        break;
                }
            }
            catch (InvalidCastException e)
            {
                AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "Invalid input type: " + e.Message);
                return;
            }
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
                return Resources.gh_prt_report_unpack;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("23602a6d-1137-4403-867a-082e001ca707"); }
        }
    }
}
