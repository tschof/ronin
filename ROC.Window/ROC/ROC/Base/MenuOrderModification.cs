using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using System.Data;
using RDSEx;
using CSVEx;
using System.Windows.Forms;
using MarketData;

namespace ROC
{
	internal class MenuOrderModification
	{
		private double _stockSmallIncr = 0.01;
		private double _stockLargeIncr = 0.05;

		#region - Property -

		private BaseSecurityInfo _currentSecInfo = null;
		internal BaseSecurityInfo CurrentSecInfo
		{
			get
			{
				return _currentSecInfo;
			}
			set
			{
				_currentSecInfo = value;
			}
		}

		internal sealed class TradeTicks
		{
			internal const int Up = 0;
			internal const int None = 1;
			internal const int Down = 2;
		}

		private Dictionary<string, string> _imSymbolNeeded;
		internal Dictionary<string, string> ImSymbolNeeded
		{
			get
			{
				if (_imSymbolNeeded == null)
				{
					_imSymbolNeeded = new Dictionary<string, string>();
				}
				return _imSymbolNeeded;
			}
			set
			{
				_imSymbolNeeded = value;
			}
		}

		private List<string> _mdSymbols = new List<string>();
		internal List<string> MDSymbols
		{
			get
			{
				return _mdSymbols;
			}
			set
			{
				_mdSymbols = value;
			}
		}

		internal long GetOrderTypeCode(string type)
		{
			switch (type)
			{
				case "MARKET":
					return CSVEx.CSVFieldIDs.OrderTypes.Market;
				case "MOC":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose;
				case "MOO":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen;
				case "LOC":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitOnClose;
				case "STOP":
					return CSVEx.CSVFieldIDs.OrderTypes.Stop;
				case "STOPLIMIT":
					return CSVEx.CSVFieldIDs.OrderTypes.StopLimit;
				case "OCO":
					return CSVEx.CSVFieldIDs.OrderTypes.OCO;
				case "LIMIT":
				default:
					return CSVEx.CSVFieldIDs.OrderTypes.Limit;
			}
		}

		internal long GetDurationCode(string tif)
		{
			switch (tif)
			{
				case "GTC":
					return CSVEx.CSVFieldIDs.TIFCodes.GTC;
				case "IOC":
					return CSVEx.CSVFieldIDs.TIFCodes.IOC;
				case "FOK":
					return CSVEx.CSVFieldIDs.TIFCodes.FOK;
				case "OPG":
					return CSVEx.CSVFieldIDs.TIFCodes.OPG;
				case "DAY":
				default:
					return CSVEx.CSVFieldIDs.TIFCodes.Day;
			}
		}

		private menuBaseOrderModification _parent;
		private menuFullOrderModification _fullParent;
		private bool _isFull = false;

		private bool isSpread
		{
			get
			{
				if (CurrentOrder != null && CurrentOrder.SymbolDetail.Contains("-"))
				{
					return true;
				}

				return false;
			}
		}

		internal string NewDuration
		{
			get
			{
				return GetDuration().ToString();
			}
		}

		private DataView _orderTypesView;
		private DataView _durationsView;

		private ROCOrder _currentOrder = null;
		internal ROCOrder CurrentOrder
		{
			get
			{
				return _currentOrder;
			}
		}

		private decimal _initialLeaveQty = 0m;
		internal decimal InitialLeaveQty
		{
			get
			{
				return _initialLeaveQty;
			}
		}

		private SymbolSettingData _settingData;

		private string orderMsg
		{
			get
			{
				return dspMessage.Text;
			}
			set
			{
				dspMessage.Text = value;
			}
		}

		#endregion

		#region - Parent Controls -

