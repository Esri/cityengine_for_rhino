﻿using System;
using System.Drawing;
using Grasshopper.Kernel;

namespace PumaGrasshopper
{
    public class PumaGrasshopperInfo : GH_AssemblyInfo
    {
        public override string Name
        {
            get
            {
                return "Puma";
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
                return "Version: " + PRTWrapper.GetVersion() + ". " +
                       "Provide access to the CityEngine PRT engine in Grasshopper.";
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