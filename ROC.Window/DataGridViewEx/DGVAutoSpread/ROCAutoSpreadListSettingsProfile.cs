using System;
using System.Windows.Forms;
using System.Collections.Generic;

using Common;

namespace DataGridViewEx
{
	[Serializable]
	public sealed class AutoSpreadSymbolSetting
	{
		private string _symbolDetail = "";
		public string SymbolDetail
		{
			get
			{
				return _symbolDetail;
			}
		}

		private long _factor = 1;
		public long Factor
		{
			get
			{
				return _factor;
			}
		}

		private double _userRatio = 0;
		public double UserRatio
		{
			get
			{
				return _userRatio;
			}
		}

		private bool _lean = false;
		public bool Lean
		{
			get
			{
				return _lean;
			}
		}

		private long _tickLimit = 5;
		public long TickLimit
		{
			get
			{
				return _tickLimit;
			}
		}

		private string _exchange = "";
		public string Exchange
		{
			get
			{
				return _exchange;
			}
		}

		private string _localAccountAcrn = "";
		public string LocalAccountAcrn
		{
			get
			{
				return _localAccountAcrn;
			}
		}

		private string _traderFor = "";
		public string TraderFor
		{
			get
			{
				return _traderFor;
			}
		}

		private string _shortLender = "";
		public string ShortLender
		{
			get
			{
				return _shortLender;
			}
		}

		public AutoSpreadSymbolSetting(string symbolDetail, long factor, double userRatio, bool lean, long tickLimit, string exchange, string localAccountAcrn, string traderFor, string shortLender)
		{
			_symbolDetail = symbolDetail;
			_factor = factor;
			_userRatio = userRatio;
			_lean = lean;
			_tickLimit = tickLimit;
			_exchange = exchange;
			_localAccountAcrn = localAccountAcrn;
			_traderFor = traderFor;
			_shortLender = shortLender;
		}
	}

	[Serializable]
	public class ROCAutoSpreadListSettingsProfile : DGVBaseProfile
	{
		public ROCAutoSpreadListSettingsProfile()
		{
		}

		public ROCAutoSpreadListSettingsProfile(ROCAutoSpreadListSettings grid)
		{
			Export(grid);
		}

		public ROCAutoSpreadListSettingsProfile(ROCAutoSpreadListSettings grid, ROCAutoSpreadListSettingsProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCAutoSpreadListSettings grid)
		{
			Update(DGVProfileFieldID.Grid.ForeColor, grid.DefaultCellForeColor);
			Update(DGVProfileFieldID.Grid.SelectedBackColor, grid.DefaultSelectedBackgroundColor);
			Update(DGVProfileFieldID.Grid.BackColor, grid.BackgroundColor);
			Update(DGVProfileFieldID.Grid.LineColor, grid.GridColor);
			Update(DGVProfileFieldID.Grid.Font, grid.Font);

			#region - SymbolDetail List -

			for (int i = 0; i < grid.Rows.Count - 1; i++)
			{
				if (grid.Rows[i].Cells["SymbolDetail"].Value != null &&
					grid.Rows[i].Cells["Factor"].Value != null)
				{
					AutoSpreadSymbolSettingList.Add(new AutoSpreadSymbolSetting(
						grid.Rows[i].Cells["SymbolDetail"].Value.ToString().ToUpper().Trim(),
						Convert.ToInt64(grid.Rows[i].Cells["Factor"].Value),
						Convert.ToDouble(grid.Rows[i].Cells["UserRatio"].Value),
						Convert.ToBoolean(((DataGridViewCheckBoxCell)grid.Rows[i].Cells["Lean"]).Value),
						Convert.ToInt64(grid.Rows[i].Cells["TickLimit"].Value),
						grid.Rows[i].Cells["Exchange"].Value.ToString().ToUpper().Trim(),
						grid.Rows[i].Cells["LocalAccountAcrn"].Value.ToString().ToUpper().Trim(),
						grid.Rows[i].Cells["TraderFor"].Value.ToString().ToUpper().Trim(),
						grid.Rows[i].Cells["ShortLender"].Value.ToString().ToUpper().Trim()));
				}
			}

			#endregion

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					case "Symbol":
					case "SymbolDetail":
					case "Factor":
					case "ContractSize":
					case "TickSize":
					case "Ratio":
					case "UserRatio":
					case "Name":
					case "Lean":
					case "TickLimit":
					case "TopAskBookPrice":
					case "TopAskBookPriceRowIndex":
					case "TopBidBookPrice":
					case "TopBidBookPriceRowIndex":
					case "DisplayFactor":
					case "SecType":
					case "Valid":
					case "Exchange":
					case "LocalAccountAcrn":
					case "TraderFor":
					case "ShortLender":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		public void Import(ROCAutoSpreadListSettings grid, ROCAutoSpreadListSettingsProfile prof)
		{
			lock (prof)
			{
				grid.DefaultCellForeColor = prof.GridForeColor;
				grid.DefaultSelectedBackgroundColor = prof.GridSelectedBackColor;
				grid.BackgroundColor = prof.GridBackColor;
				grid.GridColor = prof.GridColor;
				grid.Font = prof.GridFont;

				ProfileColumns = prof.ProfileColumns;
			}

			#region - Profile Columns -

			foreach ((string _, DGVColumnProfile val) in ProfileColumns)
			{
				switch (val.ColumnName)
				{
					case "Symbol":
					case "SymbolDetail":
					case "Factor":
					case "ContractSize":
					case "TickSize":
					case "Ratio":
					case "UserRatio":
					case "Name":
					case "Lean":
					case "TickLimit":
					case "TopAskBookPrice":
					case "TopAskBookPriceRowIndex":
					case "TopBidBookPrice":
					case "TopBidBookPriceRowIndex":
					case "DisplayFactor":
					case "SecType":
					case "Valid":
					case "Exchange":
					case "LocalAccountAcrn":
					case "TraderFor":
					case "ShortLender":
					default:
						SetProfile(grid.Columns[val.ColumnName], val);
						break;
				}
			}

			// TODO MSFT Behavior, when display index is duplicated, it defaults to index
			// http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=115233
			// Need to set displayindex twice to really set it.
			foreach ((string _, DGVColumnProfile val) in ProfileColumns)
			{
				if (grid.Columns.Contains(val.ColumnName))
				{
					grid.Columns[val.ColumnName].DisplayIndex = val.ColumnDisplayIndex;
				}
			}

			grid.LastSort = grid.RocGridTable.DefaultView.Sort;
			grid.LastFilter = grid.RocGridTable.DefaultView.RowFilter;
			grid.Invalidate();

			#endregion
		}
	}
}
