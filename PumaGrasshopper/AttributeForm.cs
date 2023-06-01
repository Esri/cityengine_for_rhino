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
        List<ListViewGroup> Groups;
        List<string> Imports;
        AttributesValuesMap[] DefaultValues;
        public Point InitialLocation;
        public string SearchText;
        public int SelectedIndex;
        private Dictionary<string, ListView> listViews;
        private Dictionary<string, TabPage> tabs;

        public AttributeForm(List<RuleAttribute> attributes, AttributesValuesMap[] defaultValues, Point loc)
        {
            InitializeComponent();
            InitialLocation = loc;
            RuleAttributes = attributes;
            DefaultValues = defaultValues;
            SearchText = String.Empty;

            Groups = getAllGroups(attributes);

            tabs = new Dictionary<string, TabPage>();
            var imports = getAllImports(attributes);
            imports.RemoveAll(import => import == null);
            Imports = new List<string>();
            Imports.Add("Main file");
            Imports.AddRange(imports);
            Imports.ForEach(import => tabs.Add(import, new TabPage
            {
                AutoScroll = true,
                Name = import,
                Location = new System.Drawing.Point(10, 48),
                Padding = new System.Windows.Forms.Padding(3),
                Size = new System.Drawing.Size(1304, 1294),
                TabIndex = 0,
                Text = import,
                UseVisualStyleBackColor = true,
            }));

            listViews = new Dictionary<string, ListView>();
            Imports.ForEach(import => listViews.Add(import, getListView(import)));

            this.flowLayoutPanel1.SuspendLayout();
            this.flowLayoutPanel2.SuspendLayout();
            this.tabContainer.SuspendLayout();
            this.SuspendLayout();

            listViews.Keys.ToList().ForEach(import => tabs[import].Controls.Add(listViews[import]));
            tabContainer.TabPages.AddRange(tabs.Values.ToArray());

            UpdateListView();

            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.flowLayoutPanel2.ResumeLayout(false);
            this.tabContainer.ResumeLayout(false);
            this.ResumeLayout(false);
        }

        private void UpdateListView()
        {
            listViews.Values.ToList().ForEach(l => l.Items.Clear());

            List<RuleAttribute> elligibleAttributes = SearchText == string.Empty ?
                RuleAttributes : 
                RuleAttributes.FindAll(attribute => attribute.mFullName.Contains(SearchText));
            
            getAttributesListViewItems(elligibleAttributes);
        }

        private ListView getListView(string importName)
        { 
            var ruleAttributeList = new ListView();
            ruleAttributeList.Alignment = ListViewAlignment.Left;
            ruleAttributeList.AutoArrange = false;
            ruleAttributeList.BackgroundImageTiled = true;
            ruleAttributeList.Columns.AddRange(new ColumnHeader[] {
                (ColumnHeader)ruleAttributeColumn.Clone(),
                (ColumnHeader)attributeType.Clone(),
                (ColumnHeader)defaultValue.Clone()});
            ruleAttributeList.Dock = DockStyle.Fill;
            ruleAttributeList.FullRowSelect = true;
            ruleAttributeList.HeaderStyle = ColumnHeaderStyle.Nonclickable;
            ruleAttributeList.HideSelection = false;
            ruleAttributeList.Location = new Point(3, 3);
            ruleAttributeList.MultiSelect = false;
            ruleAttributeList.Name = importName;
            ruleAttributeList.Size = new Size(1298, 1288);
            ruleAttributeList.TabIndex = 1;
            ruleAttributeList.UseCompatibleStateImageBehavior = false;
            ruleAttributeList.View = View.Details;

            ruleAttributeList.Groups.AddRange(Groups.ToArray());

            return ruleAttributeList;
        }

        private string GetTypeString(Annotations.AnnotationArgumentType type)
        {
            switch(type)
            {
                case Annotations.AnnotationArgumentType.AAT_BOOL:
                    return "Boolean";
                case Annotations.AnnotationArgumentType.AAT_FLOAT:
                    return "Number";
                case Annotations.AnnotationArgumentType.AAT_STR:
                    return "String";
                case Annotations.AnnotationArgumentType.AAT_INT:
                    return "Number";
                case Annotations.AnnotationArgumentType.AAT_BOOL_ARRAY:
                    return "Boolean[]";
                case Annotations.AnnotationArgumentType.AAT_FLOAT_ARRAY:
                    return "Number[]";
                case Annotations.AnnotationArgumentType.AAT_STR_ARRAY:
                    return "String[]";
                case Annotations.AnnotationArgumentType.AAT_INT_ARRAY:
                    return "Number[]";
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

        private void getAttributesListViewItems(List<RuleAttribute> ruleAttributes)
        {
            ruleAttributes.ForEach(
                attribute => {
                    var item = new ListViewItem(new string[] {
                        "   " + attribute.mNickname,
                        GetTypeString(attribute.mAttribType),
                        GetDefaultValueString(attribute.mFullName, attribute.mAttribType)
                    },
                    group: Groups.Find(group => group.Header == attribute.getFullGroup()));

                    item.Name = attribute.mFullName;
                    
                    var import = attribute.getImport();

                    if (import == null) import = "Main file";

                    listViews[import].Items.Add(item);
                }
            );
        }

        private List<string> getAllImports(List<RuleAttribute> attributes) => attributes.ConvertAll(attr => attr.getImport()).Distinct().ToList();

        private List<ListViewGroup> getAllGroups(List<RuleAttribute> attributes)
        {
            var groupNames = attributes.ConvertAll(attr => attr.getFullGroup());
            var groups = groupNames.Distinct().ToList();

            List<ListViewGroup> groupListView = new List<ListViewGroup>();
            groups.ForEach((groupName) => groupListView.Add(new ListViewGroup(groupName)));

            return groupListView;
        }

        private void AttributeForm_Load(object sender, EventArgs e)
        {
            SetDesktopLocation(InitialLocation.X, InitialLocation.Y);
        }

        private void Ok_Click(object sender, EventArgs e)
        {
            var selectedTabIndex = tabContainer.SelectedIndex;
            
            if(selectedTabIndex > -1 && listViews.ElementAt(selectedTabIndex).Value.SelectedIndices.Count > 0)
            {
                var attributeName = listViews.ElementAt(selectedTabIndex).Value.SelectedItems[0].Name;

                SelectedIndex = RuleAttributes.FindIndex(attribute => attribute.mFullName == attributeName);
                DialogResult = DialogResult.OK;
            } else
            {
                DialogResult = DialogResult.Cancel;
            }
            
            Close();
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void SearchTextBox_TextChanged(object sender, EventArgs e)
        {
            SearchText = ((TextBox) sender).Text;
            UpdateListView();
        }
    }
}
