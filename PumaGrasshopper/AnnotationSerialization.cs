/**
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

using GH_IO.Serialization;
using PumaGrasshopper.Annotations;
using PumaGrasshopper.AttributeParameter;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml.Serialization;

namespace PumaGrasshopper
{
    class AnnotationSerialization
    {
        public static void WriteAnnotations(GH_IWriter writer, List<Base> annotations)
        {
            writer.SetInt32(SerializationIds.ANNOTATION_COUNT, annotations.Count);

            for (int i = 0; i < annotations.Count; ++i)
            {
                // use xml serializer to serialize annotations to xml string
                string annotXml = Utils.ToXML(annotations[i]);

                AttributeAnnotation annotType = annotations[i].GetAnnotationType();
                if (annotType == AttributeAnnotation.A_ENUM)
                {
                    EnumAnnotationType enumType = annotations[i].GetEnumType();
                    writer.SetInt32(SerializationIds.ANNOTATION_ENUM_TYPE, i, (int)enumType);
                }

                writer.SetInt32(SerializationIds.ANNOTATION_TYPE, i, (int)annotType);
                writer.SetString(SerializationIds.ANNOTATION, i, annotXml);
            }
        }

        public static List<Base> ReadAnnotations(GH_IReader reader)
        {
            int annotationCount = 0;
            if (!reader.TryGetInt32(SerializationIds.ANNOTATION_COUNT, ref annotationCount))
                return new List<Base>();

            List<Base> annotations = new List<Base>(annotationCount);

            for (int i = 0; i < annotationCount; ++i)
            {
                string annotXml = reader.GetString(SerializationIds.ANNOTATION, i);
                int annotType = reader.GetInt32(SerializationIds.ANNOTATION_TYPE, i);

                int annotEnumType = (AttributeAnnotation)annotType == AttributeAnnotation.A_ENUM ? reader.GetInt32(SerializationIds.ANNOTATION_ENUM_TYPE, i) : 0;
                Base annot = FromXML((AttributeAnnotation)annotType, annotXml, (EnumAnnotationType)annotEnumType);
                annotations.Add(annot);
            }

            return annotations;
        }

        public static Base FromXML(AttributeAnnotation annotationType,
                                   string annotationXML,
                                   EnumAnnotationType enumType = EnumAnnotationType.ENUM_DOUBLE)
        {
            XmlSerializer serializer = SerializerFromAnnotType(annotationType, enumType);
            MemoryStream stream = ToMemoryStream(annotationXML);
            return (Base)serializer.Deserialize(stream);
        }

        private static XmlSerializer SerializerFromAnnotType(AttributeAnnotation annotationType, EnumAnnotationType enumType = EnumAnnotationType.ENUM_DOUBLE)
        {
            switch (annotationType)
            {
                case AttributeAnnotation.A_ENUM:
                    Type type = GetEnumType(enumType);
                    return new XmlSerializer(type);

                case AttributeAnnotation.A_RANGE:
                    return new XmlSerializer(typeof(Range));

                case AttributeAnnotation.A_COLOR:
                    return new XmlSerializer(typeof(Color));

                case AttributeAnnotation.A_FILE:
                    return new XmlSerializer(typeof(Annotations.File));

                case AttributeAnnotation.A_DIR:
                    return new XmlSerializer(typeof(Annotations.Directory));

                default:
                    return new XmlSerializer(typeof(Base));
            }
        }

        private static Type GetEnumType(EnumAnnotationType enumType)
        {
            switch (enumType)
            {
                case EnumAnnotationType.ENUM_DOUBLE:
                    return typeof(Enum<double>);

                case EnumAnnotationType.ENUM_BOOL:
                    return typeof(Enum<bool>);

                case EnumAnnotationType.ENUM_STRING:
                    return typeof(Enum<string>);

                default: throw new ArgumentException("enumType is invalid");
            }
        }

        private static MemoryStream ToMemoryStream(string text)
        {
            byte[] byteArray = Encoding.ASCII.GetBytes(text);
            return new MemoryStream(byteArray);
        }
    }
}
