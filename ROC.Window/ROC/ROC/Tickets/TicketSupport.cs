using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace ROC
{
	public static class TicketSupport
	{
		private static string[] _exchangeSupportExtendedMarketOrderType = Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		private static string[] _exchangeSupportExtendedLimitOrderType = Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		private static string[] _exchangeSupportStopOrderType = Configuration.ROC.Default.ExchangesSupportStopOrderType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		private static string[] _exchangeSupportLimitStopOrderType = Configuration.ROC.Default.ExchangesSupportLimitStopOrderType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);

		private static string[]	_exchangeSupportPegMktOrderType = Configuration.ROC.Default.ExchangesSupportPegMktOrderType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		private static string[]	_exchangeSupportPegPriOrderType = Configuration.ROC.Default.ExchangesSupportPegPriOrderType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		private static string[] _exchangeSupportPegMidOrderType = Configuration.ROC.Default.ExchangesSupportPegMidOrderType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);

		//private static string[] _exchangeSupportVWAPType = Configuration.ROC.Default.ExchangeSupportVWAPType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		private static string[] _exchangeSupportVWAPType = HelperOrderTypeByExchange.ExchangeSupportVWAPType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		private static string[] _exchangeSupportGWAPType = HelperOrderTypeByExchange.ExchangeSupportGWAPType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);
		
		private static string[] _exchangeSupportImbalanceOnlyType = Configuration.ROC.Default.ExchangesSupportImbalanceOnlyType.ToUpper().Trim().Split(new char[] { ',' } , StringSplitOptions.RemoveEmptyEntries);

		public static bool SetState(Label obj, bool err)
		{
			if (obj != null)
			{
				if (err)
				{
					obj.ForeColor = Color.Red;
				}
				else
				{
					obj.ForeColor = Color.Black;
				}
			}

			return err;
		}
		public static bool SetState(NumericUpDown obj, bool err)
		{
			if (obj != null)
			{
				if (err)
				{
					obj.ForeColor = Color.Red;
				}
				else
				{
					obj.ForeColor = Color.Black;
				}
			}

			return err;
		}
		public static bool SetState(ComboBox obj, bool err)
		{
			if (obj != null)
			{
				if (err)
				{
					obj.ForeColor = Color.Red;
				}
				else
				{
					obj.ForeColor = Color.Black;
				}
			}

			return err;
		}
		public static bool SetState(CheckBox obj, bool err)
		{
			if (obj != null)
			{
				if (err)
				{
					obj.ForeColor = Color.Red;
				}
				else
				{
					obj.ForeColor = Color.Black;
				}
			}

			return err;
		}
		public static bool SetState(ToolStripDropDownButton obj, bool err)
		{
			if (obj != null)
			{
				if (err)
				{
					obj.ForeColor = Color.Red;
				}
				else
				{
					obj.ForeColor = Color.Black;
				}
			}

			return err;
		}

		public static string BuildOrderTypeFilter(ComboBox cboExchange, ComboBox cboOrder)
		{
			return BuildOrderTypeFilter(cboExchange, cboOrder, "");
		}
		public static string BuildOrderTypeFilter(ComboBox cboExchange, ComboBox cboOrder, string defaultTypeFilter)
		{
			string orderTypeFilter = defaultTypeFilter;
			bool isSupported = false;

			foreach (string exchange in _exchangeSupportVWAPType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type Like 'VWAP%'");
				foreach (string exchange in _exchangeSupportGWAPType)
				{
					if (cboExchange.Text.Contains(exchange))
					{
						orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type Like 'GVWAP%'", "OR");
						break;
					}
				}
				return orderTypeFilter;
			}
			else
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'VWAP MKT' And Type <> 'VWAP LIM'And Type <> 'GVWAP'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportImbalanceOnlyType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'IMBOC' And Type <> 'IMBOO' And Type <> 'IMBOI'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportExtendedMarketOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'MOC' And Type <> 'MOO'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportExtendedLimitOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'LOC'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportStopOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'STOP'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportLimitStopOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'STOPLIMIT'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportPegMktOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'PEG MKT'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportPegPriOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'PEG PRI'");
			}

			isSupported = false;
			foreach (string exchange in _exchangeSupportPegMidOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				orderTypeFilter = BuilderFilterString(orderTypeFilter, "Type <> 'PEG MID'");
			}

			if (orderTypeFilter.Contains(string.Format("'{0}'", cboOrder.Text)))
			{
				cboOrder.Text = "LIMIT";
			}

			return orderTypeFilter;
		}

		public static string BuildDurationTypeFilter(ComboBox cboExchange)
		{
			string durationFilter = "";

			bool isSupported = false;
			foreach (string exchange in _exchangeSupportExtendedLimitOrderType)
			{
				if (cboExchange.Text.Contains(exchange))
				{
					isSupported = true;
					break;
				}
			}
			if (!isSupported)
			{
				durationFilter = BuilderFilterString(durationFilter, "Duration <> 'OPG'");
			}

			return durationFilter;
		}

		private static string BuilderFilterString(string filter, string newFilter)
		{
			return BuilderFilterString(filter, newFilter, "AND");
		}
		private static string BuilderFilterString(string filter, string newFilter, string link)
		{
			if (filter != "")
			{
				filter = string.Concat(filter, " " + link + " " + newFilter);
			}
			else
			{
				filter = newFilter;
			}

			return filter;
		}
	}
}
