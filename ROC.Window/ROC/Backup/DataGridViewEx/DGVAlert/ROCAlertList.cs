using System;
using System.Windows.Forms;
using System.ComponentModel;

using BindingListEx;
using System.Drawing;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCAlertList : DGVBase
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
			DGVAlertTextColumn txtCol;
			DGVAlertTimeColumn timCol;

			grid.BackgroundColor = Color.Black;
			grid.GridColor = Color.Black;

			grid.ShowCellToolTips = false;
			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = true;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.AllowUserToAddRows = false;
			grid.AllowUserToDeleteRows = false;

			grid.AllowUserToOrderColumns = false;

			grid.VirtualMode = true;

			timCol = new DGVAlertTimeColumn();
			timCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			timCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			timCol.DefaultCellStyle.BackColor = Color.Black;
			timCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			timCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			timCol.SortMode = DataGridViewColumnSortMode.Automatic;
			timCol.ReadOnly = true;
			timCol.ValueType = typeof(DateTime);
			timCol.DefaultCellStyle.Format = "";
			timCol.HeaderText = "Time";
			timCol.Name = "Time";
			timCol.DataPropertyName = "AlertTime";
			grid.Columns.Add(timCol);

			txtCol = new DGVAlertTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Message";
			txtCol.Name = "Message";
			txtCol.DataPropertyName = "Message";
			txtCol.MinimumWidth = 200;
			grid.Columns.Add(txtCol);

			grid.RowsDefaultCellStyle.WrapMode = DataGridViewTriState.True;
			grid.RowTemplate.MinimumHeight = grid.RowTemplate.Height * 2;

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
