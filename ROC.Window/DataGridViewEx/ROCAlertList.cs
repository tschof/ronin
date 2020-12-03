using System;
using System.Windows.Forms;
using System.ComponentModel;

using BindingListEx;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCAlertList : DGVEx
	{
		private ROCBindingList<AlertListData> _rocGridData;
		public ROCBindingList<AlertListData> ROCGridData
		{
			get
			{
				if (_rocGridData == null)
				{
					_rocGridData = new ROCBindingList<AlertListData>();
				}
				return _rocGridData;
			}
			set
			{
				_rocGridData = value;
			}
		}

		public void Initialize()
		{
			SetGrid(this);
		}

		private void SetGrid(ROCAlertList grid)
		{
			DGVTextColumn txtCol;

			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.AllowUserToAddRows = false;
			grid.AllowUserToDeleteRows = false;
			grid.AllowUserToOrderColumns = false;

			grid.VirtualMode = true;

			if (!grid.Columns.Contains("AlertTime"))
			{
				txtCol = new DGVTextColumn();
				txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
				txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
				txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
				txtCol.ReadOnly = true;
				txtCol.ValueType = typeof(DateTime);
				txtCol.DefaultCellStyle.Format = "dd-MM hh:mm:ss.ffff";
				txtCol.HeaderText = "Time";
				txtCol.Name = "AlertTime";
				txtCol.DataPropertyName = "AlertTime";
				grid.Columns.Add(txtCol);
			}

			if (!grid.Columns.Contains("Message"))
			{
				txtCol = new DGVTextColumn();
				txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
				txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
				txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
				txtCol.ReadOnly = true;
				txtCol.ValueType = typeof(string);
				txtCol.HeaderText = "Message";
				txtCol.Name = "Message";
				txtCol.DataPropertyName = "Message";
				grid.Columns.Add(txtCol);
			}

			grid.DataSource = ROCGridData;
		}
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
