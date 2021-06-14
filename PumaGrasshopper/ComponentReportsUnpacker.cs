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

        /// <summary>
        /// Initializes a new instance of the PumaGrasshopperReportsOutput class.
        /// </summary>
        public ComponentReportsUnpacker()
          : base(COMPONENT_NAME, COMPONENT_NICK_NAME,
              "Unpacks Puma CGA reports into report names and values per input shape. (Version " + PRTWrapper.GetVersion() + ")",
              ComponentLibraryInfo.MainCategory, ComponentLibraryInfo.PumaSubCategory)
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddGenericParameter(REPORT_INPUT_NAME, REPORT_INPUT_NAME,
                "The Puma CGA reports to unpack.",
                GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
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

        /// <summary>
        /// This is the method that actually does the work.
        /// Takes the given ReportAttribute and unpack the values.
        /// </summary>
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
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

                DA.SetData(REPORT_KEY_NAME, report.getKey());
                DA.SetData(REPORT_SHAPE_ID_NAME, report.InitialShapeIndex);

                switch (report.getType())
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
