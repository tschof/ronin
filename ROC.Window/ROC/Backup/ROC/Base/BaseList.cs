using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FormEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class BaseList : VistaWindowBorder
	{
		// Keep a symbol detail list for IM to match option symbol to <SymbolDetail,Symbol>
		private Dictionary<string, string> _imSymbolNeeded;
		public Dictionary<string, string> ImSymbolNeeded
		{
			get
			{
				if (_imSymbolNeeded == null)
				{
					_imSymbolNeeded = new Dictionary<string, string>();
				}
				return _imSymbolNeeded;
			}
			set
			{
				_imSymbolNeeded = value;
			}
		}
	}
}