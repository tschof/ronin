using System;
using System.Windows.Forms;
using System.Drawing;
using System.Windows.Forms.VisualStyles;
using System.Diagnostics;

namespace DataGridViewEx
{
	public class DGVTreeColumn : DataGridViewTextBoxColumn
	{
		internal Image _defaultNodeImage;

		public DGVTreeColumn()
		{
			this.CellTemplate = new DGVTreeCell();
		}

		// Need to override Clone for design-time support.
		public override object Clone()
		{
			DGVTreeColumn c = (DGVTreeColumn)base.Clone();
			c._defaultNodeImage = this._defaultNodeImage;
			return c;
		}

		public Image DefaultNodeImage
		{
			get { return _defaultNodeImage; }
			set { _defaultNodeImage = value; }
		}
	}
}
