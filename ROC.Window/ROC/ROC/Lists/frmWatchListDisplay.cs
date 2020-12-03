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
	public partial class frmWatchListDisplay : VistaWindowBorderless, INotifyPropertyChanged
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

		public int RowColorType
		{
			get
			{
				return cboRowType.SelectedIndex;
			}
		}

		public frmWatchListDisplay(ROCWatchListProfile prof)
		{
			InitializeComponent();

			itemColumnSymbol.ItemColor = prof.ProfileColumns["Symbol"].ColumnForeColor;
			itemColumnDetail.ItemColor = prof.ProfileColumns["SymbolDetail"].ColumnForeColor;
			itemColumnBidSize.ItemColor = prof.ProfileColumns["BidSize"].ColumnForeColor;
			itemColumnBidPartID.ItemColor = prof.ProfileColumns["BidPartID"].ColumnForeColor;
			itemColumnBidHigh.ItemColor = prof.ProfileColumns["BidHigh"].ColumnForeColor;
			itemColumnBidLow.ItemColor = prof.ProfileColumns["BidLow"].ColumnForeColor;
			itemColumnAskSize.ItemColor = prof.ProfileColumns["AskSize"].ColumnForeColor;
			itemColumnAskPartID.ItemColor = prof.ProfileColumns["AskPartID"].ColumnForeColor;
			itemColumnAskHigh.ItemColor = prof.ProfileColumns["AskHigh"].ColumnForeColor;
			itemColumnAskLow.ItemColor = prof.ProfileColumns["AskLow"].ColumnForeColor;
			itemColumnTradeVolume.ItemColor = prof.ProfileColumns["TradeVolume"].ColumnForeColor;
			itemColumnTradePartID.ItemColor = prof.ProfileColumns["TradePartID"].ColumnForeColor;
			itemColumnVolume.ItemColor = prof.ProfileColumns["Volume"].ColumnForeColor;
			itemColumnHigh.ItemColor = prof.ProfileColumns["High"].ColumnForeColor;
			itemColumnLow.ItemColor = prof.ProfileColumns["Low"].ColumnForeColor;
			itemColumnOpen.ItemColor = prof.ProfileColumns["Open"].ColumnForeColor;
			itemColumnClose.ItemColor = prof.ProfileColumns["Close"].ColumnForeColor;
			itemColumnVwap.ItemColor = prof.ProfileColumns["Vwap"].ColumnForeColor;
			itemColumnName.ItemColor = prof.ProfileColumns["Name"].ColumnForeColor;

			itemColumnTickSize.ItemColor = prof.ProfileColumns["TickSize"].ColumnForeColor;
			itemColumnDisplayFactor.ItemColor = prof.ProfileColumns["DisplayFactor"].ColumnForeColor;
			itemColumnSecType.ItemColor = prof.ProfileColumns["SecType"].ColumnForeColor;

			itemColumnLineTime.ItemColor = prof.ProfileColumns["LineTime"].ColumnForeColor;
			itemColumnDecodeTime.ItemColor = prof.ProfileColumns["DecodeTime"].ColumnForeColor;
			itemColumnOnMsgTime.ItemColor = prof.ProfileColumns["OnMsgTime"].ColumnForeColor;
			itemColumnServerSentTime.ItemColor = prof.ProfileColumns["ServerSentTime"].ColumnForeColor;
			itemColumnClientRecivedTime.ItemColor = prof.ProfileColumns["ClientRecivedTime"].ColumnForeColor;

			// Default Color For Animated and Stated Columns
			itemColumnBidPrice.ItemColor = prof.ProfileColumns["BidPrice"].DefaultColor;
			itemColumnAskPrice.ItemColor = prof.ProfileColumns["AskPrice"].DefaultColor;
			itemColumnLastTraded.ItemColor = prof.ProfileColumns["LastTraded"].DefaultColor;
			itemColumnTick.ItemColor = prof.ProfileColumns["Tick"].DefaultColor;
			itemColumnNetChange.ItemColor = prof.ProfileColumns["NetChange"].DefaultColor;
			itemColumnPctChange.ItemColor = prof.ProfileColumns["PctChange"].DefaultColor;

			itemColorSelectedBackground.ItemColor = prof.GridSelectedBackColor;
			itemColorBackground.ItemColor = prof.GridBackColor;
			itemColorGridlines.ItemColor = prof.GridColor;
			itemColorText.ItemColor = prof.GridForeColor;

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

		public ROCWatchListProfile GetProfile(ROCWatchListProfile prof)
		{
			prof.GridSelectedBackColor = itemColorSelectedBackground.ItemColor;
			prof.GridBackColor = itemColorBackground.ItemColor;
			prof.GridColor = itemColorGridlines.ItemColor;
			prof.GridForeColor = itemColorText.ItemColor;

			prof.ProfileColumns["Symbol"].ColumnForeColor = itemColumnSymbol.ItemColor;
			prof.ProfileColumns["SymbolDetail"].ColumnForeColor = itemColumnDetail.ItemColor;
			prof.ProfileColumns["BidSize"].ColumnForeColor = itemColumnBidSize.ItemColor;
			prof.ProfileColumns["BidPartID"].ColumnForeColor = itemColumnBidPartID.ItemColor;
			prof.ProfileColumns["BidHigh"].ColumnForeColor = itemColumnBidHigh.ItemColor;
			prof.ProfileColumns["BidLow"].ColumnForeColor = itemColumnBidLow.ItemColor;
			prof.ProfileColumns["AskSize"].ColumnForeColor = itemColumnAskSize.ItemColor;
			prof.ProfileColumns["AskPartID"].ColumnForeColor = itemColumnAskPartID.ItemColor;
			prof.ProfileColumns["AskHigh"].ColumnForeColor = itemColumnAskHigh.ItemColor;
			prof.ProfileColumns["AskLow"].ColumnForeColor = itemColumnAskLow.ItemColor;
			prof.ProfileColumns["TradeVolume"].ColumnForeColor = itemColumnTradeVolume.ItemColor;
			prof.ProfileColumns["TradePartID"].ColumnForeColor = itemColumnTradePartID.ItemColor;
			prof.ProfileColumns["Volume"].ColumnForeColor = itemColumnVolume.ItemColor;
			prof.ProfileColumns["High"].ColumnForeColor = itemColumnHigh.ItemColor;
			prof.ProfileColumns["Low"].ColumnForeColor = itemColumnLow.ItemColor;
			prof.ProfileColumns["Open"].ColumnForeColor = itemColumnOpen.ItemColor;
			prof.ProfileColumns["Close"].ColumnForeColor = itemColumnClose.ItemColor;
			prof.ProfileColumns["Vwap"].ColumnForeColor = itemColumnVwap.ItemColor;
			prof.ProfileColumns["Name"].ColumnForeColor = itemColumnName.ItemColor;

			prof.ProfileColumns["TickSize"].ColumnForeColor = itemColumnTickSize.ItemColor;
			prof.ProfileColumns["DisplayFactor"].ColumnForeColor = itemColumnDisplayFactor.ItemColor;
			prof.ProfileColumns["SecType"].ColumnForeColor = itemColumnSecType.ItemColor;

			prof.ProfileColumns["LineTime"].ColumnForeColor = itemColumnLineTime.ItemColor;
			prof.ProfileColumns["DecodeTime"].ColumnForeColor = itemColumnDecodeTime.ItemColor;
			prof.ProfileColumns["OnMsgTime"].ColumnForeColor = itemColumnOnMsgTime.ItemColor;
			prof.ProfileColumns["ServerSentTime"].ColumnForeColor = itemColumnServerSentTime.ItemColor;
			prof.ProfileColumns["ClientRecivedTime"].ColumnForeColor = itemColumnClientRecivedTime.ItemColor;

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

			prof.ProfileColumns["Tick"].ColumnForeColor = itemColumnTick.ItemColor;
			prof.ProfileColumns["Tick"].DefaultColor = itemColumnTick.ItemColor;
			prof.ProfileColumns["Tick"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["Tick"].DownColor = itemColorDown.ItemColor;
			prof.ProfileColumns["Tick"].NotifyType = notifyType;

			prof.ProfileColumns["NetChange"].ColumnForeColor = itemColumnNetChange.ItemColor;
			prof.ProfileColumns["NetChange"].DefaultColor = itemColumnNetChange.ItemColor;
			prof.ProfileColumns["NetChange"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["NetChange"].DownColor = itemColorDown.ItemColor;

			prof.ProfileColumns["PctChange"].ColumnForeColor = itemColumnPctChange.ItemColor;
			prof.ProfileColumns["PctChange"].DefaultColor = itemColumnPctChange.ItemColor;
			prof.ProfileColumns["PctChange"].UpColor = itemColorUp.ItemColor;
			prof.ProfileColumns["PctChange"].DownColor = itemColorDown.ItemColor;

			switch (cboRowType.SelectedIndex)
			{
				case 1:
					prof.GridRowColorType = ROCWatchList.RowColorTypes.Performence;
					break;
				case 0:
				default:
					prof.GridRowColorType = ROCWatchList.RowColorTypes.None;
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
			itemColumnSymbol.ItemColor = itemColorText.ItemColor;
			itemColumnDetail.ItemColor = itemColorText.ItemColor;

			itemColumnSymbol.ItemColor = itemColorText.ItemColor;
			itemColumnDetail.ItemColor = itemColorText.ItemColor;
			itemColumnBidSize.ItemColor = itemColorText.ItemColor;
			itemColumnBidPartID.ItemColor = itemColorText.ItemColor;
			itemColumnBidHigh.ItemColor = itemColorText.ItemColor;
			itemColumnBidLow.ItemColor = itemColorText.ItemColor;
			itemColumnAskSize.ItemColor = itemColorText.ItemColor;
			itemColumnAskPartID.ItemColor = itemColorText.ItemColor;
			itemColumnAskHigh.ItemColor = itemColorText.ItemColor;
			itemColumnAskLow.ItemColor = itemColorText.ItemColor;
			itemColumnTradeVolume.ItemColor = itemColorText.ItemColor;
			itemColumnTradePartID.ItemColor = itemColorText.ItemColor;
			itemColumnVolume.ItemColor = itemColorText.ItemColor;
			itemColumnHigh.ItemColor = itemColorText.ItemColor;
			itemColumnLow.ItemColor = itemColorText.ItemColor;
			itemColumnOpen.ItemColor = itemColorText.ItemColor;
			itemColumnClose.ItemColor = itemColorText.ItemColor;
			itemColumnVwap.ItemColor = itemColorText.ItemColor;
			itemColumnName.ItemColor = itemColorText.ItemColor;

			itemColumnLineTime.ItemColor = itemColorText.ItemColor;
			itemColumnDecodeTime.ItemColor = itemColorText.ItemColor;
			itemColumnOnMsgTime.ItemColor = itemColorText.ItemColor;
			itemColumnServerSentTime.ItemColor = itemColorText.ItemColor;
			itemColumnClientRecivedTime.ItemColor = itemColorText.ItemColor;

			itemColumnTickSize.ItemColor = itemColorText.ItemColor;
			itemColumnDisplayFactor.ItemColor = itemColorText.ItemColor;
			itemColumnSecType.ItemColor = itemColorText.ItemColor;

			// Default Color For Animated and Stated Columns
			itemColumnBidPrice.ItemColor = itemColorText.ItemColor;
			itemColumnAskPrice.ItemColor = itemColorText.ItemColor;
			itemColumnLastTraded.ItemColor = itemColorText.ItemColor;
			itemColumnNetChange.ItemColor = itemColorText.ItemColor;
			itemColumnPctChange.ItemColor = itemColorText.ItemColor;
			itemColumnTick.ItemColor = itemColorText.ItemColor;
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