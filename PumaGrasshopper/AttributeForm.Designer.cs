
namespace PumaGrasshopper
{
    partial class AttributeForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.CancelBtn = new System.Windows.Forms.Button();
            this.OkBtn = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.ruleAttributeList = new System.Windows.Forms.ListView();
            this.ruleAttributeColumn = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.attributeType = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.defaultValue = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
            this.flowLayoutPanel1.SuspendLayout();
            this.flowLayoutPanel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // CancelBtn
            // 
            this.CancelBtn.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CancelBtn.Location = new System.Drawing.Point(1081, 7);
            this.CancelBtn.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.CancelBtn.Name = "CancelBtn";
            this.CancelBtn.Size = new System.Drawing.Size(238, 88);
            this.CancelBtn.TabIndex = 1;
            this.CancelBtn.Text = "Cancel";
            this.CancelBtn.UseVisualStyleBackColor = true;
            this.CancelBtn.Click += new System.EventHandler(this.Cancel_Click);
            // 
            // OkBtn
            // 
            this.OkBtn.Location = new System.Drawing.Point(821, 7);
            this.OkBtn.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.OkBtn.Name = "OkBtn";
            this.OkBtn.Size = new System.Drawing.Size(244, 88);
            this.OkBtn.TabIndex = 2;
            this.OkBtn.Text = "Ok";
            this.OkBtn.UseVisualStyleBackColor = true;
            this.OkBtn.Click += new System.EventHandler(this.Ok_Click);
            // 
            // textBox1
            // 
            this.textBox1.AccessibleName = "ruleAttributeSearchField";
            this.textBox1.AccessibleRole = System.Windows.Forms.AccessibleRole.Text;
            this.textBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.textBox1.Location = new System.Drawing.Point(3, 35);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(1322, 38);
            this.textBox1.TabIndex = 4;
            this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 0);
            this.label1.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(579, 32);
            this.label1.TabIndex = 3;
            this.label1.Text = "Available Attributes of current Rule Package:";
            // 
            // ruleAttributeList
            // 
            this.ruleAttributeList.Alignment = System.Windows.Forms.ListViewAlignment.Left;
            this.ruleAttributeList.AutoArrange = false;
            this.ruleAttributeList.BackgroundImageTiled = true;
            this.ruleAttributeList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ruleAttributeColumn,
            this.attributeType,
            this.defaultValue});
            this.ruleAttributeList.FullRowSelect = true;
            this.ruleAttributeList.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.ruleAttributeList.HideSelection = false;
            this.ruleAttributeList.Location = new System.Drawing.Point(3, 79);
            this.ruleAttributeList.MultiSelect = false;
            this.ruleAttributeList.Name = "ruleAttributeList";
            this.ruleAttributeList.Size = new System.Drawing.Size(1324, 1342);
            this.ruleAttributeList.TabIndex = 1;
            this.ruleAttributeList.UseCompatibleStateImageBehavior = false;
            this.ruleAttributeList.View = System.Windows.Forms.View.Details;
            // 
            // ruleAttributeColumn
            // 
            this.ruleAttributeColumn.Text = "Rule attribute";
            this.ruleAttributeColumn.Width = 301;
            // 
            // attributeType
            // 
            this.attributeType.Text = "Type";
            this.attributeType.Width = 255;
            // 
            // defaultValue
            // 
            this.defaultValue.Text = "Default value";
            this.defaultValue.Width = 250;
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.flowLayoutPanel1.Controls.Add(this.label1);
            this.flowLayoutPanel1.Controls.Add(this.textBox1);
            this.flowLayoutPanel1.Controls.Add(this.ruleAttributeList);
            this.flowLayoutPanel1.Location = new System.Drawing.Point(7, 12);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(1327, 1431);
            this.flowLayoutPanel1.TabIndex = 5;
            // 
            // flowLayoutPanel2
            // 
            this.flowLayoutPanel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.flowLayoutPanel2.Controls.Add(this.CancelBtn);
            this.flowLayoutPanel2.Controls.Add(this.OkBtn);
            this.flowLayoutPanel2.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
            this.flowLayoutPanel2.Location = new System.Drawing.Point(7, 1462);
            this.flowLayoutPanel2.Name = "flowLayoutPanel2";
            this.flowLayoutPanel2.Size = new System.Drawing.Size(1327, 107);
            this.flowLayoutPanel2.TabIndex = 7;
            // 
            // AttributeForm
            // 
            this.AcceptButton = this.OkBtn;
            this.AutoScaleDimensions = new System.Drawing.SizeF(16F, 31F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.CancelBtn;
            this.ClientSize = new System.Drawing.Size(1346, 1578);
            this.Controls.Add(this.flowLayoutPanel1);
            this.Controls.Add(this.flowLayoutPanel2);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AttributeForm";
            this.Text = "Select Rule Attribute";
            this.Load += new System.EventHandler(this.AttributeForm_Load);
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.flowLayoutPanel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button CancelBtn;
        private System.Windows.Forms.Button OkBtn;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListView ruleAttributeList;
        private System.Windows.Forms.ColumnHeader ruleAttributeColumn;
        private System.Windows.Forms.ColumnHeader attributeType;
        private System.Windows.Forms.ColumnHeader defaultValue;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
    }
}