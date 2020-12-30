using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;

namespace PanelEx
{
	[DefaultProperty("SelectedPanel")]
	[DefaultEvent("SelectedIndexChanged")]
	[Designer(typeof(PanelEx.Design.PanelManagerDesigner))]
	[System.ComponentModel.DesignerCategory("code")]
	public partial class PanelManager : Control
	{
		#region - Event Header -

		public event EventHandler SelectedIndexChanged;

		#endregion

		#region - Constructor -

		public PanelManager()
		{
			TabStop = false;
		}

		#endregion

		#region - Control Override -

		private ManagedPanel oldSelection = null;
		protected void OnSelectedPanelChanged(EventArgs e)
		{
			if (oldSelection != null)
				oldSelection.Visible = false;

			if (m_SelectedPanel != null)
				((ManagedPanel)m_SelectedPanel).Visible = true;

			bool tabChanged = false;
			if (m_SelectedPanel == null)
				tabChanged = (oldSelection != null);
			else
				tabChanged = (!m_SelectedPanel.Equals(oldSelection));

			if (tabChanged && this.Created)
				if (SelectedIndexChanged != null)
					SelectedIndexChanged(this, EventArgs.Empty);

			oldSelection = (ManagedPanel)m_SelectedPanel;

		}

		protected override void OnControlAdded(ControlEventArgs e)
		{
			if ((e.Control is ManagedPanel) == false)
				throw new ArgumentException("Only Mangel.Controls.ManagedPanels can be added to a Mangel.Controls.PanelManger.");

			if (this.SelectedPanel != null)
				((ManagedPanel)this.SelectedPanel).Visible = false;

			this.SelectedPanel = (ManagedPanel)e.Control;
			e.Control.Visible = true;
			base.OnControlAdded(e);
		}

		protected override void OnControlRemoved(ControlEventArgs e)
		{
			if (e.Control is ManagedPanel)
			{
				if (this.ManagedPanels.Count > 0)
					this.SelectedIndex = 0;
				else
					this.SelectedPanel = null;
			}
			base.OnControlRemoved(e);
		}

		#endregion

		#region - Properties -
		//ManagedPanels
		[Editor(typeof(Editor.ManagedPanelCollectionEditor), typeof(UITypeEditor))]
		public ControlCollection ManagedPanels
		{
			get { return base.Controls; }
		}

		//SelectedPanel
		[TypeConverter(typeof(TypeConverter.SelectedPanelConverter))]
		private ManagedPanel m_SelectedPanel;
		public ManagedPanel SelectedPanel
		{
			get { return m_SelectedPanel; }
			set
			{
				if (m_SelectedPanel == value) return;
				m_SelectedPanel = value;
				OnSelectedPanelChanged(EventArgs.Empty);
			}
		}

		//SelectedIndex
		[Browsable(false)]
		public int SelectedIndex
		{
			get { return this.ManagedPanels.IndexOf((ManagedPanel)this.SelectedPanel); }
			set
			{
				if (value == -1)
					this.SelectedPanel = null;
				else
					this.SelectedPanel = (ManagedPanel)this.ManagedPanels[value];
			}
		}

		//DefaultSize
		protected override Size DefaultSize
		{
			get
			{
				return new Size(200, 100);
			}
		}

		#endregion
	}
}
