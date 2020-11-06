using GH_IO.Serialization;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Parameters;
using Grasshopper.Kernel.Types;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GrasshopperPRT
{
    public class ParamBoolCustom : Param_Boolean
    {
        public override string Name { get => base.Name; set => base.Name = value; }
        public override string NickName { get => base.NickName; set => base.NickName = value; }
        public override string Description { get => base.Description; set => base.Description = value; }

        public override IEnumerable<string> Keywords => base.Keywords;

        public override string Category { get => base.Category; set => base.Category = value; }
        public override string SubCategory { get => base.SubCategory; set => base.SubCategory = value; }

        public override bool Obsolete => base.Obsolete;

        public override bool Locked { get => base.Locked; set => base.Locked = value; }

        public override GH_RuntimeMessageLevel RuntimeMessageLevel => base.RuntimeMessageLevel;

        public override bool IconCapableUI => base.IconCapableUI;

        public override IList<IGH_Param> Sources => base.Sources;

        public override int SourceCount => base.SourceCount;

        public override IList<IGH_Param> Recipients => base.Recipients;

        public override bool HasProxySources => base.HasProxySources;

        public override int ProxySourceCount => base.ProxySourceCount;

        public override Type Type => base.Type;

        public override GH_ParamKind Kind => base.Kind;

        public override GH_ParamAccess Access { get => base.Access; set => base.Access = value; }

        public override string InstanceDescription => base.InstanceDescription;

        public override TimeSpan ProcessorTime => base.ProcessorTime;

        public override string TypeName => base.TypeName;

        public override GH_StateTagList StateTags => base.StateTags;

        public override GH_Exposure Exposure => base.Exposure;

        public override Guid ComponentGuid => base.ComponentGuid;

        protected override Bitmap Internal_Icon_24x24 => base.Internal_Icon_24x24;

        protected override Bitmap Icon => base.Icon;

        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);
        }

        public override void AddRuntimeMessage(GH_RuntimeMessageLevel level, string text)
        {
            base.AddRuntimeMessage(level, text);
        }

        public override void AddSource(IGH_Param source)
        {
            base.AddSource(source);
        }

        public override void AddSource(IGH_Param source, int index)
        {
            base.AddSource(source, index);
        }

        public override void AppendAdditionalMenuItems(ToolStripDropDown menu)
        {
            base.AppendAdditionalMenuItems(menu);

        }

        public override bool AppendMenuItems(ToolStripDropDown menu)
        {
            return base.AppendMenuItems(menu);
        }

        public override void ClearData()
        {
            base.ClearData();
        }

        public override void ClearProxySources()
        {
            base.ClearProxySources();
        }

        public override void ClearRuntimeMessages()
        {
            base.ClearRuntimeMessages();
        }

        public override void CreateAttributes()
        {
            base.CreateAttributes();
        }

        public override void CreateProxySources()
        {
            base.CreateProxySources();
        }

        public override bool DependsOn(IGH_ActiveObject potential_source)
        {
            return base.DependsOn(potential_source);
        }

        public override void DocumentContextChanged(GH_Document document, GH_DocumentContext context)
        {
            base.DocumentContextChanged(document, context);
        }

        public override bool Equals(object obj)
        {
            return base.Equals(obj);
        }

        public override void ExpirePreview(bool redraw)
        {
            base.ExpirePreview(redraw);
        }

        public override void ExpireSolution(bool recompute)
        {
            base.ExpireSolution(recompute);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public override void IsolateObject()
        {
            base.IsolateObject();
        }

        public override void MovedBetweenDocuments(GH_Document oldDocument, GH_Document newDocument)
        {
            base.MovedBetweenDocuments(oldDocument, newDocument);
        }

        public override void PostProcessData()
        {
            base.PostProcessData();
        }

        public override bool Read(GH_IReader reader)
        {
            return base.Read(reader);
        }

        public override bool ReadFull(GH_IReader reader)
        {
            return base.ReadFull(reader);
        }

        public override void RegisterRemoteIDs(GH_GuidTable id_list)
        {
            base.RegisterRemoteIDs(id_list);
        }

        public override bool RelinkProxySources(GH_Document document)
        {
            return base.RelinkProxySources(document);
        }

        public override void RemoveAllSources()
        {
            base.RemoveAllSources();
        }

        public override void RemovedFromDocument(GH_Document document)
        {
            base.RemovedFromDocument(document);
        }

        public override void RemoveEffects()
        {
            base.RemoveEffects();
        }

        public override void RemoveSource(IGH_Param source)
        {
            base.RemoveSource(source);
        }

        public override void RemoveSource(Guid source_id)
        {
            base.RemoveSource(source_id);
        }

        public override void ReplaceSource(IGH_Param old_source, IGH_Param new_source)
        {
            base.ReplaceSource(old_source, new_source);
        }

        public override void ReplaceSource(Guid old_source_id, IGH_Param new_source)
        {
            base.ReplaceSource(old_source_id, new_source);
        }

        public override IList<string> RuntimeMessages(GH_RuntimeMessageLevel level)
        {
            return base.RuntimeMessages(level);
        }

        public override bool SDKCompliancy(int exeVersion, int exeServiceRelease)
        {
            return base.SDKCompliancy(exeVersion, exeServiceRelease);
        }

        public override string ToString()
        {
            return base.ToString();
        }

        public override bool Write(GH_IWriter writer)
        {
            return base.Write(writer);
        }

        public override bool WriteFull(GH_IWriter writer)
        {
            return base.WriteFull(writer);
        }

        protected override void CollectVolatileData_FromSources()
        {
            base.CollectVolatileData_FromSources();
        }

        protected override void ConversionCallback(object source, GH_Boolean target)
        {
            base.ConversionCallback(source, target);
        }

        protected override void ExpireDownStreamObjects()
        {
            base.ExpireDownStreamObjects();
        }

        protected override string Format(GH_Boolean Data)
        {
            return base.Format(Data);
        }

        protected override string HtmlHelp_Source()
        {
            return base.HtmlHelp_Source();
        }

        protected override GH_Boolean InstantiateT()
        {
            return base.InstantiateT();
        }

        protected override void Menu_AppendDestroyPersistent(ToolStripDropDown menu)
        {
            base.Menu_AppendDestroyPersistent(menu);
        }

        protected override void Menu_AppendExtractParameter(ToolStripDropDown menu)
        {
            base.Menu_AppendExtractParameter(menu);

            var item = menu.Items.Find("Extract parameter", true);
        }

        protected override void Menu_AppendInternaliseData(ToolStripDropDown menu)
        {
            base.Menu_AppendInternaliseData(menu);
        }

        protected override void Menu_AppendManageCollection(ToolStripDropDown menu)
        {
            base.Menu_AppendManageCollection(menu);
        }

        protected override void Menu_AppendPromptMore(ToolStripDropDown menu)
        {
            base.Menu_AppendPromptMore(menu);
        }

        protected override void Menu_AppendPromptOne(ToolStripDropDown menu)
        {
            base.Menu_AppendPromptOne(menu);
        }

        protected override ToolStripMenuItem Menu_CustomMultiValueItem()
        {
            return base.Menu_CustomMultiValueItem();
        }

        protected override ToolStripMenuItem Menu_CustomSingleValueItem()
        {
            return base.Menu_CustomSingleValueItem();
        }

        protected override void OnVolatileDataCollected()
        {
            base.OnVolatileDataCollected();
        }

        protected override GH_Boolean PreferredCast(object data)
        {
            return base.PreferredCast(data);
        }

        protected override void PrepareForPrompt()
        {
            base.PrepareForPrompt();
        }

        protected override bool Prompt_ManageCollection(GH_Structure<GH_Boolean> values)
        {
            return base.Prompt_ManageCollection(values);
        }

        protected override GH_GetterResult Prompt_Plural(ref List<GH_Boolean> value)
        {
            return base.Prompt_Plural(ref value);
        }

        protected override GH_GetterResult Prompt_Singular(ref GH_Boolean value)
        {
            return base.Prompt_Singular(ref value);
        }

        protected override void RecoverFromPrompt()
        {
            base.RecoverFromPrompt();
        }

        protected override void ValuesChanged()
        {
            base.ValuesChanged();
        }

        protected override string VolatileDataDescription()
        {
            return base.VolatileDataDescription();
        }
    }
}
