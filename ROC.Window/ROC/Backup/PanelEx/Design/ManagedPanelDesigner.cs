using System;
using System.ComponentModel.Design;
using System.Windows.Forms;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace PanelEx.Design
{
	public class ManagedPanelDesigner : System.Windows.Forms.Design.ScrollableControlDesigner
	{
		private DesignerVerbCollection m_verbs = new DesignerVerbCollection();

		#region - Constructor -

		public ManagedPanelDesigner()
			: base()
		{
			DesignerVerb verb1 = new DesignerVerb("Select PanelManager", new EventHandler(OnSelectManager));
			m_verbs.Add(verb1);
		}

		#endregion

		#region - Local Method -

		private void OnSelectManager(Object sender, EventArgs e)
		{
			if (this.HostControl.Parent != null)
				this.SelectionService.SetSelectedComponents(new Component[] { this.HostControl.Parent });
		}

		#endregion

		#region - Control Override -

		public override System.Windows.Forms.Design.SelectionRules SelectionRules
		{
			get
			{
				return System.Windows.Forms.Design.SelectionRules.Visible;
			}
		}

		protected override void OnPaintAdornments(PaintEventArgs pe)
		{
			base.OnPaintAdornments(pe);
			Color penColor;
			if (this.Control.BackColor.GetBrightness() >= 0.5)
				penColor = ControlPaint.Dark(this.Control.BackColor);
			else
				penColor = Color.White;

			Pen dashedPen = new Pen(penColor);
			Rectangle borderRectangle = this.Control.ClientRectangle;
			borderRectangle.Width -= 1;
			borderRectangle.Height -= 1;
			dashedPen.DashStyle = DashStyle.Dash;
			pe.Graphics.DrawRectangle(dashedPen, borderRectangle);
			dashedPen.Dispose();
		}

		public override DesignerVerbCollection Verbs
		{
			get
			{
				return m_verbs;
			}
		}

		protected override void PostFilterProperties(IDictionary properties)
		{
			properties.Remove("Anchor");
			properties.Remove("TabStop");
			properties.Remove("TabIndex");
			base.PostFilterProperties(properties);
		}

		public override void InitializeNewComponent(IDictionary defaultValues)
		{
			base.InitializeNewComponent(defaultValues);
			this.Control.Visible = true;
		}

		#endregion

		#region - Properties -

		private ManagedPanel HostControl
		{
			get
			{ return (ManagedPanel)this.Control; }
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
	}
}
