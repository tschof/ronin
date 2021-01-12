using System.Collections.Generic;
using System.Data;

namespace ROC
{
	public abstract class HelperDataBase
	{
		private DataTable _table = null;
		public DataTable Table
		{
			get
			{
				if (_table == null)
				{
					createDataTable();
					OnCreateTable(_table);
				}
				return _table;
			}
		}

		private List<string> _keys = null;
		protected bool TryAddKey(string key)
		{
			if (_keys == null)
				_keys = new List<string>();
			if (_keys.Contains(key))
				return false;
			_keys.Add(key);
			return true;
		}
		protected void ClearKeys()
		{
			if (_keys != null)
				_keys.Clear();
		}

		public DataView SearchView { get; private set; } = null;

		protected abstract void OnCreateTable(DataTable table);
		protected abstract string GetTableTitle();
		protected abstract (string, System.Type)[] GetTableColumns();
		protected abstract string GetKeyColumnName();

		protected static DataTable BuildDataTable(string tableTitle, (string, System.Type)[] columns, string keyColumnName)
		{
			DataTable table = (tableTitle == null) ? new DataTable() : new DataTable(tableTitle);
			foreach ((string, System.Type) col in columns)
				table.Columns.Add(new DataColumn(col.Item1, col.Item2));

			if (!string.IsNullOrEmpty(keyColumnName)){
				table.PrimaryKey = new DataColumn[] { table.Columns[keyColumnName] };
			}
			return table;
		}

		protected static DataTable CreateSimpleTable(string columnName, System.Type columnType, params string[] values)
		{
			DataTable table = new DataTable();
			table.Columns.Add(new DataColumn(columnName, columnType));
			foreach (string value in values)
				table.Rows.Add(table.NewRow()[0], value);
			return table;
		}

		protected static DataTable CopySimpleTable(DataTable copyFrom, bool wantRowCopy, params string[] values)
		{
			DataTable table = wantRowCopy ? copyFrom.Copy() : copyFrom.Clone();
			foreach (string value in values)
				table.Rows.Add(table.NewRow()[0], value);
			return table;
		}

		private void createDataTable()
		{
			string keyColumnName = GetKeyColumnName();
			_table = BuildDataTable(GetTableTitle(), GetTableColumns(), keyColumnName);
			SearchView = new DataView(Table) { Sort = keyColumnName };
		}
	}
}
