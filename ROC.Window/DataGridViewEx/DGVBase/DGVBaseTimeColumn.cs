using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;


namespace DataGridViewEx
{
	public class DGVBaseTimeColumn : DataGridViewColumn
	{
		private string[] _timeFormats = new string[0];
		public string[] TimeFormats
		{
			get
			{
				if (_timeFormats.Length == 0)
				{
					_timeFormats = new string[4];
					_timeFormats[0] = "HHmmss";
					_timeFormats[1] = "yyyyMMdd-HH:mm:ss";
					_timeFormats[2] = "yyyyMMdd";
					_timeFormats[3] = "yyyyMMdd-HH:mm:ss.fff";
				}
				return _timeFormats;
			}
		}

		private bool _toLocal = false;
		public bool ToLocal
		{
			get
			{
				return _toLocal;
			}
			set
			{
				_toLocal = value;
			}
		}

		private string _fromatString = "MM-dd HH:mm:ss";
		public string FormatString
		{
			get
			{
				return _fromatString;
			}
			set
			{
				_fromatString = value;
			}
		}

		public DGVBaseTimeColumn()
		{
			base.CellTemplate = new DGVBaseTimeCell();
		}
	}
}
