using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using System.Collections.Generic;

namespace DataGridViewEx
{
	public class DGVBaseDestinationColumn : DataGridViewColumn
	{
		private Dictionary<long, string> _destIDToName;
		public Dictionary<long, string> DestIDToName
		{
			get
			{
				if (_destIDToName == null)
				{
					_destIDToName = new Dictionary<long, string>();
				}
				return _destIDToName;
			}
			set
			{
				_destIDToName = value;
			}
		}

		public DGVBaseDestinationColumn()
		{
			base.CellTemplate = new DGVBaseDestinationCell();
		}
	}
}
