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

using System;
using System.Drawing;
using Grasshopper.Kernel;

namespace PumaGrasshopper
{
    public class ComponentLibraryInfo : GH_AssemblyInfo
    {
        public override string Name
        {
            get
            {
                return "Puma Component Library " + PRTWrapper.GetVersion();
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
                return "Puma provides a collection of components to run CityEngine CGA rules within Grasshopper.";
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
                return "Esri Online LLC";
            }
        }
        public override string AuthorContact
        {
            get
            {
                //Return a string representing your preferred contact details.
                return "cityengine-info@esri.com";
            }
        }

        public const string MainCategory = "Esri";
        public const string PumaSubCategory = "Puma";
    }
}
