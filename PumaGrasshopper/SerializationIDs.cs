/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
 *
 * Copyright (c) 2021-2025 Esri R&D Center Zurich
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
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PumaGrasshopper
{
    public static class SerializationIds
    {
        public const string VERSION = "VERSION";
        public const string GROUP_NAME = "GROUP_NAME";
        public const string GROUP_UID = "GROUP_UID";
        public const string EXPECTS_ARRAY = "EXPECTS_ARRAY";
        public const string ANNOTATION_COUNT = "ANNOTATION_COUNT";
        public const string ANNOTATION_TYPE = "ANNOTATION_TYPE";
        public const string ANNOTATION_ENUM_TYPE = "ANNOTATION_ENUM_TYPE";
        public const string ANNOTATION = "ANNOTATION";

        public const string DEFAULT_GROUP_NAME = "DEFAULT_GROUP";

        public const int SERIALIZATION_VERSION = 1;
    }
}
