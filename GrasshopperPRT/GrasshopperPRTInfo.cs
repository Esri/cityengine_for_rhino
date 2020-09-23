using System;
using System.Drawing;
using Grasshopper.Kernel;

namespace GrasshopperPRT
{
    public class GrasshopperPRTInfo : GH_AssemblyInfo
    {
        public override string Name
        {
            get
            {
                return "GrasshopperPRT";
            }
        }
        public override Bitmap Icon
        {
            get
            {
                //Return a 24x24 pixel bitmap to represent this GHA library.
                return null;
            }
        }
        public override string Description
        {
            get
            {
                //Return a short string describing the purpose of this GHA library.
                return "Provide the PRT engine to Rhino and Grasshopper.\n\t" +
                       "Version: " + Version;
            }
        }
        public override Guid Id
        {
            get
            {
                return new Guid("4d67eed0-6e0a-4bf0-ac44-f84d0f07c27a");
            }
        }

        public override string AuthorName
        {
            get
            {
                //Return a string identifying you or your company.
                return "Esri";
            }
        }
        public override string AuthorContact
        {
            get
            {
                //Return a string representing your preferred contact details.
                return "laiulfi@esri.com";
            }
        }
    }
}
