using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;


namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCUserProfileList : DGVTree
	{
		private Font _nodeFont;
		[Category("Appearance"), Description("Font to flag nodes with child")]
		public Font NodeFont
		{
			get
			{
				if (_nodeFont == null)
				{
					_nodeFont = DefaultCellStyle.Font;
				}
				return _nodeFont;
			}
		}

		public ROCUserProfileList()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer | ControlStyles.OptimizedDoubleBuffer, true);
		}

		public void Initialize()
		{
			SetGrid(this);
		}

		private void SetGrid(ROCUserProfileList grid)
		{
			DGVTreeColumn treCol;
			DGVTextColumn txtCol;

			grid.Rows.Clear();

			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			treCol = new DGVTreeColumn();
			treCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			treCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			treCol.SortMode = DataGridViewColumnSortMode.Automatic;
			treCol.ReadOnly = true;
			treCol.DefaultCellStyle.Format = "";
			treCol.HeaderText = "Header";
			treCol.Name = "Header";
			grid.Columns.Add(treCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Infomation";
			txtCol.Name = "Infomation";
			grid.Columns.Add(txtCol);

			_nodeFont = new Font(grid.DefaultCellStyle.Font, FontStyle.Bold);
		}
	}
}