		private Label dspMessage
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspMessage;
				}
				else
				{
					return _parent.dspMessage;
				}
			}
		}

		private ComboBox cboOrder
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.cboOrder;
				}
				else
				{
					return _parent.cboOrder;
				}
			}
		}

		private Panel panelLimitPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.panelLimitPrice;
				}
				else
				{
					return _parent.panelLimitPrice;
				}
			}
		}

		private Panel panelStopPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.panelStopPrice;
				}
				else
				{
					return _parent.panelStopPrice;
				}
			}
		}

		private Panel panelPegPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.panelPegPrice;
				}
				else
				{
					return _parent.panelPegPrice;
				}
			}
		}

		private ComboBox cboDuration
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.cboDuration;
				}
				else
				{
					return _parent.cboDuration;
				}
			}
		}

		private NumericUpDown numQuantity
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.numQuantity;
				}
				else
				{
					return _parent.numQuantity;
				}
			}
		}

		private NumericUpDown numShow
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.numShow;
				}
				else
				{
					return _parent.numShow;
				}
			}
		}

		private NumericUpDown numLimitPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.numLimitPrice;
				}
				else
				{
					return _parent.numLimitPrice;
				}
			}
		}

		private NumericUpDown numStopPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.numStopPrice;
				}
				else
				{
					return _parent.numStopPrice;
				}
			}
		}

		private NumericUpDown numPegPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.numPegPrice;
				}
				else
				{
					return _parent.numPegPrice;
				}
			}
		}

		private LabelEx.LabelBase dspQuantityIncrement
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspQuantityIncrement;
				}
				else
				{
					return _parent.dspQuantityIncrement;
				}
			}
		}

		private LabelEx.LabelBase dspShowIncrement
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspShowIncrement;
				}
				else
				{
					return _parent.dspShowIncrement;
				}
			}
		}

		private LabelEx.LabelBase dspLimitPriceTickSize
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspLimitPriceTickSize;
				}
				else
				{
					return _parent.dspLimitPriceTickSize;
				}
			}
		}

		private LabelEx.LabelBase dspLimitPriceIncrement
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspLimitPriceIncrement;
				}
				else
				{
					return _parent.dspLimitPriceIncrement;
				}
			}
		}

		private LabelEx.LabelBase dspStopPriceTickSize
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspStopPriceTickSize;
				}
				else
				{
					return _parent.dspStopPriceTickSize;
				}
			}
		}

		private LabelEx.LabelBase dspStopPriceIncrement
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspStopPriceIncrement;
				}
				else
				{
					return _parent.dspStopPriceIncrement;
				}
			}
		}

		private LabelEx.LabelBase dspPegPriceTickSize
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspPegPriceTickSize;
				}
				else
				{
					return _parent.dspPegPriceTickSize;
				}
			}
		}

		private LabelEx.LabelBase dspPegPriceIncrement
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspPegPriceIncrement;
				}
				else
				{
					return _parent.dspPegPriceIncrement;
				}
			}
		}

		private CheckBox chkShow
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.chkShow;
				}
				else
				{
					return _parent.chkShow;
				}
			}
		}

		private Label dspSymbolDetail
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspSymbolDetail;
				}
				else
				{
					return null;
				}
			}
		}

		private Panel panelTicker
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.panelTicker;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspBidPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspBidPrice;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspBidSize
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspBidSize;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspAskPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspAskPrice;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspAskSize
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspAskSize;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspTradedPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspTradedPrice;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspVolume
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspVolume;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspTradeVolume
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspTradeVolume;
				}
				else
				{
					return null;
				}
			}
		}

		private PictureBox picTradeTick
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.picTradeTick;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspNetChange
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspNetChange;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspPctChange
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspPctChange;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspHighPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspHighPrice;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspLowPrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspLowPrice;
				}
				else
				{
					return null;
				}
			}
		}

		private LabelEx.LabelBase dspClosePrice
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.dspClosePrice;
				}
				else
				{
					return null;
				}
			}
		}

		private ImageList imageListTradeTick
		{
			get
			{
				if (_isFull)
				{
					return _fullParent.imageListTradeTick;
				}
				else
				{
					return null;
				}
			}
		}

		#endregion

		internal MenuOrderModification(menuBaseOrderModification parent)
		{
			_isFull = false;
			_parent = parent;
		}

		internal MenuOrderModification(menuFullOrderModification fullParent)
		{
			_isFull = true;
			_fullParent = fullParent;
		}

		internal void Initialize(
			string orderID,
			decimal qtyIncrement,
			decimal showIncrement,
			decimal limitPriceIncrement,
			decimal stopPriceIncrement,
			decimal pegPriceIncrement)
		{
			_currentOrder = GLOBAL.HOrders.GetOrderByOrderID(orderID);
			if (CurrentOrder != null)
			{
				if (CurrentOrder.Side == null)
				{
					orderMsg = string.Concat(new object[] { "Missing Side For Order " + CurrentOrder.Tag });
				}

				_settingData = GLOBAL.HSymbolSettingData.GetSymbolDefaults(CurrentOrder.SecType);
				_settingData.qtyIncrement = qtyIncrement;
				_settingData.showIncrement = showIncrement;
				_settingData.limitPriceIncrement = limitPriceIncrement;
				_settingData.stopPriceIncrement = stopPriceIncrement;
				_settingData.pegPriceIncrement = pegPriceIncrement;
				
				SetSymbolDefault();

				SetOrderType(CurrentOrder.OrderType);
				SetDuration(CurrentOrder.TIF);

				UpdateTicketOrderInfo();

				SubscribeToSymbol(CurrentOrder.SymbolDetail);
			}
			else
			{
				orderMsg = string.Concat(new object[] { "Order Not Found " + orderID });
			}
		}

		private void SetOrderType(long orderType)
		{
			try
			{
				switch (orderType)
				{
					case CSVFieldIDs.OrderTypes.Market:
						cboOrder.Text = "MARKET";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.MarketOnClose:
						cboOrder.Text = "MOC";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.MarketAtOpen:
						cboOrder.Text = "MOO";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.LimitOnClose:
						cboOrder.Text = "LOC";
						panelLimitPrice.Enabled = true;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.Stop:
						cboOrder.Text = "STOP";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = true;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.StopLimit:
						cboOrder.Text = "STOPLIMIT";
						panelLimitPrice.Enabled = true;
						panelStopPrice.Enabled = true;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.PegMkt:
						cboOrder.Text = "PEG MKT";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = true;
						break;
					case CSVFieldIDs.OrderTypes.PegPri:
						cboOrder.Text = "PEG PRI";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = true;
						break;
					case CSVFieldIDs.OrderTypes.PegMid:
						cboOrder.Text = "PEG MID";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = true;
						break;
					case CSVFieldIDs.OrderTypes.VWAPMKT:
						cboOrder.Text = "VWAP MKT";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.VWAPLIM:
						cboOrder.Text = "VWAP LIM";
						panelLimitPrice.Enabled = true;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.IMBOO:
						cboOrder.Text = "IMBOO";
						panelLimitPrice.Enabled = false;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.IMBOC:
						cboOrder.Text = "IMBOC";
						panelLimitPrice.Enabled = true;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.IMBOI:
						cboOrder.Text = "IMBOI";
						panelLimitPrice.Enabled = true;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
					case CSVFieldIDs.OrderTypes.Limit:
					default:
						cboOrder.Text = "LIMIT";
						panelLimitPrice.Enabled = true;
						panelStopPrice.Enabled = false;
						panelPegPrice.Enabled = false;
						break;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void SetDuration(long tif)
		{
			switch (tif)
			{
				case CSVFieldIDs.TIFCodes.GTC:
					cboDuration.Text = "GTC";
					break;
				case CSVFieldIDs.TIFCodes.IOC:
					cboDuration.Text = "IOC";
					break;
				case CSVFieldIDs.TIFCodes.FOK:
					cboDuration.Text = "FOK";
					break;
				case CSVFieldIDs.TIFCodes.OPG:
					cboDuration.Text = "OPG";
					break;
				case CSVFieldIDs.TIFCodes.Day:
				default:
					cboDuration.Text = "DAY";
					break;
			}
		}

		private long GetDuration()
		{
			switch (cboDuration.Text)
			{
				case "GTC":
					return CSVFieldIDs.TIFCodes.GTC;
				case "IOC":
					return CSVFieldIDs.TIFCodes.IOC;
				case "FOK":
					return CSVFieldIDs.TIFCodes.FOK;
				case "OPG":
					return CSVFieldIDs.TIFCodes.OPG;
				case "DAY":
				default:
					return CSVFieldIDs.TIFCodes.Day;
			}
		}

		private void SetSymbolDefault()
		{
			numQuantity.Increment = _settingData.qtyIncrement;
			numShow.Increment = _settingData.showIncrement;

			dspQuantityIncrement.Value = _settingData.qtyIncrement;
			dspShowIncrement.Value = _settingData.showIncrement;

			_orderTypesView = new DataView(GLOBAL.HOrders.BaseOrderTypes);
			cboOrder.DataSource = _orderTypesView;
			cboOrder.DisplayMember = "Type";
			cboOrder.Text = "LIMIT";

			_durationsView = new DataView(GLOBAL.HOrders.Durations);
			cboDuration.DataSource = _durationsView;
			cboDuration.DisplayMember = "Duration";
			cboDuration.Text = "DAY";
		}

		private void UpdateTicket()
		{
			if (_isFull)
			{
				dspSymbolDetail.Text = CurrentSecInfo.MDSymbol;

				dspAskPrice.TickSize = CurrentSecInfo.TickSize;
				dspBidPrice.TickSize = CurrentSecInfo.TickSize;
				//dspNetChange.TickSize = CurrentSecInfo.TickSize;
				dspLowPrice.TickSize = CurrentSecInfo.TickSize;
				dspHighPrice.TickSize = CurrentSecInfo.TickSize;
				dspClosePrice.TickSize = CurrentSecInfo.TickSize;
				dspTradedPrice.TickSize = CurrentSecInfo.TickSize;

				dspAskPrice.MaxDecimal = CurrentSecInfo.DecimalPlaces;
				dspBidPrice.MaxDecimal = CurrentSecInfo.DecimalPlaces;
				dspLowPrice.MaxDecimal = CurrentSecInfo.DecimalPlaces;
				dspHighPrice.MaxDecimal = CurrentSecInfo.DecimalPlaces;
				dspClosePrice.MaxDecimal = CurrentSecInfo.DecimalPlaces;
				dspTradedPrice.MaxDecimal = CurrentSecInfo.DecimalPlaces;
			}

			#region - Limit Price -

			if ((_settingData.limitPriceIncrement % CurrentSecInfo.TickSizeDec) != 0)
			{
				_settingData.limitPriceIncrement = CurrentSecInfo.TickSizeDec;
			}

			numLimitPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
			switch (CurrentOrder.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Option:
					SetOptionPriceIncrement(numLimitPrice, dspLimitPriceTickSize);
					dspLimitPriceIncrement.Value = 1;
					break;
				case CSVFieldIDs.SecurityTypes.Spread:
					numLimitPrice.Minimum = -numLimitPrice.Maximum;
					numLimitPrice.Increment = _settingData.limitPriceIncrement;
					dspLimitPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;
					dspLimitPriceIncrement.Value = _settingData.limitPriceIncrement / CurrentSecInfo.TickSizeDec;
					break;
				default:
					numLimitPrice.Increment = _settingData.limitPriceIncrement;
					dspLimitPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;
					dspLimitPriceIncrement.Value = _settingData.limitPriceIncrement / CurrentSecInfo.TickSizeDec;
					break;
			}

			#endregion

			#region - Stop Price -

			if ((_settingData.stopPriceIncrement % CurrentSecInfo.TickSizeDec) != 0)
			{
				_settingData.stopPriceIncrement = CurrentSecInfo.TickSizeDec;
			}

			numStopPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
			switch (CurrentOrder.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Option:
					numStopPrice.Minimum = 0;
					SetOptionPriceIncrement(numStopPrice, dspStopPriceTickSize);
					dspStopPriceIncrement.Value = 1;
					break;
				case CSVFieldIDs.SecurityTypes.Spread:
					numStopPrice.Minimum = -numStopPrice.Maximum;
					numStopPrice.Increment = _settingData.stopPriceIncrement;
					dspStopPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;
					dspStopPriceIncrement.Value = _settingData.stopPriceIncrement / CurrentSecInfo.TickSizeDec;
					break;
				default:
					numStopPrice.Minimum = 0;
					numStopPrice.Increment = _settingData.stopPriceIncrement;
					dspStopPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;
					dspStopPriceIncrement.Value = _settingData.stopPriceIncrement / CurrentSecInfo.TickSizeDec;
					break;
			}

			numStopPrice.Increment = _settingData.stopPriceIncrement;
			dspStopPriceTickSize.TickSize = CurrentSecInfo.TickSize;
			dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;
			dspStopPriceIncrement.Value = _settingData.stopPriceIncrement / CurrentSecInfo.TickSizeDec;

			#endregion

			#region - Peg Price -

			if ((_settingData.pegPriceIncrement % CurrentSecInfo.TickSizeDec) != 0)
			{
				_settingData.pegPriceIncrement = CurrentSecInfo.TickSizeDec;
			}

			numPegPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
			numPegPrice.Increment = _settingData.pegPriceIncrement;
			dspPegPriceTickSize.TickSize = CurrentSecInfo.TickSize;
			dspPegPriceTickSize.Value = CurrentSecInfo.TickSize;
			dspPegPriceIncrement.Value = _settingData.pegPriceIncrement / CurrentSecInfo.TickSizeDec;

			#endregion
		}

		private void UpdateTicketOrderInfo()
		{
			if (isSpread)
			{
				numLimitPrice.Minimum = -numLimitPrice.Maximum;
				numStopPrice.Minimum = -numStopPrice.Maximum;
			}
			else
			{
				numLimitPrice.Minimum = 0;
				numStopPrice.Minimum = 0;
			}

			_initialLeaveQty = Convert.ToDecimal(CurrentOrder.LeaveQty);
			numQuantity.Value = _initialLeaveQty;

			// 0 Mean Hide All, '' Mean Show All
			if (_currentOrder.MaxFloor == null || CurrentOrder.MaxFloor == "")
			{
				chkShow.Checked = false;
			}
			else
			{
				chkShow.Checked = true;
				numShow.Value = Convert.ToDecimal(CurrentOrder.MaxFloor);
			}

			numLimitPrice.Value = Convert.ToDecimal(CurrentOrder.Price);
			numStopPrice.Value = Convert.ToDecimal(CurrentOrder.StopPrice);
			numPegPrice.Value = Convert.ToDecimal(CurrentOrder.PegPrice);
			switch (CurrentOrder.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Option:
					SetOptionPriceIncrement(numStopPrice, dspStopPriceTickSize);
					break;
				default:
					break;
			}

			orderMsg = string.Concat(new object[] { CurrentOrder.Tag, " ", CurrentOrder.LeaveQty, "@", CurrentOrder.Price, " ", CurrentOrder.StopPrice, " ", CurrentOrder.OmTime });
		}

		internal void SetOptionPriceIncrement(NumericUpDown num, LabelEx.LabelBase lblTick)
		{
			switch (CurrentOrder.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Option:
					if (CurrentSecInfo != null && CurrentOrder.IsOptionOnFuture)
					{
						num.Increment = CurrentSecInfo.TickSizeDec;
						lblTick.Value = CurrentSecInfo.TickSizeDec;
					}
					else
					{
						if (num.Value >= 3M)
						{
							num.Increment = Convert.ToDecimal(_stockLargeIncr);
							lblTick.Value = _stockLargeIncr;
						}
						else
						{
							num.Increment = Convert.ToDecimal(_stockSmallIncr);
							lblTick.Value = _stockSmallIncr;
						}
					}
					break;
			}
		}

		#region - Binding -

		private void SubscribeToSymbol(string symbolDetail)
		{
			UpdateSecurityInfo(symbolDetail);

			UpdateMarketDataDeltas(symbolDetail, GLOBAL.HMarketData.Current);
		}

		#region - Used By Process Thread -

		internal void UpdateOrderModificationTicketByProcess(bool updateIM, Market deltas)
		{
			try
			{
				if (updateIM)
				{
					UpdateSecurityInfo();
				}

				if (!deltas.Empty)
				{
					if (CurrentSecInfo != null)
					{
						UpdateMarketDataDeltas(CurrentSecInfo.MDSymbol, deltas);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void UpdateSecurityInfo()
		{
			Dictionary<string, string> lcoImSymbolNeeded = new Dictionary<string, string>();
			if (ImSymbolNeeded.Count > 0)
			{
				lock (ImSymbolNeeded)
				{
					lcoImSymbolNeeded = new Dictionary<string, string>(ImSymbolNeeded);
				}

				List<string> removeList = new List<string>();

				foreach (string symbolDetail in lcoImSymbolNeeded.Keys)
				{
					CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
					if (CurrentSecInfo != null)
					{
						UpdateIMInfo(symbolDetail, CurrentSecInfo);

						if (!removeList.Contains(symbolDetail))
						{
							removeList.Add(symbolDetail);
						}

						UpdateTicket();

						// Get the Snap Shot From Memory
						UpdateMarketDataDeltas(symbolDetail, GLOBAL.HMarketData.Current);
					}
				}

				// Clean Up
				if (removeList.Count > 0)
				{
					lock (ImSymbolNeeded)
					{
						foreach (string symbolDetail in removeList)
							ImSymbolNeeded.Remove(symbolDetail);
					}
				}
			}
		}

		private void UpdateIMInfo(string symbolDetail, BaseSecurityInfo secInfo)
		{
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Option:
					HelperSubscriber.SubscribeOptionNBBO(secInfo.MDSymbol, secInfo.MDSource);
					break;
				default:
					HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
					break;
			}

			if (!MDSymbols.Contains(secInfo.MDSymbol))
			{
				MDSymbols.Add(secInfo.MDSymbol);
			}
		}

		#endregion

		// Update the ticket with all current IM info
		private void UpdateSecurityInfo(string symbolDetail)
		{
			UpdateTicketWithSecurityInfo(symbolDetail);

			switch (CurrentOrder.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Option:
					GLOBAL.HRDS.GetOptionChain(CurrentOrder.Underlying);
					break;
				case CSVFieldIDs.SecurityTypes.SingleStockFuture:
					GLOBAL.HRDS.GetSSFutureChain(CurrentOrder.Underlying);
					break;
				default:
					GLOBAL.HRDS.GetSecurityByTicker(CurrentOrder.SymbolDetail);
					break;
			}
		}

		// Update with Security Info On Play back & onLoad
		private void UpdateTicketWithSecurityInfo(string symbolDetail)
		{
			CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
			if (CurrentSecInfo != null)
			{
				UpdateTicket();
			}

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[symbolDetail] = CurrentOrder.Symbol;
			}
		}

		// Update with Security Info On Play back & onProcess
		private void UpdateMarketDataDeltas(string symbolDetail, Market deltas)
		{
			// Update Level 1
			if (_isFull && deltas.TryGet(symbolDetail, out Book book))
			{
				UpdateMarketDataDelta(book);
			}
		}

		// Update with matching market data Level 1
		private void UpdateMarketDataDelta(Book delta)
		{
			double price, change;
			long size;

			lock (panelTicker)
			{
				panelTicker.SuspendLayout();

				if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
				{
					dspBidPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.BidSize, out size))
				{
					if (CurrentOrder.SecType == CSVFieldIDs.SecurityTypes.Equity)
					{
						dspBidSize.Value = size * 100;
					}
					else
					{
						dspBidSize.Value = size;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.AskPrice, out price))
				{
					dspAskPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.AskSize, out size))
				{
					if (CurrentOrder.SecType == CSVFieldIDs.SecurityTypes.Equity)
					{
						dspAskSize.Value = size * 100;
					}
					else
					{
						dspAskSize.Value = size;
					}
				}

				if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price))
				{
					dspTradedPrice.Value = price;
				}
				else if (delta.TryGetNonZero(Book.FieldEnum.ClosePrice, out price))
				{
					dspTradedPrice.Value = price;
				}
				else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price))
				{
					dspTradedPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size))
				{
					dspVolume.Value = size;
				}

				if (delta.TryGetNonEmpty(Book.FieldEnum.TradeTick, out string tradeTick))
				{
					switch (tradeTick)
					{
						case "+":
						case "+0":
							picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Up];
							break;
						case "-":
						case "-0":
							picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Down];
							break;
						default:
							// Leave the last tick state
							//picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.None];
							break;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.TradeVolume, out size))
				{
					dspTradeVolume.Value = size;
				}

				if (delta.TryGetField(Book.FieldEnum.NetChange, out change))
				{
					dspNetChange.Value = change;
					dspTradedPrice.ForeColor = dspNetChange.ForeColor;
				}

				if (delta.TryGetField(Book.FieldEnum.PctChange, out change))
				{
					dspPctChange.Value = change;
				}

				if (delta.TryGetField(Book.FieldEnum.HighPrice, out price))
				{
					dspHighPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.LowPrice, out price))
				{
					dspLowPrice.Value = price;
				}

				if (delta.TryGetNonZero(Book.FieldEnum.ClosePrice, out price))
				{
					dspClosePrice.Value = price;
				}
				else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price))
				{
					dspClosePrice.Value = price;
				}

				panelTicker.ResumeLayout();
			}
		}

		#endregion
	}
}
