using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FormEx;
using DataGridViewEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmQuickTicketDisplay : VistaWindowBorderless, INotifyPropertyChanged
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

		public frmQuickTicketDisplay(ROCQuickListProfile grid, ROCTicketProfile ticket)
		{
			InitializeComponent();

			itemColorGridlines.ItemColor = grid.GridColor;

			itemColumnForeBuyOpenQty.ItemColor = grid.ProfileColumns["BuyOpenQty"].ColumnForeColor;
			itemColumnForeBidQty.ItemColor = grid.ProfileColumns["BidQty"].ColumnForeColor;
			itemColumnForeBidSize.ItemColor = grid.ProfileColumns["BidSize"].ColumnForeColor;
			itemColumnForePrice.ItemColor = grid.ProfileColumns["Price"].ColumnForeColor;
			itemColumnForeAskSize.ItemColor = grid.ProfileColumns["AskSize"].ColumnForeColor;
			itemColumnForeAskQty.ItemColor = grid.ProfileColumns["AskQty"].ColumnForeColor;
			itemColumnForeSellOpenQty.ItemColor = grid.ProfileColumns["SellOpenQty"].ColumnForeColor;

			itemColumnBackBuyOpenQty.ItemColor = grid.ProfileColumns["BuyOpenQty"].ColumnBackColor;
			itemColumnBackBidQty.ItemColor = grid.ProfileColumns["BidQty"].ColumnBackColor;
			itemColumnBackBidSize.ItemColor = grid.ProfileColumns["BidSize"].ColumnBackColor;
			itemColumnBackPrice.ItemColor = grid.ProfileColumns["Price"].ColumnBackColor;
			itemColumnBackAskSize.ItemColor = grid.ProfileColumns["AskSize"].ColumnBackColor;
			itemColumnBackAskQty.ItemColor = grid.ProfileColumns["AskQty"].ColumnBackColor;
			itemColumnBackSellOpenQty.ItemColor = grid.ProfileColumns["SellOpenQty"].ColumnBackColor;

			itemFontText.ItemFont = grid.GridFont;

			itemColorTicketBackColor.ItemColor = ticket.TicketBackColor;
			itemOpacity.Value = Convert.ToInt32(ticket.TicketOpacity * 100);
			itemColorSelectedStopLimitBackColor.ItemColor = grid.SelectedStopLimitBackColor;

			chkAutoCenter.Checked = grid.GridAutoCenterOnTraded;
			numAutoCenterRange.Value = grid.GridAutoCenterRange;

			chkShowBidPrice.Checked = ticket.ShowBidPrice;
			chkShowBidSize.Checked = ticket.ShowBidSize;
			chkShowAskPrice.Checked = ticket.ShowAskPrice;
			chkShowAskSize.Checked = ticket.ShowAskSize;
			chkShowNetChange.Checked = ticket.ShowNetChange;
			chkShowPctChange.Checked = ticket.ShowPctChange;
			chkShowTotalVolume.Checked = ticket.ShowTotalVolume;
			chkShowTradedVolume.Checked = ticket.ShowTradedVolume;
			chkShowLowPrice.Checked = ticket.ShowLowPrice;
			chkShowHighPrice.Checked = ticket.ShowHighPrice;
			chkShowPrevClosePrice.Checked = ticket.ShowPrevClosePrice;

			chkShowQty.Checked = ticket.ShowQty;
			chkShowOrderType.Checked = ticket.ShowOrder;
			chkShowLimitPrice.Checked = ticket.ShowLimitPrice;
			chkShowStopPrice.Checked = ticket.ShowStopPrice;
			chkShowDuration.Checked = ticket.ShowDuration;
			chkShowExchange.Checked = ticket.ShowExchange;

			chkShowOrderInfo.Checked = ticket.ShowOrderInfo;

			chkShowCommand.Checked = ticket.ShowCommand;

			chkShowSelectedAccountOnly.Checked = ticket.ShowSelectedAccountOnly;
		}

		public ROCQuickListProfile GetProfile(ROCQuickListProfile prof)
		{
			prof.GridColor = itemColorGridlines.ItemColor;

			prof.ProfileColumns["BuyOpenQty"].ColumnForeColor = itemColumnForeBuyOpenQty.ItemColor;
			prof.ProfileColumns["BidQty"].ColumnForeColor = itemColumnForeBidQty.ItemColor;
			prof.ProfileColumns["BidSize"].ColumnForeColor = itemColumnForeBidSize.ItemColor;
			prof.ProfileColumns["Price"].ColumnForeColor = itemColumnForePrice.ItemColor;
			prof.ProfileColumns["AskSize"].ColumnForeColor = itemColumnForeAskSize.ItemColor;
			prof.ProfileColumns["AskQty"].ColumnForeColor = itemColumnForeAskQty.ItemColor;
			prof.ProfileColumns["SellOpenQty"].ColumnForeColor = itemColumnForeSellOpenQty.ItemColor;

			prof.ProfileColumns["BuyOpenQty"].ColumnBackColor = itemColumnBackBuyOpenQty.ItemColor;
			prof.ProfileColumns["BidQty"].ColumnBackColor = itemColumnBackBidQty.ItemColor;
			prof.ProfileColumns["BidSize"].ColumnBackColor = itemColumnBackBidSize.ItemColor;
			prof.ProfileColumns["Price"].ColumnBackColor = itemColumnBackPrice.ItemColor;
			prof.ProfileColumns["AskSize"].ColumnBackColor = itemColumnBackAskSize.ItemColor;
			prof.ProfileColumns["AskQty"].ColumnBackColor = itemColumnBackAskQty.ItemColor;
			prof.ProfileColumns["SellOpenQty"].ColumnBackColor = itemColumnBackSellOpenQty.ItemColor;

			prof.GridFont = itemFontText.ItemFont;

			prof.SelectedStopLimitBackColor = itemColorSelectedStopLimitBackColor.ItemColor;

			prof.GridAutoCenterOnTraded = chkAutoCenter.Checked;
			prof.GridAutoCenterRange = (long)numAutoCenterRange.Value;

			return prof;
		}

		public ROCTicketProfile GetProfile(ROCTicketProfile prof)
		{
			prof.TicketOpacity = itemOpacity.Value / 100F;
			prof.TicketBackColor = itemColorTicketBackColor.ItemColor;

			prof.ShowBidPrice = chkShowBidPrice.Checked;
			prof.ShowBidSize = chkShowBidSize.Checked;
			prof.ShowAskPrice = chkShowAskPrice.Checked;
			prof.ShowAskSize = chkShowAskSize.Checked;
			prof.ShowNetChange = chkShowNetChange.Checked;
			prof.ShowPctChange = chkShowPctChange.Checked;
			prof.ShowTotalVolume = chkShowTotalVolume.Checked;
			prof.ShowTradedVolume = chkShowTradedVolume.Checked;
			prof.ShowLowPrice = chkShowLowPrice.Checked;
			prof.ShowHighPrice = chkShowHighPrice.Checked;
			prof.ShowPrevClosePrice = chkShowPrevClosePrice.Checked;

			prof.ShowQty = chkShowQty.Checked;
			prof.ShowOrder = chkShowOrderType.Checked;
			prof.ShowLimitPrice = chkShowLimitPrice.Checked;
			prof.ShowStopPrice = chkShowStopPrice.Checked;
			prof.ShowDuration = chkShowDuration.Checked;
			prof.ShowExchange = chkShowExchange.Checked;

			prof.ShowOrderInfo = chkShowOrderInfo.Checked;

			prof.ShowCommand = chkShowCommand.Checked;

			prof.ShowSelectedAccountOnly = chkShowSelectedAccountOnly.Checked;

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

		private void cmdDefaultForeColor_Click(object sender, EventArgs e)
		{
			itemColumnForeBuyOpenQty.ItemColor = Color.LimeGreen;
			itemColumnForeBidQty.ItemColor = Color.White;
			itemColumnForeBidSize.ItemColor = Color.LimeGreen;
			itemColumnForePrice.ItemColor = Color.Black;
			itemColumnForeAskSize.ItemColor = Color.Red;
			itemColumnForeAskQty.ItemColor = Color.White;
			itemColumnForeSellOpenQty.ItemColor = Color.Red;
		}

		private void cmdDefaultBackColor_Click(object sender, EventArgs e)
		{
			itemColumnBackBuyOpenQty.ItemColor = Color.Black;
			itemColumnBackBidQty.ItemColor = Color.LimeGreen;
			itemColumnBackBidSize.ItemColor = Color.Black;
			itemColumnBackPrice.ItemColor = Color.WhiteSmoke;
			itemColumnBackAskSize.ItemColor = Color.Black;
			itemColumnBackAskQty.ItemColor = Color.Red;
			itemColumnBackSellOpenQty.ItemColor = Color.Black;
		}

		private void cmdDefaultGridColor_Click(object sender, EventArgs e)
		{
			itemColorGridlines.ItemColor = Color.WhiteSmoke;
		}

		private void cmdDefaultTicketColor_Click(object sender, EventArgs e)
		{
			itemOpacity.Value = 100;
			itemColorTicketBackColor.ItemColor = Color.LightSteelBlue;
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