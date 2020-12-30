using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

using FormEx;
using DataGridViewEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmPositionsDisplay : VistaWindowBorderless, INotifyPropertyChanged
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

		public frmPositionsDisplay(ROCPositionProfile prof)
		{
			InitializeComponent();

			itemColorSelectedBackground.ItemColor = prof.GridSelectedBackColor;
			itemColorBackground.ItemColor = prof.GridBackColor;
			itemColorGridlines.ItemColor = prof.GridColor;
			itemColorText.ItemColor = prof.GridForeColor;

			itemColumnPositionKey.ItemColor = prof.ProfileColumns["PositionKey"].ColumnForeColor;
			itemColumnMDSymbol.ItemColor = prof.ProfileColumns["Symbol"].ColumnForeColor;
			itemColumnSecType.ItemColor = prof.ProfileColumns["SecType"].ColumnForeColor;

			itemColumnSymbol.ItemColor = prof.ProfileColumns["SymbolDetail"].ColumnForeColor;
			itemColumnDisplay.ItemColor = prof.ProfileColumns["SymbolDisplay"].ColumnForeColor;
			itemColumnBidSize.ItemColor = prof.ProfileColumns["BidSize"].ColumnForeColor;
			itemColumnAskSize.ItemColor = prof.ProfileColumns["AskSize"].ColumnForeColor;
			itemColumnVolumn.ItemColor = prof.ProfileColumns["Volume"].ColumnForeColor;
			itemColumnAccount.ItemColor = prof.ProfileColumns["ClearingAcct"].ColumnForeColor;
			itemColumnTrader.ItemColor = prof.ProfileColumns["Trader"].ColumnForeColor;
			itemColumnOpenAvg.ItemColor = prof.ProfileColumns["OpenAvg"].ColumnForeColor;
			itemColumnBuyQty.ItemColor = prof.ProfileColumns["BuyQty"].ColumnForeColor;
			itemColumnBuyAvg.ItemColor = prof.ProfileColumns["BuyAvg"].ColumnForeColor;
			itemColumnSellQty.ItemColor = prof.ProfileColumns["SellQty"].ColumnForeColor;
			itemColumnSellAvg.ItemColor = prof.ProfileColumns["SellAvg"].ColumnForeColor;
			
			// Default Color For Animated and Stated Columns
			itemColumnBidPrice.ItemColor = prof.ProfileColumns["BidPrice"].DefaultColor;
			itemColumnAskPrice.ItemColor = prof.ProfileColumns["AskPrice"].DefaultColor;
			itemColumnLastTraded.ItemColor = prof.ProfileColumns["LastTraded"].DefaultColor;
			itemColumnNetChange.ItemColor = prof.ProfileColumns["NetChange"].DefaultColor;
			itemColumnPctChange.ItemColor = prof.ProfileColumns["PctChange"].DefaultColor;
			itemColumnOpenQty.ItemColor = prof.ProfileColumns["OpenQty"].DefaultColor;
			itemColumnCurrentQty.ItemColor = prof.ProfileColumns["CurrentQty"].DefaultColor;
			itemColumnOpenPnL.ItemColor = prof.ProfileColumns["OpenPnL"].DefaultColor;
			itemColumnDayPnL.ItemColor = prof.ProfileColumns["DayPnL"].DefaultColor;
			itemColumnTotalPnL.ItemColor = prof.ProfileColumns["TotalPnL"].DefaultColor;
			itemColumnMarketValue.ItemColor = prof.ProfileColumns["MarketValue"].DefaultColor;
			itemColumnRealizedPnL.ItemColor = prof.ProfileColumns["DayRealizedPnL"].DefaultColor;

			itemColumnDisplayFactor.ItemColor = prof.ProfileColumns["DisplayFactor"].ColumnForeColor;
			itemColumnTickSize.ItemColor = prof.ProfileColumns["TickSize"].ColumnForeColor;
			itemColumnContractSize.ItemColor = prof.ProfileColumns["ContractSize"].ColumnForeColor;

			// Pick any of the column that has the Up and Down Color use it, Global setting
			itemColorUp.ItemColor = prof.ProfileColumns["BidPrice"].UpColor;
			itemColorDown.ItemColor = prof.ProfileColumns["BidPrice"].DownColor;

			switch (prof.ProfileColumns["BidPrice"].NotifyType)
			{
				case NotifyTypes.BackGround:
					cboNotificationType.SelectedIndex = 2;
					break;
				case NotifyTypes.ForeGround:
					cboNotificationType.SelectedIndex = 3;
					break;
				case NotifyTypes.None:
					cboNotificationType.SelectedIndex = 0;
					break;
				case NotifyTypes.Border:
					cboNotificationType.SelectedIndex = 1;
					break;
			}

			cboRowType.SelectedIndex = (int)prof.GridRowColorType;

			itemFontText.ItemFont = prof.GridFont;
		}

		public ROCPositionProfile GetProfile(ROCPositionProfile prof)
		{
			prof.GridSelectedBackColor = itemColorSelectedBackground.ItemColor;
			prof.GridBackColor = itemColorBackground.ItemColor;
			prof.GridColor = itemColorGridlines.ItemColor;
			prof.GridForeColor = itemColorText.ItemColor;

			prof.ProfileColumns["PositionKey"].ColumnForeColor = itemColumnPositionKey.ItemColor;
			prof.ProfileColumns["Symbol"].ColumnForeColor = itemColumnMDSymbol.ItemColor;
			prof.ProfileColumns["SecType"].ColumnForeColor = itemColumnSecType.ItemColor;

			prof.ProfileColumns["SymbolDetail"].ColumnForeColor = itemColumnSymbol.ItemColor;
			prof.ProfileColumns["SymbolDisplay"].ColumnForeColor = itemColumnDisplay.ItemColor;
			prof.ProfileColumns["BidSize"].ColumnForeColor = itemColumnBidSize.ItemColor;
			prof.ProfileColumns["AskSize"].ColumnForeColor = itemColumnAskSize.ItemColor;
			prof.ProfileColumns["Volume"].ColumnForeColor = itemColumnVolumn.ItemColor;
			prof.ProfileColumns["ClearingAcct"].ColumnForeColor = itemColumnAccount.ItemColor;
			prof.ProfileColumns["Trader"].ColumnForeColor = itemColumnTrader.ItemColor;
			prof.ProfileColumns["OpenAvg"].ColumnForeColor = itemColumnOpenAvg.ItemColor;
			prof.ProfileColumns["BuyQty"].ColumnForeColor = itemColumnBuyQty.ItemColor;
			prof.ProfileColumns["BuyAvg"].ColumnForeColor = itemColumnBuyAvg.ItemColor;
			prof.ProfileColumns["SellQty"].ColumnForeColor = itemColumnSellQty.ItemColor;
			prof.ProfileColumns["SellAvg"].ColumnForeColor = itemColumnSellAvg.ItemColor;

			prof.ProfileColumns["ContractSize"].ColumnForeColor = itemColumnContractSize.ItemColor;
			prof.ProfileColumns["TickSize"].ColumnForeColor = itemColumnTickSize.ItemColor;
			prof.ProfileColumns["DisplayFactor"].ColumnForeColor = itemColumnDisplayFactor.ItemColor;

			NotifyTypes notifyType = NotifyTypes.Border;
			switch (cboNotificationType.SelectedIndex)
			{
				case 0:
					notifyType = NotifyTypes.None;
					break;
				case 2:
					notifyType = NotifyTypes.BackGround;
					break;
				case 3:
					notifyType = NotifyTypes.ForeGround;
					break;
				case 1:
				default:
					notifyType = NotifyTypes.Border;
					break;
			}

			prof.ProfileColumns["BidPrice"].ColumnForeColor = itemColumnBidPrice.ItemColor;
			prof.ProfileColumns["BidPrice"].DefaultColor = itemColumnBidPrice.ItemColor;
			prof.ProfileColumns["BidPrice"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["BidPrice"].DownColor = itemColorDown.ItemColor;
			prof.ProfileColumns["BidPrice"].NotifyType = notifyType;

			prof.ProfileColumns["AskPrice"].ColumnForeColor = itemColumnAskPrice.ItemColor;
			prof.ProfileColumns["AskPrice"].DefaultColor = itemColumnAskPrice.ItemColor;
			prof.ProfileColumns["AskPrice"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["AskPrice"].DownColor = itemColorDown.ItemColor;
			prof.ProfileColumns["AskPrice"].NotifyType = notifyType;

			prof.ProfileColumns["LastTraded"].ColumnForeColor = itemColumnLastTraded.ItemColor;
			prof.ProfileColumns["LastTraded"].DefaultColor = itemColumnLastTraded.ItemColor;
			prof.ProfileColumns["LastTraded"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["LastTraded"].DownColor = itemColorDown.ItemColor;
			prof.ProfileColumns["LastTraded"].NotifyType = notifyType;

			prof.ProfileColumns["NetChange"].ColumnForeColor = itemColumnNetChange.ItemColor;
			prof.ProfileColumns["NetChange"].DefaultColor = itemColumnNetChange.ItemColor;
			prof.ProfileColumns["NetChange"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["NetChange"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["PctChange"].ColumnForeColor = itemColumnPctChange.ItemColor;
			prof.ProfileColumns["PctChange"].DefaultColor = itemColumnPctChange.ItemColor;
			prof.ProfileColumns["PctChange"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["PctChange"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["OpenQty"].ColumnForeColor = itemColumnOpenQty.ItemColor;
			prof.ProfileColumns["OpenQty"].DefaultColor = itemColumnOpenQty.ItemColor;
			prof.ProfileColumns["OpenQty"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["OpenQty"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["CurrentQty"].ColumnForeColor = itemColumnCurrentQty.ItemColor;
			prof.ProfileColumns["CurrentQty"].DefaultColor = itemColumnCurrentQty.ItemColor;
			prof.ProfileColumns["CurrentQty"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["CurrentQty"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["OpenPnL"].ColumnForeColor = itemColumnOpenPnL.ItemColor;
			prof.ProfileColumns["OpenPnL"].DefaultColor = itemColumnOpenPnL.ItemColor;
			prof.ProfileColumns["OpenPnL"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["OpenPnL"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["DayPnL"].ColumnForeColor = itemColumnDayPnL.ItemColor;
			prof.ProfileColumns["DayPnL"].DefaultColor = itemColumnDayPnL.ItemColor;
			prof.ProfileColumns["DayPnL"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["DayPnL"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["TotalPnL"].ColumnForeColor = itemColumnTotalPnL.ItemColor;
			prof.ProfileColumns["TotalPnL"].DefaultColor = itemColumnTotalPnL.ItemColor;
			prof.ProfileColumns["TotalPnL"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["TotalPnL"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["MarketValue"].ColumnForeColor = itemColumnMarketValue.ItemColor;
			prof.ProfileColumns["MarketValue"].DefaultColor = itemColumnMarketValue.ItemColor;
			prof.ProfileColumns["MarketValue"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["MarketValue"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["DayRealizedPnL"].ColumnForeColor = itemColumnRealizedPnL.ItemColor;
			prof.ProfileColumns["DayRealizedPnL"].DefaultColor = itemColumnRealizedPnL.ItemColor;
			prof.ProfileColumns["DayRealizedPnL"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["DayRealizedPnL"].DownColor = itemColorDown.ItemColor;

			switch (cboRowType.SelectedIndex)
			{
				case 1:
					prof.GridRowColorType = ROCPositionList.RowColorTypes.CurrentQty;
					break;
				case 2:
					prof.GridRowColorType = ROCPositionList.RowColorTypes.TotalPnL;
					break;
				case 0:
				default:
					prof.GridRowColorType = ROCPositionList.RowColorTypes.None;
					break;
			}

			prof.GridFont = itemFontText.ItemFont;

			return prof;
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void cmdApply_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Apply"));
			Close();
		}

		private void cmdApplyToAll_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("ApplyToAll"));
			Close();
		}

		private void cboRowType_SelectedIndexChanged(object sender, EventArgs e)
		{
			switch (cboRowType.SelectedIndex)
			{
				case 0:	// None
					grpColumn.Enabled = true;
					break;
				default:
					grpColumn.Enabled = false;
					break;
			}
		}

		private void cmdDefaultColor_Click(object sender, EventArgs e)
		{
			itemColumnPositionKey.ItemColor = itemColorText.ItemColor;
			itemColumnMDSymbol.ItemColor = itemColorText.ItemColor;
			itemColumnSecType.ItemColor = itemColorText.ItemColor;
			itemColumnDisplayFactor.ItemColor = itemColorText.ItemColor;
			itemColumnTickSize.ItemColor = itemColorText.ItemColor;
			itemColumnContractSize.ItemColor = itemColorText.ItemColor;

			itemColumnSymbol.ItemColor = itemColorText.ItemColor;
			itemColumnBidSize.ItemColor = itemColorText.ItemColor;
			itemColumnAskSize.ItemColor = itemColorText.ItemColor;
			itemColumnVolumn.ItemColor = itemColorText.ItemColor;
			itemColumnAccount.ItemColor = itemColorText.ItemColor;
			itemColumnTrader.ItemColor = itemColorText.ItemColor;
			itemColumnOpenAvg.ItemColor = itemColorText.ItemColor;
			itemColumnBuyQty.ItemColor = itemColorText.ItemColor;
			itemColumnBuyAvg.ItemColor = itemColorText.ItemColor;
			itemColumnSellQty.ItemColor = itemColorText.ItemColor;
			itemColumnSellAvg.ItemColor = itemColorText.ItemColor;

			// Default Color For Animated and Stated Columns
			itemColumnBidPrice.ItemColor = itemColorText.ItemColor;
			itemColumnAskPrice.ItemColor = itemColorText.ItemColor;
			itemColumnLastTraded.ItemColor = itemColorText.ItemColor;
			itemColumnNetChange.ItemColor = itemColorText.ItemColor;
			itemColumnPctChange.ItemColor = itemColorText.ItemColor;
			itemColumnOpenQty.ItemColor = itemColorText.ItemColor;
			itemColumnCurrentQty.ItemColor = itemColorText.ItemColor;
			itemColumnOpenPnL.ItemColor = itemColorText.ItemColor;
			itemColumnDayPnL.ItemColor = itemColorText.ItemColor;
			itemColumnTotalPnL.ItemColor = itemColorText.ItemColor;
			itemColumnMarketValue.ItemColor = itemColorText.ItemColor;
			itemColumnRealizedPnL.ItemColor = itemColorText.ItemColor;
		}

		#region - Show -

		public void Display(Control control)
		{
			Display(control, control.ClientRectangle);
		}
		public void Display(Form f, Point p)
		{
			Display(f, new Rectangle(p, new Size(0, 0)));
		}
		private void Display(Control control, Rectangle area)
		{
			if (control == null)
				throw new ArgumentNullException("control can not be null");

			Point location = control.PointToScreen(new Point(area.Left, area.Top + area.Height));

			Rectangle screen = Screen.FromControl(control).WorkingArea;

			if (location.X + Size.Width > (screen.Left + screen.Width))
				location.X = (screen.Left + screen.Width) - Size.Width;

			if (location.Y + Size.Height > (screen.Top + screen.Height))
				location.Y -= Size.Height + area.Height;

			//location = control.PointToClient(location);

			Location = location;
			ShowDialog(control);
		}

		#endregion
	}
}