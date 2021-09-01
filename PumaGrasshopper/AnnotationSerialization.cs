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
                string annot_xml = Utils.ToXML(annotations[i]);

                AttributeAnnotation annotType = annotations[i].GetAnnotationType();
                if (annotType == AttributeAnnotation.A_ENUM)
                {
                    EnumAnnotationType enumType = annotations[i].GetEnumType();
                    writer.SetInt32(SerializationIds.ANNOTATION_ENUM_TYPE, i, (int)enumType);
                }

                writer.SetInt32(SerializationIds.ANNOTATION_TYPE, i, (int)annotType);
                writer.SetString(SerializationIds.ANNOTATION, i, annot_xml);
            }
        }

        public static List<Base> ReadAnnotations(GH_IReader reader)
        {
            int annotation_count = reader.GetInt32(SerializationIds.ANNOTATION_COUNT);
            List<Base> annotations = new List<Base>(annotation_count);

            for (int i = 0; i < annotation_count; ++i)
            {
                string annot_xml = reader.GetString(SerializationIds.ANNOTATION, i);
                int annot_type = reader.GetInt32(SerializationIds.ANNOTATION_TYPE, i);

                int annot_enum_type = (AttributeAnnotation)annot_type == AttributeAnnotation.A_ENUM ? reader.GetInt32(SerializationIds.ANNOTATION_ENUM_TYPE, i) : 0;
                Base annot = FromXML((AttributeAnnotation)annot_type, annot_xml, (EnumAnnotationType)annot_enum_type);
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
