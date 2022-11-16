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
        public int SelectedIndex { get; set; }
        public Point InitialLocation;

        public AttributeForm(List<string> attributeNames, Point loc)
        {
            InitializeComponent();
            AttributeComboBox.Items.AddRange(attributeNames.ToArray());
            InitialLocation = loc;
        }

        private void AttributeForm_Load(object sender, EventArgs e)
        {
            SetDesktopLocation(InitialLocation.X, InitialLocation.Y);
            AttributeComboBox.SelectedIndex = 0;
        }

        private void Ok_Click(object sender, EventArgs e)
        {
            SelectedIndex = AttributeComboBox.SelectedIndex;
            DialogResult = DialogResult.OK;
            Close();
        }

        private void Cancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void AttributeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
