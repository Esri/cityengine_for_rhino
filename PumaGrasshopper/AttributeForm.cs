/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2023 Esri R&D Center Zurich
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
        private static readonly string MAIN_FILE_TAB_NAME = "Main Rule File";

        readonly List<RuleAttribute> RuleAttributes;
        readonly List<ListViewGroup> Groups;
        readonly List<string> Imports;
        readonly AttributesValuesMap[] DefaultValues;
        public Point InitialLocation;
        public string SearchText;
        public int SelectedIndex;
        private readonly Dictionary<string, ListView> listViews;
        private readonly Dictionary<string, TabPage> tabs;

        public AttributeForm(List<RuleAttribute> attributes, AttributesValuesMap[] defaultValues, Point loc)
        {
            InitializeComponent();
            InitialLocation = loc;
            RuleAttributes = attributes;
            DefaultValues = defaultValues;
            SearchText = String.Empty;

            Groups = GetAllGroups(attributes);

            tabs = new Dictionary<string, TabPage>();
            var imports = GetAllImports(attributes);
            imports.RemoveAll(import => import == null);
            Imports = new List<string>() { MAIN_FILE_TAB_NAME };
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
            Imports.ForEach(import => listViews.Add(import, GetListView(import)));

            this.mainTableLayout.SuspendLayout();
            this.flowLayoutPanel2.SuspendLayout();
            this.tabContainer.SuspendLayout();
            this.SuspendLayout();

            listViews.Keys.ToList().ForEach(import => tabs[import].Controls.Add(listViews[import]));
            tabContainer.TabPages.AddRange(tabs.Values.ToArray());

            UpdateListView();

            this.mainTableLayout.ResumeLayout(false);
            this.mainTableLayout.PerformLayout();
            this.flowLayoutPanel2.ResumeLayout(false);
            this.tabContainer.ResumeLayout(false);
            this.ResumeLayout(false);

            this.AcceptButton = OkBtn;
            this.CancelButton = CancelBtn;
        }

        private void UpdateListView()
        {
            listViews.Values.ToList().ForEach(l => l.Items.Clear());

            List<RuleAttribute> elligibleAttributes = SearchText == string.Empty ?
                RuleAttributes : 
                RuleAttributes.FindAll(attribute => attribute.mNickname.ToLower().Contains(SearchText.ToLower()));
            
            GetAttributesListViewItems(elligibleAttributes);
        }

        private ListView GetListView(string importName)
        {
            var ruleAttributeList = new ListView() { 
                Alignment = ListViewAlignment.Left,
                AutoArrange = false,
                BackgroundImageTiled = true,
                Dock = DockStyle.Fill,
                FullRowSelect = true,
                HeaderStyle = ColumnHeaderStyle.Nonclickable,
                HideSelection = false,
                Location = new Point(3, 3),
                MultiSelect = false,
                Name = importName,
                Size = new Size(1298, 1288),
                TabIndex = 1,
                UseCompatibleStateImageBehavior = false,
                View = View.Details
            };

            ruleAttributeList.Columns.AddRange(new ColumnHeader[] {
                (ColumnHeader)ruleAttributeColumn.Clone(),
                (ColumnHeader)attributeType.Clone(),
                (ColumnHeader)defaultValue.Clone()});

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

        private void GetAttributesListViewItems(List<RuleAttribute> ruleAttributes)
        {
            ruleAttributes.ForEach(
                attribute => {
                    var item = new ListViewItem(new string[] {
                        "   " + attribute.mNickname,
                        GetTypeString(attribute.mAttribType),
                        GetDefaultValueString(attribute.mFullName, attribute.mAttribType)
                    },
                    group: Groups.Find(group => group.Header == attribute.GetFullGroup()));

                    item.Name = attribute.mFullName;
                    
                    var import = attribute.GetImportWithoutStylePrefix();

                    if (import == null)
                        import = MAIN_FILE_TAB_NAME;

                    listViews[import].Items.Add(item);
                }
            );
        }

        private List<string> GetAllImports(List<RuleAttribute> attributes) => attributes.ConvertAll(attr => attr.GetImportWithoutStylePrefix()).Distinct().ToList();

        private List<ListViewGroup> GetAllGroups(List<RuleAttribute> attributes)
        {
            var groupNames = attributes.ConvertAll(attr => attr.GetFullGroup());
            var groups = groupNames.Distinct().ToList();
            groups.RemoveAll(item => item == null);

            List<ListViewGroup> groupListView = new List<ListViewGroup>();
            groups.ForEach((groupName) => groupListView.Add(new ListViewGroup(groupName)));

            return groupListView;
        }

        private void AttributeForm_Load(object sender, EventArgs e)
        {
            SetDesktopLocation(InitialLocation.X, InitialLocation.Y);
        }

        private void OkBtn_Click(object sender, EventArgs e)
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
            SearchText = ((TextBox)sender).Text;
            UpdateListView();
        }
    }
}
