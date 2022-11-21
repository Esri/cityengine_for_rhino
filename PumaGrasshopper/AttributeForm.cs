using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PumaGrasshopper
{
    public partial class AttributeForm : Form
    {
        List<RuleAttribute> RuleAttributes;
        AttributesValuesMap[] DefaultValues;
        public Point InitialLocation;
        public string SearchText;
        public int SelectedIndex;

        public AttributeForm(List<RuleAttribute> attributes, AttributesValuesMap[] defaultValues, Point loc)
        {
            InitializeComponent();
            InitialLocation = loc;
            RuleAttributes = attributes;
            DefaultValues = defaultValues;

            var groups = getAllGroups(attributes);
            ruleAttributeList.Groups.AddRange(groups);

            var attributeListItems = getAttributesListViewItems(groups.ToList());
            ruleAttributeList.Items.AddRange(attributeListItems.ToArray());
        }

        private string GetTypeString(Annotations.AnnotationArgumentType type)
        {
            switch(type)
            {
                case Annotations.AnnotationArgumentType.AAT_BOOL:
                    return "Boolean";
                case Annotations.AnnotationArgumentType.AAT_FLOAT:
                    return "Floating point";
                case Annotations.AnnotationArgumentType.AAT_STR:
                    return "Text";
                case Annotations.AnnotationArgumentType.AAT_INT:
                    return "Integer";
                case Annotations.AnnotationArgumentType.AAT_BOOL_ARRAY:
                    return "Boolean array";
                case Annotations.AnnotationArgumentType.AAT_FLOAT_ARRAY:
                    return "Floating point array";
                case Annotations.AnnotationArgumentType.AAT_STR_ARRAY:
                    return "Text array";
                case Annotations.AnnotationArgumentType.AAT_INT_ARRAY:
                    return "Integer array";
                case Annotations.AnnotationArgumentType.AAT_VOID:
                    return "No type";
                default:
                    return "Unknown";
            }
        }

        private string GetDefaultValueString(string attributeName, Annotations.AnnotationArgumentType type)
        {
            if (DefaultValues.Length > 1) return "Multiple";

            var defaultValueMap = DefaultValues.First();
            switch (type)
            {
                case Annotations.AnnotationArgumentType.AAT_BOOL:
                    {
                        defaultValueMap.GetBool(attributeName, out bool val);
                        return val.ToString();
                    }
                case Annotations.AnnotationArgumentType.AAT_FLOAT:
                    {
                        defaultValueMap.GetDouble(attributeName, out double val);
                        return val.ToString();
                    }
                case Annotations.AnnotationArgumentType.AAT_STR:
                    {
                        defaultValueMap.GetString(attributeName, out string val);
                        return val;
                    }
                case Annotations.AnnotationArgumentType.AAT_INT:
                    {
                        defaultValueMap.GetInteger(attributeName, out int val);
                        return val.ToString();
                    }
                case Annotations.AnnotationArgumentType.AAT_BOOL_ARRAY:
                    {
                        defaultValueMap.GetBoolArray(attributeName, out bool[] vals);
                        return vals.Aggregate("", (left, right) => left + right + ";");
                    }
                case Annotations.AnnotationArgumentType.AAT_FLOAT_ARRAY:
                    {
                        defaultValueMap.GetDoubleArray(attributeName, out double[] vals);
                        return vals.Aggregate("", (left, right) => left + right + ";");
                    }
                case Annotations.AnnotationArgumentType.AAT_STR_ARRAY:
                    {
                        defaultValueMap.GetStringArray(attributeName, out string[] vals);
                        return vals.Aggregate("", (left, right) => left + right + ";");
                    }
                case Annotations.AnnotationArgumentType.AAT_INT_ARRAY:
                    {
                        defaultValueMap.GetIntegerArray(attributeName, out int[] vals);
                        return vals.Aggregate("", (left, right) => left + right + ";");
                    }
                default:
                    return "No default value";
            }
        }

        private List<ListViewItem> getAttributesListViewItems(List<ListViewGroup> groups)
        {
            List<ListViewItem> listViewItems = RuleAttributes.ConvertAll(
                attribute => {
                    return new ListViewItem(new string[] {
                            attribute.mNickname,
                            GetTypeString(attribute.mAttribType),
                            attribute.GetDescriptions(),
                            GetDefaultValueString(attribute.mFullName, attribute.mAttribType)
                        },
                        group: groups.Find(group => group.Header == attribute.mGroup));
                    }
                );

            return listViewItems;
        }

        private ListViewGroup[] getAllGroups(List<RuleAttribute> attributes)
        {
            var groupNames = attributes.ConvertAll(attr => attr.mGroup);

            HashSet<string> groups = new HashSet<string>();
            groupNames.ForEach(groupName => groups.Add(groupName));

            List<ListViewGroup> groupListView = new List<ListViewGroup>();
            groups.ToList().ForEach((groupName) => groupListView.Add(new ListViewGroup(groupName)));

            return groupListView.ToArray();
        }

        private void AttributeForm_Load(object sender, EventArgs e)
        {
            SetDesktopLocation(InitialLocation.X, InitialLocation.Y);
        }

        private void Ok_Click(object sender, EventArgs e)
        {
            SelectedIndex = ruleAttributeList.SelectedIndices[0];
            DialogResult = DialogResult.OK;
            Close();
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
