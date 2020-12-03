using System;
using System.Windows.Forms;
using System.ComponentModel;

using BindingListEx;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCAlertList : DataGridView
	{
		private BindingListWatch<AlertListData> _alertDatas;
		public BindingListWatch<AlertListData> AlertDatas
		{
			get
			{
				if (_alertDatas == null)
				{
					_alertDatas = new BindingListWatch<AlertListData>();
				}
				return _alertDatas;
			}
			set
			{
				_alertDatas = value;
			}
		}

		private AlertListData _inEditRowData;
		private int _inEditRowIndex = -1;
		private bool _rowScopeCommit = true;

		public ROCAlertList()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer | ControlStyles.OptimizedDoubleBuffer, true);
		}

		public void Initialize()
		{
			SetGrid(this);
		}

		private void SetGrid(ROCAlertList grid)
		{
			DGVTextColumn col;

			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.VirtualMode = true;

			if (!grid.Columns.Contains("AlertTime"))
			{
				col = new DGVTextColumn();
				col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
				col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
				col.SortMode = DataGridViewColumnSortMode.Automatic;
				col.ReadOnly = true;
				col.ValueType = typeof(DateTime);
				col.DefaultCellStyle.Format = "dd-MM hh:mm:ss.ffff";
				col.HeaderText = "Time";
				col.Name = "AlertTime";
				col.DataPropertyName = "AlertTime";
				grid.Columns.Add(col);
			}

			if (!grid.Columns.Contains("Message"))
			{
				col = new DGVTextColumn();
				col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
				col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
				col.SortMode = DataGridViewColumnSortMode.Automatic;
				col.ReadOnly = true;
				col.ValueType = typeof(string);
				col.HeaderText = "Message";
				col.Name = "Message";
				col.DataPropertyName = "AlertMsg";
				grid.Columns.Add(col);
			}

			grid.DataSource = AlertDatas;
		}

		public void ReorderColumn()
		{
			ReorderColumn(this, "AlertTime", 0);
			ReorderColumn(this, "Message", 1);
		}
		public void ReorderColumn(string colName, int displayIndex)
		{
			ReorderColumn(this, colName, displayIndex);
		}
		private void ReorderColumn(ROCAlertList grid, string colName, int displayIndex)
		{
			grid.Columns[colName].DisplayIndex = displayIndex;
		}

		protected override void OnCellValueNeeded(DataGridViewCellValueEventArgs e)
		{
			// If this is the row for new records, no values are needed.
			if (e.RowIndex == RowCount - 1)
			{
				return;
			}

			AlertListData alertData;

			// Store a reference to the Customer object for the row being painted.
			if (e.RowIndex == _inEditRowIndex)
			{
				alertData = _inEditRowData;
			}
			else
			{
				alertData = AlertDatas[e.RowIndex];
			}

			// Set the cell value to paint using the Customer object retrieved.
			switch (Columns[e.ColumnIndex].Name)
			{
				case "AlertTime":
					e.Value = alertData.AlertTime;
					break;
				case "Message":
					e.Value = alertData.Message;
					break;
			}

			base.OnCellValueNeeded(e);
		}

		protected override void OnCellValuePushed(DataGridViewCellValueEventArgs e)
		{
			AlertListData alertData;

			// Store a reference to the Customer object for the row being edited.
			if (e.RowIndex <= AlertDatas.Count)
			{
				// If the user is editing a new row, create a new Customer object.
				if (_inEditRowData == null)
				{
					_inEditRowData = new AlertListData(
						AlertDatas[e.RowIndex].AlertTime,
						AlertDatas[e.RowIndex].Message);
				}
				alertData = _inEditRowData;
				_inEditRowIndex = e.RowIndex;
			}
			else
			{
				alertData = _inEditRowData;
			}

			// Set the appropriate Customer property to the cell value entered.
			switch (Columns[e.ColumnIndex].Name)
			{
				case "AlertTime":
					if (e.Value.GetType() == Type.GetType("System.DateTime"))
					{
						alertData.AlertTime = (DateTime)e.Value;
					}
					break;
				case "Message":
					alertData.Message = e.Value as string;
					break;
			}

			base.OnCellValuePushed(e);
		}

		protected override void OnNewRowNeeded(DataGridViewRowEventArgs e)
		{
			// Create a new Customer object when the user edits
			// the row for new records.
			_inEditRowData = new AlertListData();
			_inEditRowIndex = Rows.Count - 1;
			base.OnNewRowNeeded(e);
		}

		protected override void OnRowValidated(DataGridViewCellEventArgs e)
		{
			// Save row changes if any were made and release the edited 
			// Customer object if there is one.
			if (e.RowIndex >= AlertDatas.Count && e.RowIndex != Rows.Count - 1)
			{
				// Add the new Customer object to the data store.
				AlertDatas.Add(_inEditRowData);
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}
			else if (_inEditRowData != null && e.RowIndex < AlertDatas.Count)
			{
				// Save the modified Customer object in the data store.
				AlertDatas[e.RowIndex] = _inEditRowData;
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}
			else if (ContainsFocus)
			{
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}

			base.OnRowValidated(e);
		}

		protected override void OnRowDirtyStateNeeded(QuestionEventArgs e)
		{
			if (!_rowScopeCommit)
			{
				// In cell-level commit scope, indicate whether the value
				// of the current cell has been modified.
				e.Response = IsCurrentCellDirty;
			}

			base.OnRowDirtyStateNeeded(e);
		}

		protected override void OnCancelRowEdit(QuestionEventArgs e)
		{
			if (_inEditRowIndex == Rows.Count - 2 && _inEditRowIndex == AlertDatas.Count)
			{
				// If the user has canceled the edit of a newly created row, 
				// replace the corresponding Customer object with a new, empty one.
				_inEditRowData = new AlertListData();
			}
			else
			{
				// If the user has canceled the edit of an existing row, 
				// release the corresponding Customer object.
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}

			base.OnCancelRowEdit(e);
		}

		protected override void OnUserDeletedRow(DataGridViewRowEventArgs e)
		{
			if (e.Row.Index < AlertDatas.Count)
			{
				// If the user has deleted an existing row, remove the 
				// corresponding Customer object from the data store.
				AlertDatas.RemoveAt(e.Row.Index);
			}

			if (e.Row.Index == _inEditRowIndex)
			{
				// If the user has deleted a newly created row, release
				// the corresponding Customer object. 
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}

			base.OnUserDeletedRow(e);
		}

		protected override void OnDataError(bool displayErrorDialogIfNoHandler, DataGridViewDataErrorEventArgs e)
		{
			base.OnDataError(false, e);
		}

		public class AlertListData : INotifyPropertyChanged
		{
			#region - INotifyPropertyChanged Members -

			public event PropertyChangedEventHandler PropertyChanged;

			protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
			{
				if (PropertyChanged != null)
				{
					PropertyChanged(this, e);
				}
			}

			#endregion

			public AlertListData()
			{
			}

			public AlertListData(DateTime alertTime, string message)
			{
				_alertTime = alertTime;
				_message = message;
			}

			private DateTime _alertTime;
			public DateTime AlertTime
			{
				get
				{
					return _alertTime;
				}
				set
				{
					if (_alertTime != value)
					{
						_alertTime = value;
						onPropertyChanged(new PropertyChangedEventArgs("AlertTime"));
					}
				}
			}

			private string _message;
			public string Message
			{
				get
				{
					return _message;
				}
				set
				{
					if (_message != value)
					{
						_message = value;
						onPropertyChanged(new PropertyChangedEventArgs("Message"));
					}
				}
			}
		}
	}
}
