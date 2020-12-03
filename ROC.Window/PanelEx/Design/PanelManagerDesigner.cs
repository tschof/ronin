using System;
using System.ComponentModel.Design;
using System.Collections;
using System.Windows.Forms;
using System.ComponentModel;

namespace PanelEx.Design
{
	public class PanelManagerDesigner : System.Windows.Forms.Design.ParentControlDesigner
	{
		private DesignerVerbCollection m_verbs = new DesignerVerbCollection();

		#region - Constructor -

		public PanelManagerDesigner()
			: base()
		{
			DesignerVerb verb1 = new DesignerVerb("Add MangedPanel", new EventHandler(OnAddPanel));
			DesignerVerb verb2 = new DesignerVerb("Remove ManagedPanel", new EventHandler(OnRemovePanel));
			m_verbs.AddRange(new DesignerVerb[] { verb1, verb2 });
		}

		#endregion

		#region - Local Method -

		private void OnAddPanel(Object sender, EventArgs e)
		{
			Control.ControlCollection oldManagedPanels = HostControl.Controls;

			RaiseComponentChanging(TypeDescriptor.GetProperties(HostControl)["ManagedPanels"]);

			ManagedPanel P = (ManagedPanel)DesignerHost.CreateComponent(typeof(ManagedPanel));
			P.Text = P.Name;
			HostControl.ManagedPanels.Add(P);

			RaiseComponentChanged(TypeDescriptor.GetProperties(HostControl)["ManagedPanels"], oldManagedPanels, HostControl.ManagedPanels);
			HostControl.SelectedPanel = P;

			SetVerbs();
		}

		private void OnRemovePanel(Object sender, EventArgs e)
		{
			Control.ControlCollection oldManagedPanels = HostControl.Controls;

			if (HostControl.SelectedIndex < 0) return;

			RaiseComponentChanging(TypeDescriptor.GetProperties(HostControl)["TabPages"]);

			DesignerHost.DestroyComponent((ManagedPanel)HostControl.ManagedPanels[HostControl.SelectedIndex]);

			RaiseComponentChanged(TypeDescriptor.GetProperties(HostControl)["ManagedPanels"], oldManagedPanels, HostControl.ManagedPanels);

			SelectionService.SetSelectedComponents(new IComponent[] { HostControl }, SelectionTypes.Auto);

			SetVerbs();
		}

		private void SetVerbs()
		{
			Verbs[1].Enabled = HostControl.ManagedPanels.Count == 1;
		}

		#endregion

		#region - Properties -

		private PanelManager HostControl
		{
			get { return (PanelManager)this.Control; }
		}

		#endregion

		#region - Interface -

		private IDesignerHost m_DesignerHost;
		public IDesignerHost DesignerHost
		{
			get
			{
				if (m_DesignerHost == null)
					m_DesignerHost = (IDesignerHost)GetService(typeof(IDesignerHost));
				return m_DesignerHost;
			}
		}

		private ISelectionService m_SelectionService;
		public ISelectionService SelectionService
		{
			get
			{
				if (m_SelectionService == null)
					m_SelectionService = (ISelectionService)GetService(typeof(ISelectionService));

				return m_SelectionService;
			}
		}

		#endregion

		#region - Control Override -

		public override DesignerVerbCollection Verbs
		{
			get
			{
				if (m_verbs.Count == 2)
					m_verbs[1].Enabled = HostControl.ManagedPanels.Count > 0;
				return m_verbs;
			}
		}

		public override void InitializeNewComponent(IDictionary defaultValues)
		{
			base.InitializeNewComponent(defaultValues);

			HostControl.ManagedPanels.Add((ManagedPanel)DesignerHost.CreateComponent(typeof(ManagedPanel)));
			HostControl.ManagedPanels.Add((ManagedPanel)DesignerHost.CreateComponent(typeof(ManagedPanel)));
			PanelManager pm = (PanelManager)this.Control;
			pm.ManagedPanels[0].Text = pm.ManagedPanels[0].Name;
			pm.ManagedPanels[1].Text = pm.ManagedPanels[1].Name;
			HostControl.SelectedIndex = 0;
		}

		protected override void OnPaintAdornments(PaintEventArgs pe)
		{
			// Don't want DrawGrid Dots.
		}

		protected override void PostFilterProperties(IDictionary properties)
		{
			properties.Remove("AutoScroll");
			properties.Remove("AutoScrollMargin");
			properties.Remove("AutoScrollMinSize");
			properties.Remove("Text");
			base.PostFilterProperties(properties);
		}

		#endregion
	}
}
