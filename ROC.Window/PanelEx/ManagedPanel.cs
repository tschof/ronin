using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace PanelEx
{
	[Designer(typeof(Design.ManagedPanelDesigner))]
	[ToolboxItem(false)]
	[System.ComponentModel.DesignerCategory("code")]
	public class ManagedPanel : ScrollableControl
	{
		#region - Constructor -

		public ManagedPanel()
		{
			base.Dock = DockStyle.Fill;
			SetStyle(
				ControlStyles.AllPaintingInWmPaint |
				ControlStyles.CacheText |
				ControlStyles.OptimizedDoubleBuffer |
				ControlStyles.ResizeRedraw |
				ControlStyles.UserPaint, true);
			base.TabIndex = 0;
			base.TabStop = false;
		}

		#endregion

		#region - Control Override -

		protected override void OnLocationChanged(EventArgs e)
		{
			base.OnLocationChanged(e);
			base.Location = Point.Empty;
		}

		protected override void OnSizeChanged(EventArgs e)
		{
			base.OnSizeChanged(e);
			if (this.Parent == null)
				this.Size = Size.Empty;
			else
				this.Size = this.Parent.ClientSize;
		}

		protected override void OnParentChanged(EventArgs e)
		{
			if ((this.Parent is PanelManager) == false && this.Parent != null)
				throw new ArgumentException("Managed Panels may only be added to a Panel Manager.");
			base.OnParentChanged(e);
		}

		#endregion

		#region - Properties -

		[Browsable(false), EditorBrowsable(EditorBrowsableState.Never)]
		[DefaultValue(typeof(DockStyle), "Fill")]
		public override DockStyle Dock
		{
			get
			{
				return base.Dock;
			}
			set
			{
				base.Dock = DockStyle.Fill;
			}
		}


		[Browsable(false), EditorBrowsable(EditorBrowsableState.Never)]
		[DefaultValue(typeof(AnchorStyles), "None")]
		public override AnchorStyles Anchor
		{
			get
			{
				return AnchorStyles.None;
			}
			set
			{
				base.Anchor = AnchorStyles.None;
			}
		}

		[Browsable(false), EditorBrowsable(EditorBrowsableState.Never)]
		[DefaultValue(typeof(int), "0")]
		public new int TabIndex
		{
			get
			{
				return this.TabIndex;
			}
			set
			{
				this.TabIndex = 0;
			}
		}

		[Browsable(false), EditorBrowsable(EditorBrowsableState.Never)]
		[DefaultValue(typeof(bool), "False")]
		public new bool TabStop
		{
			get
			{
				return this.TabStop;
			}
			set
			{
				this.TabStop = false;
			}
		}

		#endregion
	}
}
