using System;
using System.Data;
using System.Collections.Generic;
using RDSEx;

namespace ROC
{
	public abstract class HelperDataBase
	{
		private DataTable _table;
		public DataTable Table
		{
			get
			{
				if (_table == null)
				{
					SetTable();
				}
				return _table;
			}
			set
			{
				_table = value;
			}
		}

		private DataView _shearchView;
		public DataView SearchView
		{
			get
			{
				return _shearchView;
			}
			set
			{
				_shearchView = value;
			}
		}

		public abstract void SetTable();

		private List<string> _keys;
		public List<string> Keys
		{
			get
			{
				if (_keys == null)
				{
					_keys = new List<string>();
				}
				return _keys;
			}
			set
			{
				_keys = value;
			}
		}
	}
}
