using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using FormEx;
using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using CSVEx;
using ROMEx;
using RDSEx;
using System.Diagnostics;
using System.IO;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class BaseBatchTicket : BaseTicket, BaseIBatchTicket
	{
		#region - Base -

		private DGVBaseBatch _batchGrid;
		internal DGVBaseBatch BatchGrid
		{
			get
			{
				return _batchGrid;
			}
			set
			{
				_batchGrid = value;
				if (_batchGrid is ROCBatchMarket)
				{
					_batchGrid.IsMarketDataBatchGrid = true;
				}
			}
		}

		#endregion

		#region - Local Variable -

		// Used to store user info for faster lookup
		private Dictionary<string, RomBasicOrder> _validUserInfos = new Dictionary<string, RomBasicOrder>();

		private List<string> _orderIDs = new List<string>();
		internal List<string> orderIDs
		{
			get
			{
				return _orderIDs;
			}
		}

		#endregion

		#region - From Event -

		public virtual void AutoResetShow() { }

		#endregion

		#region - Special Paste && CSV Insert -

		public void PasteFromClipboard()
		{
			string clipboardString;
			string[] txtRows;

			lock (BatchGrid.RocGridTable)
			{
				StartInsert();

				clipboardString = Clipboard.GetText();

				txtRows = clipboardString.Split(new string[] { "\r\n" }, StringSplitOptions.None);
				foreach (string txtRow in txtRows)
				{
					if (txtRow != "")
					{
						InsertTextCells(txtRow.Split(new string[] { "\t" }, StringSplitOptions.None));
					}
				}

				EndInsert();
			}
		}

		public void InsertFromCSVFile(string[] csvItems)
		{
			lock (BatchGrid.RocGridTable)
			{
				StartInsert();

				ApplyCSVDefaultCellValue();

				foreach (string csvItem in csvItems)
				{
					if (csvItem.TrimEnd() != "")
					{
						InsertCSVCells(csvItem.Split(new string[] { "," }, StringSplitOptions.None));
					}
				}

				EndInsert();
			}
		}

		private void StartInsert()
		{
			if (BatchGrid.SelectedRows.Contains(BatchGrid.Rows[BatchGrid.NewRowIndex]))
			{
				BatchGrid.Rows[BatchGrid.NewRowIndex].Selected = false;
				BatchGrid.CancelEdit();

				BatchGrid.DataSource = BatchGrid.RocGridTable;
			}
		}

		private void EndInsert()
		{
			if (BatchGrid.RocGridTable.Rows.Count > 0)
			{
				BatchGrid.RefreshAggragation = true;
				BatchGrid.ShouldScrollToLastRow = true;
			}
		}

		// Used by paste
		private void InsertTextCells(string[] txtCells)
		{
			try
			{
				DataRow row = BatchGrid.RocGridTable.NewRow();
				long qty = 0;
				double price = 0;
				double stopPrice = 0;
				double pegPrice = 0;

				for (int i = 0; i < txtCells.Length; i++)
				{
					switch (i)
					{
						case 0:  // Symbol
							if (BatchGrid.IsMarketDataBatchGrid)
							{
								row["SymbolDetail"] = (string)txtCells[i];
							}
							else
							{
								row["Symbol"] = (string)txtCells[i];
							}
							break;
						case 1:  // Qty
							if (Int64.TryParse(txtCells[i].Replace(",", ""), out qty))
							{
								row["Qty"] = qty;
							}
							break;
						case 2:	// Side
							row["Side"] = (string)txtCells[i];
							break;
						case 3:  // Price
							if (Double.TryParse(txtCells[i].Replace(",", ""), out price))
							{
								row["Price"] = price;
							}
							break;
						case 4: // Exchange
							row["Exchange"] = (string)txtCells[i];
							break;
						case 5: // LocalAccountAcrn
							row["LocalAccountAcrn"] = (string)txtCells[i];
							break;
						case 6: // TraderFor
							row["TraderFor"] = (string)txtCells[i];
							break;
						case 7: // Display
							row["Display"] = (string)txtCells[i].Replace(",", "");
							break;
						case 8: // ShortLender
							row["ShortLender"] = (string)txtCells[i];
							break;
						case 9: // Duration
							row["Duration"] = (string)txtCells[i];
							break;
						case 10: // OrderType
							row["OrderType"] = (string)txtCells[i];
							break;
						case 11: // Instruction
							row["Instruction"] = (string)txtCells[i];
							break;
						case 12: // ExecInstruction
							row["ExecInstruction"] = (string)txtCells[i];
							break;
						case 13:  // StopPrice
							if (Double.TryParse(txtCells[i].Replace(",", ""), out stopPrice))
							{
								row["StopPrice"] = stopPrice;
							}
							break;
						case 14:  // PegPrice
							if (Double.TryParse(txtCells[i].Replace(",", ""), out pegPrice))
							{
								row["PegPrice"] = pegPrice;
							}
							break;
						case 15:  // Note
							row["Note"] = (string)txtCells[i];
							break;
						case 16:  // ProgramTrade
							row["ProgramTrade"] = (string)txtCells[i];
							break;
						default:
							break;
					}
				}
				BatchGrid.RocGridTable.Rows.Add(row);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		// User by CSV
		private void InsertCSVCells(string[] txtCells)
		{
			try
			{
				DataRow row = BatchGrid.RocGridTable.NewRow();
				long qty = 0;
				double price = 0;
				double stopPrice = 0;
				double pegPrice = 0;
				char[] filterOut = new char[] { '"' };

				for (int i = 0; i < txtCells.Length; i++)
				{
					if (HelperBatchCSV.CSVFormat.ContainsKey(i))
					{
						txtCells[i] = ((string)txtCells[i]).Trim(filterOut);

						switch (HelperBatchCSV.CSVFormat[i])
						{
							case "Symbol":
							case "SymbolDetail":
								if (BatchGrid.IsMarketDataBatchGrid)
								{
									row["SymbolDetail"] = (string)txtCells[i];
								}
								else
								{
									row["Symbol"] = (string)txtCells[i];
								}
								break;
							case "Qty":
								if (Int64.TryParse(txtCells[i].Replace(",", ""), out qty))
								{
									row["Qty"] = qty;
								}
								break;
							case "Side":
								row["Side"] = (string)txtCells[i];
								break;
							case "Price":
								if (Double.TryParse(txtCells[i].Replace(",", ""), out price))
								{
									row["Price"] = price;
								}
								break;
							case "Exchange":
								row["Exchange"] = (string)txtCells[i];
								break;
							case "LocalAccountAcrn":
								row["LocalAccountAcrn"] = (string)txtCells[i];
								break;
							case "TraderFor":
								row["TraderFor"] = (string)txtCells[i];
								break;
							case "Display":
								row["Display"] = (string)txtCells[i].Replace(",", "");
								break;
							case "ShortLender":
								row["ShortLender"] = (string)txtCells[i];
								break;
							case "Duration":
								row["Duration"] = (string)txtCells[i];
								break;
							case "OrderType":
								row["OrderType"] = (string)txtCells[i];
								break;
							case "Instruction":
								row["Instruction"] = (string)txtCells[i];
								break;
							case "ExecInstruction":
								row["ExecInstruction"] = (string)txtCells[i];
								break;
							case "StopPrice":
								if (Double.TryParse(txtCells[i].Replace(",", ""), out stopPrice))
								{
									row["StopPrice"] = stopPrice;
								}
								break;
							case "PegPrice":
								if (Double.TryParse(txtCells[i].Replace(",", ""), out pegPrice))
								{
									row["PegPrice"] = pegPrice;
								}
								break;
							case "Note":
								row["Note"] = (string)txtCells[i];
								break;
							case "ProgramTrade":
								row["ProgramTrade"] = (string)txtCells[i];
								break;
						}
					}
				}

				BatchGrid.RocGridTable.Rows.Add(row);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void ApplyCSVDefaultCellValue()
		{
			try
			{
				long qty = 0;
				double price = 0;
				double stopPrice = 0;
				double pegPrice = 0;

				foreach (string colName in HelperBatchCSV.CSVDefaultValue.Keys)
				{
					switch (colName)
					{
						case "Symbol":
						case "SymbolDetail":
							break;
						case "Qty":
							if (Int64.TryParse(HelperBatchCSV.CSVDefaultValue[colName], out qty))
							{
								BatchGrid.RocGridTable.Columns["Qty"].DefaultValue = qty;
							}
							break;
						case "Side":
							BatchGrid.RocGridTable.Columns["Side"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "Price":
							if (Double.TryParse(HelperBatchCSV.CSVDefaultValue[colName], out price))
							{
								BatchGrid.RocGridTable.Columns["Price"].DefaultValue = price;
							}
							break;
						case "Exchange":
							BatchGrid.RocGridTable.Columns["Exchange"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "LocalAccountAcrn":
							BatchGrid.RocGridTable.Columns["LocalAccountAcrn"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "TraderFor":
							BatchGrid.RocGridTable.Columns["TraderFor"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "Display":
							BatchGrid.RocGridTable.Columns["Display"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "ShortLender":
							BatchGrid.RocGridTable.Columns["ShortLender"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "Duration":
							BatchGrid.RocGridTable.Columns["Duration"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "OrderType":
							BatchGrid.RocGridTable.Columns["OrderType"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "Instruction":
							BatchGrid.RocGridTable.Columns["Instruction"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "ExecInstruction":
							BatchGrid.RocGridTable.Columns["ExecInstruction"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "StopPrice":
							if (Double.TryParse(HelperBatchCSV.CSVDefaultValue[colName], out stopPrice))
							{
								BatchGrid.RocGridTable.Columns["StopPrice"].DefaultValue = stopPrice;
							}
							break;
						case "PegPrice":
							if (Double.TryParse(HelperBatchCSV.CSVDefaultValue[colName], out pegPrice))
							{
								BatchGrid.RocGridTable.Columns["PegPrice"].DefaultValue = pegPrice;
							}
							break;
						case "Note":
							BatchGrid.RocGridTable.Columns["Note"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
						case "ProgramTrade":
							BatchGrid.RocGridTable.Columns["ProgramTrade"].DefaultValue = HelperBatchCSV.CSVDefaultValue[colName];
							break;
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Build Order -

		private List<RomBasicOrder> _orders = new List<RomBasicOrder>();

		internal void BuildOrder()
		{
			BuildOrder(false);
		}
		public virtual void BuildOrder(bool selectedOnly) { }

		#region - From Grid View -

		internal void BuildOrderList(int multiple, bool selectedOnly)
		{
			try
			{
				_orderIDs = new List<string>();
				_orders = new List<RomBasicOrder>();

				RomBasicOrder order;
				if (selectedOnly)
				{
					foreach (DataGridViewRow row in BatchGrid.SelectedRows)
					{
						if (ToRomOrder(multiple, row, out order))
						{
							_orderIDs.Add(order.orderID);
						}
					}
				}
				else
				{
					foreach (DataGridViewRow row in BatchGrid.Rows)
					{
						if (ToRomOrder(multiple, row, out order))
						{
							_orderIDs.Add(order.orderID);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		bool _isOption = false;
		private bool ToRomOrder(int multiple, DataGridViewRow row, out RomBasicOrder order)
		{
			order = new RomBasicOrder();

			if ((row.Cells["Status"].Value == null || row.Cells["Status"].Value == DBNull.Value || row.Cells["Status"].Value.ToString() == "") && (row.Cells["OrderID"].Value == null || row.Cells["OrderID"].Value == DBNull.Value || row.Cells["OrderID"].Value.ToString() == ""))
			{
				if (BatchGrid.IsMarketDataBatchGrid)
				{
					order.secType = GetValue(row, "SecType");
					order.symbol = GetValue(row, "SymbolDetail");
					if (order.symbol == "")
					{
						//SetOrderID(row, "Missing Detail", true);
						return false;
					}
					if (order.symbol.Contains(" "))
					{
						switch (order.secType)
						{
							case CSVFieldIDs.SecutrityTypes.Option:
							case CSVFieldIDs.SecutrityTypes.OptionFuture:
								break;
							default:
								SetOrderID(row, "Invalid Option", true);
								return false;
						}
					}
				}
				else
				{
					order.secType = "";
					order.symbol = GetValue(row, "Symbol");
					if (order.symbol == "")
					{
						//SetOrderID(row, "Missing Symbol", true);
						return false;
					}
					if (order.symbol.Contains(" "))
					{
						SetOrderID(row, "Use MDBach Ticket", true);
						return false;
					}
				}

				#region - Verify User Info -

				order.exchangeID = GetValue(row, "Exchange");
				order.tradeFor = GetValue(row, "TraderFor");
				if (order.exchangeID == "" || order.tradeFor == "")
				{
					SetOrderID(row, "Incomplete User Info", false);
					return false;
				}
				order.localAcctAcrn = GetValue(row, "LocalAccountAcrn");

				if (!VerifyTrader(ref order))
				{
					SetOrderID(row, "Invalid User Info", true);
					return false;
				}

				#endregion

				//if (BatchGrid.IsMarketDataBatchGrid)
				//{
				//    order.symbol = GetValue(row, "SymbolDetail");
				//    if (order.symbol == "")
				//    {
				//        SetOrderID(row, "Missing Detail", true);
				//        return false;
				//    }
				//}
				//else
				//{
				//    order.symbol = GetValue(row, "Symbol");
				//    if (order.symbol == "")
				//    {
				//        SetOrderID(row, "Missing Symbol", true);
				//        return false;
				//    }
				//}

				order.shares = GetValue(row, "Qty");
				if (order.shares == "" || order.shares == "0")
				{
					SetOrderID(row, "Missing or Zero Qty", true);
					return false;
				}

				#region - Verify Side -

				long side = GetSide(row);
				switch (side)
				{
					case -1:
						SetOrderID(row, "Invalid Side", true);
						return false;
					case CSVEx.CSVFieldIDs.SideCodes.Short:
						order.shortLender = GetValue(row, "ShortLender");
						if (order.shortLender == "")
						{
							SetOrderID(row, "Missing Short Lender", true);
							return false;
						}
						break;
				}
				order.side = side.ToString();

				#endregion

				order.instructions = GetValue(row, "Instruction");
				order.execInstructions = GetValue(row, "ExecInstruction");

				#region - Verify OrderType and Duration -

				long orderType = GetOrderType(row);
				switch (orderType)
				{
					case CSVEx.CSVFieldIDs.OrderTypes.Market:
					case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
						order.price = "";
						order.stopPrice = "";
						order.pegPrice = "";
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.StopLimit:
						order.price = GetValue(row, "Price");
						if (order.price == "")
						{
							SetOrderID(row, "Inavlid Price", true);
							return false;
						}
						order.stopPrice = GetValue(row, "StopPrice");
						if (order.stopPrice == "")
						{
							SetOrderID(row, "Inavlid Stop Price", true);
							return false;
						}
						order.pegPrice = "";
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.Stop:
						order.price = "";
						order.stopPrice = GetValue(row, "StopPrice");
						if (order.stopPrice == "")
						{
							SetOrderID(row, "Inavlid Stop Price", true);
							return false;
						}
						order.pegPrice = "";
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.PegMkt:
						order.price = "";
						order.stopPrice = "";
						order.pegPrice = GetValue(row, "PegPrice");
						order.execInstructions = CSVEx.CSVFieldIDs.PegOrderExecInstructions.MarketPeg;
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.PegPri:
						order.price = "";
						order.stopPrice = "";
						order.pegPrice = GetValue(row, "PegPrice");
						order.execInstructions = CSVEx.CSVFieldIDs.PegOrderExecInstructions.PrimaryPeg;
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.PegMid:
						order.price = "";
						order.stopPrice = "";
						order.pegPrice = GetValue(row, "PegPrice");
						order.execInstructions = CSVEx.CSVFieldIDs.PegOrderExecInstructions.MidpointPeg;
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.VWAPMKT:
						order.price = "";
						SetAlgoType(row, "VWAP");
						orderType = CSVEx.CSVFieldIDs.OrderTypes.Market;
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.GVWAP:
						order.price = "";
						SetAlgoType(row, "GVWAP");
						orderType = CSVEx.CSVFieldIDs.OrderTypes.Market;
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.VWAPLIM:
						SetAlgoType(row, "VWAP");
						order.price = GetValue(row, "Price");
						if (order.price == "")
						{
							SetOrderID(row, "Inavlid Price", true);
							return false;
						}
						orderType = CSVEx.CSVFieldIDs.OrderTypes.Limit;
						order.stopPrice = "";
						order.pegPrice = "";
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.IMBOI:
						order.price = GetValue(row, "Price");
						if (order.price == "")
						{
							SetOrderID(row, "Inavlid Price", true);
							return false;
						}
						if (order.instructions == "")
						{
							order.instructions = CSVEx.CSVFieldIDs.InstructionCmds.IOI;
						}
						order.displayInstruction = "I";
						orderType = CSVEx.CSVFieldIDs.OrderTypes.Limit;
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.IMBOO:
						order.price = GetValue(row, "Price");
						if (order.price == "")
						{
							SetOrderID(row, "Inavlid Price", true);
							return false;
						}
						if (order.instructions == "")
						{
							order.instructions = CSVEx.CSVFieldIDs.InstructionCmds.IOO;
						}
						order.displayInstruction = "I";
						orderType = CSVEx.CSVFieldIDs.OrderTypes.Limit;
						break;
					case CSVEx.CSVFieldIDs.OrderTypes.IMBOC:
						order.price = GetValue(row, "Price");
						if (order.price == "")
						{
							SetOrderID(row, "Inavlid Price", true);
							return false;
						}
						if (order.instructions == "")
						{
							order.instructions = CSVEx.CSVFieldIDs.InstructionCmds.IOC;
						}
						order.displayInstruction = "I";
						orderType = CSVEx.CSVFieldIDs.OrderTypes.Limit;
						break;
					default:
						order.price = GetValue(row, "Price");
						if (order.price == "")
						{
							SetOrderID(row, "Inavlid Price", true);
							return false;
						}
						order.stopPrice = "";
						order.pegPrice = "";
						break;
				}
				order.duration = GetDuration(row, orderType);
				order.orderType = orderType.ToString();

				#endregion

				order.show = GetValue(row, "Display");
				order.trader = GLOBAL.HROM.UserName;

				order.algoType = GetAlgoType(row);

				order.startTime = GetValue(row, "StartTime");
				order.endTime = GetValue(row, "EndTime");

				order.orderID = GetValue(row, "Note");

				order.programTrade = GetValue(row, "ProgramTrade");

				if (order.secType == "")
				{
					SetOrderID(row, "Missing SecType", true);
					return false;
				}

				_isOption = false;
				switch (order.secType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
						_isOption = true;
						if (order.secType != CSVFieldIDs.SecutrityTypes.Option)
						{
							SetOrderID(row, "Inavlid Account For Option", true);
							return false;
						}

						order.multiplier = "100";
						break;
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
						_isOption = true;
						if (order.secType != CSVFieldIDs.SecutrityTypes.OptionFuture && 
							order.secType != CSVFieldIDs.SecutrityTypes.Option &&
							order.secType != CSVFieldIDs.SecutrityTypes.Future)
						{
							SetOrderID(row, "Inavlid Account For Option On future", true);
							return false;
						}

						// Force secType to be option
						order.secType = CSVFieldIDs.SecutrityTypes.Option;
						order.multiplier = GetValue(row, "DisplayFactor");
						break;
					case CSVFieldIDs.SecutrityTypes.Future:
						#region - Verify Future -

						if (order.secType != CSVFieldIDs.SecutrityTypes.Future)
						{
							SetOrderID(row, "Inavlid Account For Future", true);
							return false;
						}

						if (BatchGrid.IsMarketDataBatchGrid)
						{
							order.multiplier = GetValue(row, "DisplayFactor");

							//order.expDate = GetValue(row, "ExpDate");
							//if (order.expDate == "")
							//{
							//	SetOrderID(row, "Inavlid ExpDate", true);
							//	return false;
							//}

							order.mdSymbol = GetValue(row, "Symbol");
							if (order.mdSymbol == "")
							{
								SetOrderID(row, "Inavlid MDSymbol", true);
								return false;
							}
						}

						#endregion
						break;
				}

				#region - Verify Option -

				if (_isOption)
				{
					order.openClose = GetValue(row, "Side");
					if (order.openClose.Contains("OPEN"))
					{
						order.openClose = CSVFieldIDs.OptionTypes.Open;
					}
					else if (order.openClose.Contains("CLOSE"))
					{
						order.openClose = CSVFieldIDs.OptionTypes.Close;
					}
					else
					{
						SetOrderID(row, "Inavlid OpenClose", true);
						return false;
					}

					order.callPut = GetValue(row, "CallPut");
					if (order.callPut == "C")
					{
						order.callPut = CSVFieldIDs.OptionTypes.Call;
					}
					else if (order.callPut == "P")
					{
						order.callPut = CSVFieldIDs.OptionTypes.Put;
					}
					else
					{
						SetOrderID(row, "Inavlid CallPut", true);
						return false;
					}

					order.expDate = GetValue(row, "ExpDate");
					if (order.expDate == "")
					{
						SetOrderID(row, "Inavlid ExpDate", true);
						return false;
					}
					order.maturityDay = GetValue(row, "MaturityDay");
					if (order.maturityDay == "")
					{
						SetOrderID(row, "Inavlid MaturityDay", true);
						return false;
					}
					order.strike = GetValue(row, "Strike");
					if (order.strike == "")
					{
						SetOrderID(row, "Inavlid Strike", true);
						return false;
					}
					order.mdSymbol = GetValue(row, "Symbol");
					if (order.mdSymbol == "")
					{
						SetOrderID(row, "Inavlid MDSymbol", true);
						return false;
					}

					order.underlying = GetValue(row, "Underlying");
					if (order.underlying == "")
					{
						SetOrderID(row, "Inavlid underlying", true);
						return false;
					}

					// Reset Symbol to Underlying
					order.symbol = order.underlying;
				}

				#endregion

				if (multiple == 1)
				{
					order.orderID = GLOBAL.HROM.GetOrderID(order.orderID);
					_orders.Add(order);
				}
				else
				{
					for (int i = 0; i < multiple; i++)
					{
						// Get the new OrderID for each orders
						order = new RomBasicOrder(order);
						order.orderID = string.Concat(GLOBAL.HROM.GetOrderID(), "-", i);
						_orders.Add(order);
					}
				}

				if (row.Cells["OrderID"].Value == null || row.Cells["OrderID"].Value == DBNull.Value || row.Cells["OrderID"].Value.ToString() == "")
				{
					// Set the Sent Status if there is not error on the row
					row.Cells["Status"].Value = 0;
				}

				SetOrderID(row, order.orderID, false);

				return true;
			}

			return false;
		}

		private string GetValue(DataGridViewRow row, string columnName)
		{
			try
			{
				if (row.Cells[columnName].Value != null && row.Cells[columnName].Value != DBNull.Value && row.Cells[columnName].Value.ToString().Trim() != "")
				{
					return row.Cells[columnName].Value.ToString().Trim().ToUpper();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return "";
		}

		private long GetSide(DataGridViewRow row)
		{
			switch (GetValue(row, "Side"))
			{
				case "SELL OPEN":
				case "SELL CLOSE":
				case "SELL":
				case "S":
					return CSVEx.CSVFieldIDs.SideCodes.Sell;
				case "BUY OPEN":
				case "BUY CLOSE":
				case "BUY":
				case "B":
					return CSVEx.CSVFieldIDs.SideCodes.Buy;
				case "SELL SHORT":
				case "SHORT":
				case "SS":
					return CSVEx.CSVFieldIDs.SideCodes.Short;
				default:
					return -1;
			}
		}

		private long GetOrderType(DataGridViewRow row)
		{
			switch (GetValue(row, "OrderType"))
			{
				case "MARKET":
				case "MKT":
					return CSVEx.CSVFieldIDs.OrderTypes.Market;
				case "STOP":
				case "STP":
					return CSVEx.CSVFieldIDs.OrderTypes.Stop;
				case "STOP LIMIT":
					return CSVEx.CSVFieldIDs.OrderTypes.StopLimit;
				case "MARKET ON CLOSE":
				case "MOC":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose;
				case "WITH OR WITHOUT":
					return CSVEx.CSVFieldIDs.OrderTypes.WithOrWithout;
				case "LIMIT OR BETTER":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitOrBetter;
				case "LIMIT WITH OR WITHOUT":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitWithOrWithout;
				case "ON BASIS":
					return CSVEx.CSVFieldIDs.OrderTypes.OnBasis;
				case "ON CLOSE":
					return CSVEx.CSVFieldIDs.OrderTypes.OnClose;
				case "LIMIT ON CLOSE":
				case "LOC":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitOnClose;
				case "ON MARKET AT OPEN":
				case "MOO":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen;
				case "VWAP":
					return CSVEx.CSVFieldIDs.OrderTypes.VWAP;
				case "PEG MKT":
					return CSVEx.CSVFieldIDs.OrderTypes.PegMkt;
				case "PEG PRI":
					return CSVEx.CSVFieldIDs.OrderTypes.PegPri;
				case "PEG MID":
					return CSVEx.CSVFieldIDs.OrderTypes.PegMid;
				case "VWAP MKT":
					return CSVEx.CSVFieldIDs.OrderTypes.VWAPMKT;
				case "VWAP LIM":
					return CSVEx.CSVFieldIDs.OrderTypes.VWAPLIM;
				case "GVWAP":
					return CSVEx.CSVFieldIDs.OrderTypes.GVWAP;
				case "IMBOC":
					return CSVEx.CSVFieldIDs.OrderTypes.IMBOC;
				case "IMBOI":
					return CSVEx.CSVFieldIDs.OrderTypes.IMBOI;
				case "IMBOO":
					return CSVEx.CSVFieldIDs.OrderTypes.IMBOO;
				case "LIMIT":
				default:
					row.Cells["OrderType"].Value = "LIMIT";
					return CSVEx.CSVFieldIDs.OrderTypes.Limit;
			}
		}

		private string GetDuration(DataGridViewRow row, long orderType)
		{
			switch (orderType)
			{
				case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					return CSVEx.CSVFieldIDs.TIFCodes.Day.ToString();
				case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
					return CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString();
				default:
					switch (GetValue(row, "Duration"))
					{
						case "DAY":
							return CSVEx.CSVFieldIDs.TIFCodes.Day.ToString();
						case "GTC":
							return CSVEx.CSVFieldIDs.TIFCodes.GTC.ToString();
						case "OPG":
							return CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString();
						case "IOC":
							return CSVEx.CSVFieldIDs.TIFCodes.IOC.ToString();
						case "FOK":
							return CSVEx.CSVFieldIDs.TIFCodes.FOK.ToString();
						case "GTX":
							return CSVEx.CSVFieldIDs.TIFCodes.GTX.ToString();
						case "GTD":
							return CSVEx.CSVFieldIDs.TIFCodes.GTD.ToString();
						default:
							row.Cells["Duration"].Value = "DAY";
							return CSVEx.CSVFieldIDs.TIFCodes.Day.ToString();
					}
			}
		}

		private string GetAlgoType(DataGridViewRow row)
		{
			switch (GetValue(row, "AlgoType"))
			{
				case "TWAP":
					return CSVEx.CSVFieldIDs.AlgoTypes.TWAP.ToString();
				case "VWAP":
					return CSVEx.CSVFieldIDs.AlgoTypes.VWAP.ToString();
				case "GVWAP":
					return CSVEx.CSVFieldIDs.AlgoTypes.GVWAP.ToString();
				default:
					return "";
			}
		}

		private void SetOrderID(DataGridViewRow row, string errorMsg, bool isError)
		{
			row.Cells["OrderID"].Value = errorMsg;
			if (isError)
			{
				row.Cells["Status"].Value = CSVFieldIDs.StatusCodes.Rejected;
				GLOBAL.HROC.AddToException(errorMsg);
			}
		}

		private void SetAlgoType(DataGridViewRow row, string type)
		{
			row.Cells["AlgoType"].Value = type;
		}

		#endregion

		#region - From Data Table -

		private void BuildOrderListFromDataTable(int multiple)
		{
			_orderIDs = new List<string>();
			_orders = new List<RomBasicOrder>();

			RomBasicOrder order;
			foreach (DataRow row in BatchGrid.RocGridTable.Rows)
			{
				if (row["Status"] == DBNull.Value && row["OrderID"] == DBNull.Value)
				{
					order = new RomBasicOrder();

					#region - Verify User Info -

					order.exchangeID = GetValueFromDataTable(row, "Exchange");
					order.tradeFor = GetValueFromDataTable(row, "TraderFor");
					if (order.exchangeID == "" || order.tradeFor == "")
					{
						SetOrderIDFromDataTable(row, "Incomplete User Info");
						continue;
					}
					order.localAcctAcrn = GetValueFromDataTable(row, "LocalAccountAcrn");

					if (!VerifyTrader(ref order))
					{
						SetOrderIDFromDataTable(row, "Invalid User Info");
						continue;
					}

					#endregion

					order.symbol = GetValueFromDataTable(row, "Symbol");
					if (order.symbol == "")
					{
						SetOrderIDFromDataTable(row, "Missing Symbol");
						continue;
					}

					order.shares = GetValueFromDataTable(row, "Qty");
					if (order.shares == "")
					{
						SetOrderIDFromDataTable(row, "Missing Qty");
						continue;
					}

					long side = GetSideFromDataTable(row);
					switch (side)
					{
						case -1:
							SetOrderIDFromDataTable(row, "Invalid Side");
							continue;
						case CSVEx.CSVFieldIDs.SideCodes.Short:
							order.shortLender = GetValueFromDataTable(row, "ShortLender");
							if (order.shortLender == "")
							{
								SetOrderIDFromDataTable(row, "Missing Short Lender");
								continue;
							}
							break;
					}
					order.side = side.ToString();

					long orderType = GetOrderTypeFromDataTable(row);
					switch (orderType)
					{
						case CSVEx.CSVFieldIDs.OrderTypes.Market:
						case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
						case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
							order.price = "";
							break;
						default:
							order.price = GetValueFromDataTable(row, "Price");
							if (order.price == "")
							{
								SetOrderIDFromDataTable(row, "Inavlid Price");
								continue;
							}
							break;
					}
					order.duration = GetDurationFromDataTable(row, orderType);
					order.orderType = orderType.ToString();

					order.show = GetValueFromDataTable(row, "Display");
					order.trader = GLOBAL.HROM.UserName;

					if (multiple == 1)
					{
						order.orderID = GLOBAL.HROM.GetOrderID();
						_orders.Add(order);
					}
					else
					{
						for (int i = 0; i < multiple; i++)
						{
							// Get the new OrderID for each orders
							order = new RomBasicOrder(order);
							order.orderID = GLOBAL.HROM.GetOrderID();
							_orders.Add(order);
						}
					}

					if (row["OrderID"] == null || row["OrderID"].ToString() == "")
					{
						// Set the Sent Status if there is not error on the row
						row["Status"] = 0;
					}

					SetOrderIDFromDataTable(row, order.orderID);

					_orderIDs.Add(order.orderID);
				}
			}
		}

		private string GetValueFromDataTable(DataRow row, string columnName)
		{
			if (row[columnName] != null && row[columnName].ToString().Trim() != "")
			{
				return row[columnName].ToString().Trim().ToUpper();
			}
			else
			{
				return "";
			}
		}

		private long GetSideFromDataTable(DataRow row)
		{
			switch (GetValueFromDataTable(row, "Side"))
			{
				case "SELL":
				case "S":
					return CSVEx.CSVFieldIDs.SideCodes.Sell;
				case "BUY":
				case "B":
					return CSVEx.CSVFieldIDs.SideCodes.Buy;
				case "SELL SHORT":
				case "SHORT":
				case "SS":
					return CSVEx.CSVFieldIDs.SideCodes.Short;
				default:
					return -1;
			}
		}

		private long GetOrderTypeFromDataTable(DataRow row)
		{
			switch (GetValueFromDataTable(row, "OrderType"))
			{
				case "MARKET":
				case "MKT":
					return CSVEx.CSVFieldIDs.OrderTypes.Market;
				case "STOP":
					return CSVEx.CSVFieldIDs.OrderTypes.Stop;
				case "STOP LIMIT":
					return CSVEx.CSVFieldIDs.OrderTypes.StopLimit;
				case "MARKET ON CLOSE":
				case "MOC":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose;
				case "WITH OR WITHOUT":
					return CSVEx.CSVFieldIDs.OrderTypes.WithOrWithout;
				case "LIMIT OR BETTER":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitOrBetter;
				case "LIMIT WITH OR WITHOUT":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitWithOrWithout;
				case "ON BASIS":
					return CSVEx.CSVFieldIDs.OrderTypes.OnBasis;
				case "ON CLOSE":
					return CSVEx.CSVFieldIDs.OrderTypes.OnClose;
				case "LIMIT ON CLOSE":
				case "LOC":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitOnClose;
				case "ON MARKET AT OPEN":
				case "MOO":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen;
				case "VWAP":
					return CSVEx.CSVFieldIDs.OrderTypes.VWAP;
				case "MARKET PEG":
					return CSVEx.CSVFieldIDs.OrderTypes.PegMkt;
				case "PRIMARY PEG":
					return CSVEx.CSVFieldIDs.OrderTypes.PegPri;
				case "MIDPOINT PEG":
					return CSVEx.CSVFieldIDs.OrderTypes.PegMid;
				case "LIMIT":
				default:
					row["OrderType"] = "LIMIT";
					return CSVEx.CSVFieldIDs.OrderTypes.Limit;
			}
		}

		private string GetDurationFromDataTable(DataRow row, long orderType)
		{
			switch (orderType)
			{
				case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					return CSVEx.CSVFieldIDs.TIFCodes.Day.ToString();
				case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
					return CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString();
				default:
					switch (GetValueFromDataTable(row, "Duration"))
					{
						case "DAY":
							return CSVEx.CSVFieldIDs.TIFCodes.Day.ToString();
						case "GTC":
							return CSVEx.CSVFieldIDs.TIFCodes.GTC.ToString();
						case "OPG":
							return CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString();
						case "IOC":
							return CSVEx.CSVFieldIDs.TIFCodes.IOC.ToString();
						case "FOK":
							return CSVEx.CSVFieldIDs.TIFCodes.FOK.ToString();
						case "GTX":
							return CSVEx.CSVFieldIDs.TIFCodes.GTX.ToString();
						case "GTD":
							return CSVEx.CSVFieldIDs.TIFCodes.GTD.ToString();
						default:
							row["Duration"] = "DAY";
							return CSVEx.CSVFieldIDs.TIFCodes.Day.ToString();
					}
			}
		}

		private void SetOrderIDFromDataTable(DataRow row, string errorMsg)
		{
			row["OrderID"] = errorMsg;
			GLOBAL.HROC.AddToStatusLogs(errorMsg);
		}

		#endregion

		internal void SendOrderList()
		{
			foreach (RomBasicOrder order in _orders)
			{
				GLOBAL.HROM.EnterOrder(order, false);
			}
		}

		private bool VerifyTrader(ref RomBasicOrder order)
		{
			if (GLOBAL.HRDS.GotUserProfiles)
			{
				string key = MakeUserInfoKey(order);
				if (_validUserInfos.ContainsKey(key))
				{
					order.capacity = _validUserInfos[key].capacity;
					order.clearingAcctID = _validUserInfos[key].clearingAcctID;
					order.clearingFirmID = _validUserInfos[key].clearingFirmID;
					order.firmArc = _validUserInfos[key].firmArc;
					order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

					order.exchangeID = _validUserInfos[key].exchangeID;
					if (order.secType == "")
					{
						order.secType = _validUserInfos[key].secType;
					}

					return true;
				}
				else
				{
					// Search for a match
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor == order.tradeFor)
						{
							switch (order.secType)
							{
								case CSVFieldIDs.SecutrityTypes.Option:
									if (VerifyExchange(ref order, trader.OPTAccounts))
									{
										return true;
									}
									break;
								case CSVFieldIDs.SecutrityTypes.OptionFuture:
									if (VerifyExchange(ref order, trader.OPTAccounts))
									{
										return true;
									}
									if (VerifyExchange(ref order, trader.FUTAccounts))
									{
										return true;
									}
									break;
								case CSVFieldIDs.SecutrityTypes.Future:
									if (VerifyExchange(ref order, trader.FUTAccounts))
									{
										return true;
									}
									break;
								default:
									if (VerifyExchange(ref order, trader.CSAccounts))
									{
										return true;
									}

									if (VerifyExchange(ref order, trader.FUTAccounts))
									{
										return true;
									}

									if (VerifyExchange(ref order, trader.OPTAccounts))
									{
										return true;
									}
									break;
							}
						}
					}
				}
			}
			return false;
		}

		private bool VerifyExchange(ref RomBasicOrder order, Dictionary<string, AccountMap> accounts)
		{
			foreach (AccountMap acctMap in accounts.Values)
			{
				if (order.localAcctAcrn == "")
				{
					// User didn't specify an account, use the first destination found
					foreach (DestinationMap destMap in acctMap.Destinations.Values)
					{
						if (destMap.shortName == order.exchangeID)
						{
							StoreUserInfo(ref order, acctMap, destMap);
							return true;
						}
					}
				}
				else
				{
					if (acctMap.account == order.localAcctAcrn)
					{
						foreach (DestinationMap destMap in acctMap.Destinations.Values)
						{
							if (destMap.shortName == order.exchangeID)
							{
								StoreUserInfo(ref order, acctMap, destMap);
								return true;
							}
						}
					}
				}
			}
			return false;
		}

		private void StoreUserInfo(ref RomBasicOrder order, AccountMap acctMap, DestinationMap destMap)
		{
			order.capacity = acctMap.capacity;
			order.clearingAcctID = acctMap.clearingAcID;
			order.clearingFirmID = acctMap.clearingFirmID;
			order.firmArc = acctMap.firmAcr;
			order.localAcctAcrn = acctMap.localAcAcrn;
			order.occAcct = acctMap.occAcr;
			if (acctMap.homeExchange != null)
			{
				order.homeExchange = acctMap.homeExchange;
			}

			if (destMap.destID != null)
			{
				order.exchangeID = destMap.destID.ToString();
				order.cmtaFirmID = destMap.cmtaFirmID;
				if (order.secType == "")
				{
					order.secType = destMap.secType;
				}
			}

			string key = order.tradeFor + "," + order.localAcctAcrn + "," + destMap.shortName;
			if (_validUserInfos.ContainsKey(key))
			{
				_validUserInfos[key] = order;
			}
			else
			{
				_validUserInfos.Add(key, order);
			}
		}

		private string MakeUserInfoKey(RomBasicOrder order)
		{
			// Starting with exchangeID as the destination short name
			return order.tradeFor + "," + order.localAcctAcrn + "," + order.exchangeID;
		}

		#endregion

		#region - Export & Import -

		public virtual string ExportGrid() { return ""; }

		public virtual void ImportGrid(string s) { }

		#endregion

		#region - CSV Load -

		internal void LoadCSVFrom()
		{
			HelperBatchCSV.Load();

			OpenFileDialog ofd = new OpenFileDialog();
			ofd.DefaultExt = "csv";
			ofd.Filter = "Profile File (*.csv)|*.csv";
			ofd.AddExtension = true;
			ofd.RestoreDirectory = true;
			ofd.Title = "Open Profile From";
			if (Configuration.User.Default.LastCSVFile == "")
			{
				ofd.InitialDirectory = "C:";
			}
			else
			{
				try
				{
					ofd.InitialDirectory = Path.GetDirectoryName(Configuration.User.Default.LastCSVFile);
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
					ofd.InitialDirectory = "C:";
				}
			}

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				foreach (string filename in ofd.FileNames)
				{
					Configuration.User.Default.LastCSVFile = filename;
					InsertFromCSVFile(HelperBatchCSV.LoadCSV(Path.GetDirectoryName(filename), Path.GetFileName(filename)));
				}

				Configuration.User.Default.Save();
			}
		}

		#endregion

		#region - Columns Menu -

		private menuBaseFilter _menuColumns;
		private PropertyContainer _menuColumnsContainer;
		internal void ShowColumnMenu()
		{
			if (_menuColumns == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();
				items.Add("AlgoType", new FilterItem("AlgoType", !BatchGrid.Columns["AlgoType"].Visible));
				items.Add("ExecInstruction", new FilterItem("ExecInstruction", !BatchGrid.Columns["ExecInstruction"].Visible));
				items.Add("StopPrice", new FilterItem("StopPrice", !BatchGrid.Columns["StopPrice"].Visible));
				items.Add("PegPrice", new FilterItem("PegPrice", !BatchGrid.Columns["PegPrice"].Visible));
				items.Add("Note", new FilterItem("Note", !BatchGrid.Columns["Note"].Visible));
				items.Add("ProgramTrade", new FilterItem("ProgramTrade", !BatchGrid.Columns["ProgramTrade"].Visible));

				if (BatchGrid.IsMarketDataBatchGrid)
				{
					items.Add("Symbol", new FilterItem("Symbol", !BatchGrid.Columns["Symbol"].Visible));
					items.Add("Underlying", new FilterItem("Underlying", !BatchGrid.Columns["Underlying"].Visible));
					items.Add("BidPrice", new FilterItem("BidPrice", !BatchGrid.Columns["BidPrice"].Visible));
					items.Add("AskPrice", new FilterItem("AskPrice", !BatchGrid.Columns["AskPrice"].Visible));
					items.Add("LastTraded", new FilterItem("LastTraded", !BatchGrid.Columns["LastTraded"].Visible));
					items.Add("TickSize", new FilterItem("TickSize", !BatchGrid.Columns["TickSize"].Visible));
					items.Add("DisplayFactor", new FilterItem("DisplayFactor", !BatchGrid.Columns["DisplayFactor"].Visible));
					items.Add("SecType", new FilterItem("SecType", !BatchGrid.Columns["SecType"].Visible));
					items.Add("CallPut", new FilterItem("CallPut", !BatchGrid.Columns["CallPut"].Visible));
					items.Add("ExpDate", new FilterItem("ExpDate", !BatchGrid.Columns["ExpDate"].Visible));
					items.Add("MaturityDay", new FilterItem("MaturityDay", !BatchGrid.Columns["MaturityDay"].Visible));
					items.Add("Strike", new FilterItem("Strike", !BatchGrid.Columns["Strike"].Visible));
				}

				_menuColumns = new menuBaseFilter(items, true);
				_menuColumns.PropertyChanged += new PropertyChangedEventHandler(menuColumns_PropertyChanged);
			}

			if (_menuColumnsContainer == null)
			{
				_menuColumnsContainer = new PropertyContainer(_menuColumns);
				_menuColumnsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuColumnsContainer_Closed);
			}

			_menuColumnsContainer.Show(Cursor.Position);
		}

		private void menuColumns_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "AlgoType":
					BatchGrid.Columns["AlgoType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					BatchGrid.Columns["StartTime"].Visible = BatchGrid.Columns["AlgoType"].Visible;
					BatchGrid.Columns["EndTime"].Visible = BatchGrid.Columns["AlgoType"].Visible;
					break;
				case "ExecInstruction":
					BatchGrid.Columns["ExecInstruction"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "StopPrice":
					BatchGrid.Columns["StopPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "PegPrice":
					BatchGrid.Columns["PegPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Note":
					BatchGrid.Columns["Note"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "ProgramTrade":
					BatchGrid.Columns["ProgramTrade"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Symbol":
					BatchGrid.Columns["Symbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidPrice":
					BatchGrid.Columns["BidPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskPrice":
					BatchGrid.Columns["AskPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "LastTraded":
					BatchGrid.Columns["LastTraded"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TickSize":
					BatchGrid.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DisplayFactor":
					BatchGrid.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecType":
					BatchGrid.Columns["SecType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "CallPut":
					BatchGrid.Columns["CallPut"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "ExpDate":
					BatchGrid.Columns["ExpDate"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "MaturityDay":
					BatchGrid.Columns["MaturityDay"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Strike":
					BatchGrid.Columns["Strike"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Underlying":
					BatchGrid.Columns["Underlying"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
			}
		}

		private void menuColumnsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuColumnsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuColumnsContainer_Closed);
			_menuColumnsContainer = null;
			_menuColumns.PropertyChanged -= new PropertyChangedEventHandler(menuColumns_PropertyChanged);
			_menuColumns = null;
		}

		#endregion

		#region - Row Edit Menu -

		private menuBaseAction _menuGridActions;
		private PropertyContainer _menuGridActionsContainer;
		internal void ShowGridMenu()
		{
			if (_menuGridActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("Insert", "Insert");
				items.Add("Delete", "Delete");
				items.Add("DeleteSelected", "Delete Selected");
				items.Add("Clear", "Clear");
				items.Add("Paste", "Paste");
				//items.Add("ExecuteAll", "ExecuteAll");

				_menuGridActions = new menuBaseAction(items, true);
				_menuGridActions.PropertyChanged += new PropertyChangedEventHandler(menuGridActions_PropertyChanged);
			}

			if (_menuGridActionsContainer == null)
			{
				_menuGridActionsContainer = new PropertyContainer(_menuGridActions);
				_menuGridActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuGridActionsContainer_Closed);
			}

			_menuGridActionsContainer.Show(Cursor.Position);
		}

		private void menuGridActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Insert":
					AddDataRow();
					break;
				case "Delete":
					DeleteDataRow();
					break;
				case "DeleteSelected":
					DeleteSelectedDataRow();
					break;
				case "Clear":
					ClearDataRow();
					break;
				case "Paste":
					PasteFromClipboard();
					break;
				//case "ExecuteAll":
				//    BuildOrder();
				//    break;
			}

			_menuGridActionsContainer.Close();
		}

		private void menuGridActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuGridActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuGridActionsContainer_Closed);
			_menuGridActionsContainer = null;
			_menuGridActions.PropertyChanged -= new PropertyChangedEventHandler(menuGridActions_PropertyChanged);
			_menuGridActions = null;
		}

		#endregion

		#region - Row Status Menu -

		private menuBaseAction _menuStatusActions;
		private PropertyContainer _menuStatusActionsContainer;
		internal void ShowStatusMenu()
		{
			if (_menuStatusActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("Reset", "Reset");
				items.Add("ResetRejectedOnly", "ResetRejectedOnly");
				items.Add("Insert", "Insert");
				items.Add("Delete", "Delete");
				items.Add("Clear", "Clear");
				items.Add("Paste", "Paste");
				items.Add("ExecuteAll", "ExecuteAll");

				_menuStatusActions = new menuBaseAction(items, true);
				_menuStatusActions.PropertyChanged += new PropertyChangedEventHandler(menuStatusActions_PropertyChanged);
			}

			if (_menuStatusActionsContainer == null)
			{
				_menuStatusActionsContainer = new PropertyContainer(_menuStatusActions);
				_menuStatusActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuStatusActionsContainer_Closed);
			}

			_menuStatusActionsContainer.Show(Cursor.Position);
		}

		private void menuStatusActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Reset":
					ResetDataRow();
					break;
				case "ResetRejectedOnly":
					ResetDataRow(true);
					break;
				case "Insert":
					AddDataRow();
					break;
				case "Delete":
					DeleteDataRow();
					break;
				case "Clear":
					ClearDataRow();
					break;
				case "Paste":
					PasteFromClipboard();
					break;
				case "ExecuteAll":
					BuildOrder();
					break;
			}

			_menuStatusActionsContainer.Close();
		}

		private void menuStatusActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuStatusActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuStatusActionsContainer_Closed);
			_menuStatusActionsContainer = null;
			_menuStatusActions.PropertyChanged -= new PropertyChangedEventHandler(menuStatusActions_PropertyChanged);
			_menuStatusActions = null;
		}

		#endregion

		#region - Row Side Menu -

		private menuBaseAction _menuSideActions;
		private PropertyContainer _menuSideActionsContainer;
		internal void ShowSideMenu()
		{
			if (_menuSideActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("BUY", "BUY");
				items.Add("SELL", "SELL");
				items.Add("SHORT", "SHORT");
				items.Add("ALL BUY", "ALL BUY");
				items.Add("ALL SELL", "ALL SELL");
				items.Add("ALL SHORT", "ALL SHORT");

				_menuSideActions = new menuBaseAction(items, true);
				_menuSideActions.PropertyChanged += new PropertyChangedEventHandler(menuSideActions_PropertyChanged);
			}

			if (_menuSideActionsContainer == null)
			{
				_menuSideActionsContainer = new PropertyContainer(_menuSideActions);
				_menuSideActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuSideActionsContainer_Closed);
			}

			_menuSideActionsContainer.Show(Cursor.Position);
		}

		private void menuSideActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "BUY":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "BUY", true);
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "BUY", true);
					}
					break;
				case "ALL BUY":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "BUY");
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "BUY");
					}
					break;
				case "SELL":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SELL", true);
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SELL", true);
					}
					break;
				case "ALL SELL":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SELL");
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SELL");
					}
					break;
				case "SHORT":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SHORT", true);
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SHORT", true);
					}
					break;
				case "ALL SHORT":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SHORT");
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SHORT");
					}
					break;
			}

			_menuSideActionsContainer.Close();
		}

		private void menuSideActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuSideActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuSideActionsContainer_Closed);
			_menuSideActionsContainer = null;
			_menuSideActions.PropertyChanged -= new PropertyChangedEventHandler(menuSideActions_PropertyChanged);
			_menuSideActions = null;
		}

		#endregion

		#region - Row TraderFor Menu -

		private menuBaseAction _menuTraderActions;
		private PropertyContainer _menuTraderActionsContainer;
		internal void ShowTraderMenu()
		{
			if (_menuTraderActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					items.Add(trader.tradeFor.ToUpper(), trader.tradeFor.ToUpper());
				}

				_menuTraderActions = new menuBaseAction(items, true);
				_menuTraderActions.PropertyChanged += new PropertyChangedEventHandler(menuTraderActions_PropertyChanged);
			}

			if (_menuTraderActionsContainer == null)
			{
				_menuTraderActionsContainer = new PropertyContainer(_menuTraderActions);
				_menuTraderActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuTraderActionsContainer_Closed);
			}

			_menuTraderActionsContainer.Show(Cursor.Position);
		}

		private void menuTraderActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			bool found = false;

			RomBasicOrder order = new RomBasicOrder();

			order.tradeFor = e.PropertyName;

			// Get the current account for selected row
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value != null)
			{
				order.localAcctAcrn = BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString();
			}
			else
			{
				order.localAcctAcrn = "";
			}

			// Get the current destination short name for selected row
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value != null)
			{
				order.exchangeID = BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value.ToString();
			}
			else
			{
				order.exchangeID = "";
			}

			string key = MakeUserInfoKey(order);
			if (_validUserInfos.ContainsKey(key))
			{
				order.capacity = _validUserInfos[key].capacity;
				order.clearingAcctID = _validUserInfos[key].clearingAcctID;
				order.clearingFirmID = _validUserInfos[key].clearingFirmID;
				order.firmArc = _validUserInfos[key].firmArc;
				order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

				order.exchangeID = _validUserInfos[key].exchangeID;
				found = true;
			}
			else
			{
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (trader.tradeFor == e.PropertyName)
					{
						if (VerifyExchange(ref order, trader.CSAccounts) || VerifyExchange(ref order, trader.FUTAccounts) || VerifyExchange(ref order, trader.OPTAccounts))
						{
							found = true;
							break;
						}
					}
				}
			}

			// Update the TradeFor and clear all account and exchange field
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value == null)
			{
				ReplaceValue("TraderFor", "", order.tradeFor);
			}
			else
			{
				ReplaceValue("TraderFor", BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value.ToString(), order.tradeFor);
			}

			if (!found)
			{
				// Update the TradeFor and clear all account and exchange field
				if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value == null)
				{
					ReplaceValue("LocalAccountAcrn", "", "");
				}
				else
				{
					ReplaceValue("LocalAccountAcrn", BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString(), "");
				}

				// Update the TradeFor and clear all account and exchange field
				if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value == null)
				{
					ReplaceValue("Exchange", "", "");
				}
				else
				{
					ReplaceValue("Exchange", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value.ToString(), "");
				}
			}

			_menuTraderActionsContainer.Close();
		}

		private void menuTraderActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuTraderActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuTraderActionsContainer_Closed);
			_menuTraderActionsContainer = null;
			_menuTraderActions.PropertyChanged -= new PropertyChangedEventHandler(menuTraderActions_PropertyChanged);
			_menuTraderActions = null;
		}

		#endregion

		#region - Row Local Account Acrn Menu -

		private menuBaseAction _menuAccountActions;
		private PropertyContainer _menuAccountActionsContainer;
		internal void ShowAccountMenu()
		{
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value != null &&
				BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value.ToString() != "")
			{
				if (_menuAccountActions == null)
				{
					Dictionary<string, string> items = new Dictionary<string, string>();
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor.ToUpper() == BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value.ToString().ToUpper())
						{
							foreach (AccountMap acctMap in trader.CSAccounts.Values)
							{
								if (!items.ContainsKey(acctMap.account))
								{
									items.Add(acctMap.account, acctMap.account);
								}
							}

							foreach (AccountMap acctMap in trader.FUTAccounts.Values)
							{
								if (!items.ContainsKey(acctMap.account))
								{
									items.Add(acctMap.account, acctMap.account);
								}
							}

							if (BatchGrid.IsMarketDataBatchGrid)
							{
								foreach (AccountMap acctMap in trader.OPTAccounts.Values)
								{
									if (!items.ContainsKey(acctMap.account))
									{
										items.Add(acctMap.account, acctMap.account);
									}
								}
							}

							break;
						}
					}

					_menuAccountActions = new menuBaseAction(items, true);
					_menuAccountActions.PropertyChanged += new PropertyChangedEventHandler(menuAccountActions_PropertyChanged);
				}

				if (_menuAccountActionsContainer == null)
				{
					_menuAccountActionsContainer = new PropertyContainer(_menuAccountActions);
					_menuAccountActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuAccountActionsContainer_Closed);
				}

				_menuAccountActionsContainer.Show(Cursor.Position);
			}
		}

		private void menuAccountActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			bool found = false;

			RomBasicOrder order = new RomBasicOrder();

			// Get the current account for selected row
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value != null)
			{
				order.tradeFor = BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value.ToString();
			}
			else
			{
				order.tradeFor = "";
			}

			// Load Account into localAcctAcrn first, then it will be replaced by the Correct LocalAcctAcrn
			order.localAcctAcrn = e.PropertyName;

			// Get the current destination short name for selected row
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value != null)
			{
				order.exchangeID = BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value.ToString();
			}
			else
			{
				order.exchangeID = "";
			}

			string key = MakeUserInfoKey(order);
			if (_validUserInfos.ContainsKey(key))
			{
				order.capacity = _validUserInfos[key].capacity;
				order.clearingAcctID = _validUserInfos[key].clearingAcctID;
				order.clearingFirmID = _validUserInfos[key].clearingFirmID;
				order.firmArc = _validUserInfos[key].firmArc;
				order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

				order.exchangeID = _validUserInfos[key].exchangeID;
				found = true;
			}
			else
			{
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (trader.tradeFor == e.PropertyName)
					{
						if (VerifyExchange(ref order, trader.CSAccounts) || VerifyExchange(ref order, trader.FUTAccounts) || VerifyExchange(ref order, trader.OPTAccounts))
						{
							found = true;
							break;
						}
					}
				}
			}

			// Update the TradeFor and clear all account and exchange field
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value == null)
			{
				ReplaceValue("LocalAccountAcrn", "", order.localAcctAcrn);
			}
			else
			{
				ReplaceValue("LocalAccountAcrn", BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString(), order.localAcctAcrn);
			}

			if (!found)
			{
				// Update the TradeFor and clear all account and exchange field
				if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value == null)
				{
					ReplaceValue("Exchange", "", "");
				}
				else
				{
					ReplaceValue("Exchange", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value.ToString(), "");
				}
			}

			_menuAccountActionsContainer.Close();
		}

		private void menuAccountActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuAccountActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuAccountActionsContainer_Closed);
			_menuAccountActionsContainer = null;
			_menuAccountActions.PropertyChanged -= new PropertyChangedEventHandler(menuAccountActions_PropertyChanged);
			_menuAccountActions = null;
		}

		#endregion

		#region - Row Short Lender Menu -

		private menuBaseAction _menuShortLender;
		private PropertyContainer _menuShortLenderContainer;
		internal void ShowShortLenderMenu()
		{
			if (BatchGrid.ShortLender != "")
			{
				if (_menuShortLender == null)
				{
					Dictionary<string, string> items = new Dictionary<string, string>();
					items.Add("Selected", "Selected To: " + BatchGrid.ShortLender);
					items.Add("All", "All To: " + BatchGrid.ShortLender);
					items.Add("Clear", "Clear");

					_menuShortLender = new menuBaseAction(items, true);
					_menuShortLender.PropertyChanged += new PropertyChangedEventHandler(menuShortLender_PropertyChanged);
				}

				if (_menuShortLenderContainer == null)
				{
					_menuShortLenderContainer = new PropertyContainer(_menuShortLender);
					_menuShortLenderContainer.Closed += new ToolStripDropDownClosedEventHandler(menuShortLenderContainer_Closed);
				}

				_menuShortLenderContainer.Show(Cursor.Position);
			}
		}

		private void menuShortLender_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Selected":
					ReplaceShortLenderValue(BatchGrid.ShortLender, true);
					break;
				case "All":
					ReplaceShortLenderValue(BatchGrid.ShortLender, false);
					break;
				case "Clear":
					ReplaceShortLenderValue("", false);
					break;
			}

			_menuShortLenderContainer.Close();
		}

		private void menuShortLenderContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuShortLenderContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuShortLenderContainer_Closed);
			_menuShortLenderContainer = null;
			_menuShortLender.PropertyChanged -= new PropertyChangedEventHandler(menuShortLender_PropertyChanged);
			_menuShortLender = null;
		}

		#endregion

		#region - Row Exchange Menu -

		private menuBaseAction _menuExchangeActions;
		private PropertyContainer _menuExchangeActionsContainer;
		internal void ShowExchangeMenu()
		{
			if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value != null &&
				BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value.ToString() != "" &&
				BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value != null &&
				BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString() != "")
			{
				if (_menuExchangeActions == null)
				{
					Dictionary<string, string> items = new Dictionary<string, string>();
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor.ToUpper() == BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value.ToString().ToUpper())
						{
							foreach (AccountMap acctMap in trader.CSAccounts.Values)
							{
								if (acctMap.account.ToUpper() == BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString().ToUpper())
								{
									foreach (DestinationMap destMap in acctMap.Destinations.Values)
									{
										if (!items.ContainsKey(destMap.shortName))
										{
											items.Add(destMap.shortName, destMap.shortName);
										}
									}
								}
							}

							foreach (AccountMap acctMap in trader.FUTAccounts.Values)
							{
								if (acctMap.account.ToUpper() == BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString().ToUpper())
								{
									foreach (DestinationMap destMap in acctMap.Destinations.Values)
									{
										if (!items.ContainsKey(destMap.shortName))
										{
											items.Add(destMap.shortName, destMap.shortName);
										}
									}
								}
							}

							foreach (AccountMap acctMap in trader.OPTAccounts.Values)
							{
								if (acctMap.account.ToUpper() == BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString().ToUpper())
								{
									foreach (DestinationMap destMap in acctMap.Destinations.Values)
									{
										if (!items.ContainsKey(destMap.shortName))
										{
											items.Add(destMap.shortName, destMap.shortName);
										}
									}
								}
							}

							break;
						}
					}

					_menuExchangeActions = new menuBaseAction(items, true);
					_menuExchangeActions.PropertyChanged += new PropertyChangedEventHandler(menuExchangeActions_PropertyChanged);
				}

				if (_menuExchangeActionsContainer == null)
				{
					_menuExchangeActionsContainer = new PropertyContainer(_menuExchangeActions);
					_menuExchangeActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuExchangeActionsContainer_Closed);
				}

				_menuExchangeActionsContainer.Show(Cursor.Position);
			}
		}

		private void menuExchangeActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			bool found = false;

			RomBasicOrder order = new RomBasicOrder();

			try
			{
				// Get the current account for selected row
				if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value != null)
				{
					order.tradeFor = BatchGrid.Rows[BatchGrid.RowLocation].Cells["TraderFor"].Value.ToString();
				}
				else
				{
					order.tradeFor = "";
				}

				// Get the current account for selected row
				if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value != null)
				{
					order.localAcctAcrn = BatchGrid.Rows[BatchGrid.RowLocation].Cells["LocalAccountAcrn"].Value.ToString();
				}
				else
				{
					order.localAcctAcrn = "";
				}

				order.exchangeID = e.PropertyName;

				string key = MakeUserInfoKey(order);
				if (_validUserInfos.ContainsKey(key))
				{
					order.capacity = _validUserInfos[key].capacity;
					order.clearingAcctID = _validUserInfos[key].clearingAcctID;
					order.clearingFirmID = _validUserInfos[key].clearingFirmID;
					order.firmArc = _validUserInfos[key].firmArc;
					order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

					//order.exchangeID = _validUserInfos[key].exchangeID;
					found = true;
				}
				else
				{
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor == e.PropertyName)
						{
							if (VerifyExchange(ref order, trader.CSAccounts) || VerifyExchange(ref order, trader.FUTAccounts) || VerifyExchange(ref order, trader.OPTAccounts))
							{
								found = true;
								break;
							}
						}
					}
				}

				// Update the TradeFor and clear all account and exchange field
				if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value == null)
				{
					ReplaceValue("Exchange", "", order.exchangeID);
				}
				else
				{
					if (BatchGrid.IsMarketDataBatchGrid)
					{
						ReplaceValue("Exchange", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value.ToString(), BatchGrid.Rows[BatchGrid.RowLocation].Cells["SecType"].Value.ToString(), order.exchangeID);
					}
					else
					{
						ReplaceValue("Exchange", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Exchange"].Value.ToString(), order.exchangeID);
					}
				}

				if (!found)
				{
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			_menuExchangeActionsContainer.Close();
		}

		private void menuExchangeActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuExchangeActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuExchangeActionsContainer_Closed);
			_menuExchangeActionsContainer = null;
			_menuExchangeActions.PropertyChanged -= new PropertyChangedEventHandler(menuExchangeActions_PropertyChanged);
			_menuExchangeActions = null;
		}

		#endregion

		#region - Row Duration Menu -

		private menuBaseAction _menuDurationActions;
		private PropertyContainer _menuDurationActionsContainer;
		internal void ShowDurationMenu()
		{
			if (_menuDurationActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("DAY", "DAY");
				items.Add("GTC", "GTC");
				items.Add("OPG", "OPG");
				items.Add("IOC", "IOC");
				//items.Add("FOK", "FOK");
				//items.Add("GTX", "GTX");
				items.Add("GTD", "GTD");

				_menuDurationActions = new menuBaseAction(items, true);
				_menuDurationActions.PropertyChanged += new PropertyChangedEventHandler(menuDurationActions_PropertyChanged);
			}

			if (_menuDurationActionsContainer == null)
			{
				_menuDurationActionsContainer = new PropertyContainer(_menuDurationActions);
				_menuDurationActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuDurationActionsContainer_Closed);
			}

			_menuDurationActionsContainer.Show(Cursor.Position);
		}

		private void menuDurationActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "DAY":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value == null)
					{
						ReplaceValue("Duration", "", "DAY");
					}
					else
					{
						ReplaceValue("Duration", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value.ToString(), "DAY");
					}
					break;
				case "GTC":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value == null)
					{
						ReplaceValue("Duration", "", "GTC");
					}
					else
					{
						ReplaceValue("Duration", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value.ToString(), "GTC");
					}
					break;
				case "OPG":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value == null)
					{
						ReplaceValue("Duration", "", "OPG");
					}
					else
					{
						ReplaceValue("Duration", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value.ToString(), "OPG");
					}
					break;
				case "IOC":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value == null)
					{
						ReplaceValue("Duration", "", "IOC");
					}
					else
					{
						ReplaceValue("Duration", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value.ToString(), "IOC");
					}
					break;
				case "FOK":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value == null)
					{
						ReplaceValue("Duration", "", "FOK");
					}
					else
					{
						ReplaceValue("Duration", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value.ToString(), "FOK");
					}
					break;
				case "GTX":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value == null)
					{
						ReplaceValue("Duration", "", "GTX");
					}
					else
					{
						ReplaceValue("Duration", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value.ToString(), "GTX");
					}
					break;
				case "GTD":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value == null)
					{
						ReplaceValue("Duration", "", "GTD");
					}
					else
					{
						ReplaceValue("Duration", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Duration"].Value.ToString(), "GTD");
					}
					break;
			}

			_menuDurationActionsContainer.Close();
		}

		private void menuDurationActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuDurationActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuDurationActionsContainer_Closed);
			_menuDurationActionsContainer = null;
			_menuDurationActions.PropertyChanged -= new PropertyChangedEventHandler(menuDurationActions_PropertyChanged);
			_menuDurationActions = null;
		}

		#endregion

		#region - Row OrderType Menu -

		private menuBaseAction _menuOrderTypeActions;
		private PropertyContainer _menuOrderTypeActionsContainer;
		internal void ShowOrderTypeMenu()
		{
			if (_menuOrderTypeActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("LIMIT", "Limit");
				items.Add("MKT", "Market");
				items.Add("MOO", "Market On Open");
				items.Add("MOC", "Market On Close");
				items.Add("LOC", "Limit On Close");
				items.Add("PEG MKT", "Market Peg");
				items.Add("PEG PRI", "Primary Peg");
				items.Add("PEG MID", "Midpoint Peg");
				items.Add("IMBOO", "Imbalance Only Open");
				items.Add("IMBOC", "Imbalance Only Close");
				items.Add("IMBOI", "Imbalance Only Intraday");
				items.Add("VWAP MKT", "VWAP Market");
				items.Add("VWAP LIM", "VWAP Limit");
				items.Add("GVWAP", "GVWAP");

				_menuOrderTypeActions = new menuBaseAction(items, true);
				_menuOrderTypeActions.PropertyChanged += new PropertyChangedEventHandler(menuOrderTypeActions_PropertyChanged);
			}

			if (_menuOrderTypeActionsContainer == null)
			{
				_menuOrderTypeActionsContainer = new PropertyContainer(_menuOrderTypeActions);
				_menuOrderTypeActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuOrderTypeActionsContainer_Closed);
			}

			_menuOrderTypeActionsContainer.Show(Cursor.Position);
		}

		private void menuOrderTypeActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "LIMIT":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "LIMIT");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "LIMIT");
					}
					break;
				case "MKT":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "MKT");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "MKT");
					}
					break;
				case "MOO":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "MOO");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "MOO");
					}
					break;
				case "MOC":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "MOC");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "MOC");
					}
					break;
				case "LOC":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "LOC");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "LOC");
					}
					break;
				case "PEG MKT":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "PEG MKT");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "PEG MKT");
					}
					break;
				case "PEG PRI":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "PEG PRI");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "PEG PRI");
					}
					break;
				case "PEG MID":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "PEG MID");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "PEG MID");
					}
					break;
				case "IMBOO":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "IMBOO");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "IMBOO");
					}
					break;
				case "IMBOC":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "IMBOC");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "IMBOC");
					}
					break;
				case "IMBOI":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "IMBOI");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "IMBOI");
					}
					break;
				case "VWAP MKT":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "VWAP MKT");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "VWAP MKT");
					}
					break;
				case "VWAP LIM":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "VWAP LIM");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "VWAP LIM");
					}
					break;
				case "GVWAP":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value == null)
					{
						ReplaceValue("OrderType", "", "GVWAP");
					}
					else
					{
						ReplaceValue("OrderType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["OrderType"].Value.ToString(), "GVWAP");
					}
					break;
			}

			_menuOrderTypeActionsContainer.Close();
		}

		private void menuOrderTypeActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuOrderTypeActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuOrderTypeActionsContainer_Closed);
			_menuOrderTypeActionsContainer = null;
			_menuOrderTypeActions.PropertyChanged -= new PropertyChangedEventHandler(menuOrderTypeActions_PropertyChanged);
			_menuOrderTypeActions = null;
		}

		#endregion

		#region - Row AlgoType Menu -

		private menuBaseAction _menuAlgoTypeActions;
		private PropertyContainer _menuAlgoTypeActionsContainer;
		internal void ShowAlgoTypeMenu()
		{
			if (_menuAlgoTypeActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("", "");
				items.Add("TWAP", "TWAP");
				items.Add("VWAP", "VWAP");

				_menuAlgoTypeActions = new menuBaseAction(items, true);
				_menuAlgoTypeActions.PropertyChanged += new PropertyChangedEventHandler(menuAlgoTypeActions_PropertyChanged);
			}

			if (_menuAlgoTypeActionsContainer == null)
			{
				_menuAlgoTypeActionsContainer = new PropertyContainer(_menuAlgoTypeActions);
				_menuAlgoTypeActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuAlgoTypeActionsContainer_Closed);
			}

			_menuAlgoTypeActionsContainer.Show(Cursor.Position);
		}

		private void menuAlgoTypeActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "TWAP":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["AlgoType"].Value == null)
					{
						ReplaceValue("AlgoType", "", "TWAP");
					}
					else
					{
						ReplaceValue("AlgoType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["AlgoType"].Value.ToString(), "TWAP");
					}
					break;
				case "VWAP":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["AlgoType"].Value == null)
					{
						ReplaceValue("AlgoType", "", "VWAP");
					}
					else
					{
						ReplaceValue("AlgoType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["AlgoType"].Value.ToString(), "VWAP");
					}
					break;
				case "":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["AlgoType"].Value == null)
					{
						ReplaceValue("AlgoType", "", "");
					}
					else
					{
						ReplaceValue("AlgoType", BatchGrid.Rows[BatchGrid.RowLocation].Cells["AlgoType"].Value.ToString(), "");
					}
					break;
			}

			_menuAlgoTypeActionsContainer.Close();
		}

		private void menuAlgoTypeActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuAlgoTypeActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuAlgoTypeActionsContainer_Closed);
			_menuAlgoTypeActionsContainer = null;
			_menuAlgoTypeActions.PropertyChanged -= new PropertyChangedEventHandler(menuAlgoTypeActions_PropertyChanged);
			_menuAlgoTypeActions = null;
		}

		#endregion

		#region - Row StartTime Menu -

		private itemBaseDateTimePicker _menuStartTimePicker;
		private PropertyContainer _menuStartTimePickerContainer;
		internal void ShowStartTimeMenu()
		{
			if (_menuStartTimePicker == null)
			{
				_menuStartTimePicker = new itemBaseDateTimePicker();
				_menuStartTimePicker.PropertyChanged += new PropertyChangedEventHandler(menuStartTimePicker_PropertyChanged);
			}

			if (_menuStartTimePickerContainer == null)
			{
				_menuStartTimePickerContainer = new PropertyContainer(_menuStartTimePicker);
				_menuStartTimePickerContainer.Closed += new ToolStripDropDownClosedEventHandler(menuStartTimePickerContainer_Closed);
			}

			_menuStartTimePickerContainer.Show(Cursor.Position);
		}

		private void menuStartTimePicker_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Ok":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["StartTime"].Value == null)
					{
						ReplaceValue("StartTime", "", _menuStartTimePicker.SelectedTime);
					}
					else
					{
						ReplaceValue("StartTime", BatchGrid.Rows[BatchGrid.RowLocation].Cells["StartTime"].Value.ToString(), _menuStartTimePicker.SelectedTime);
					}
					break;
				case "Exit":
					break;
			}

			_menuStartTimePickerContainer.Close();
		}

		private void menuStartTimePickerContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuStartTimePickerContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuStartTimePickerContainer_Closed);
			_menuStartTimePickerContainer = null;
			_menuStartTimePicker.PropertyChanged -= new PropertyChangedEventHandler(menuStartTimePicker_PropertyChanged);
			_menuStartTimePicker = null;
		}

		#endregion

		#region - Row EndTime Menu -

		private itemBaseDateTimePicker _menuEndTimePicker;
		private PropertyContainer _menuEndTimePickerContainer;
		internal void ShowEndTimeMenu()
		{
			if (_menuEndTimePicker == null)
			{
				_menuEndTimePicker = new itemBaseDateTimePicker();
				_menuEndTimePicker.PropertyChanged += new PropertyChangedEventHandler(menuEndTimePicker_PropertyChanged);
			}

			if (_menuEndTimePickerContainer == null)
			{
				_menuEndTimePickerContainer = new PropertyContainer(_menuEndTimePicker);
				_menuEndTimePickerContainer.Closed += new ToolStripDropDownClosedEventHandler(menuEndTimePickerContainer_Closed);
			}

			_menuEndTimePickerContainer.Show(Cursor.Position);
		}

		private void menuEndTimePicker_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Ok":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["EndTime"].Value == null)
					{
						ReplaceValue("EndTime", "", _menuEndTimePicker.SelectedTime);
					}
					else
					{
						ReplaceValue("EndTime", BatchGrid.Rows[BatchGrid.RowLocation].Cells["EndTime"].Value.ToString(), _menuEndTimePicker.SelectedTime);
					}
					break;
				case "Exit":
					break;
			}

			_menuEndTimePickerContainer.Close();
		}

		private void menuEndTimePickerContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuEndTimePickerContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuEndTimePickerContainer_Closed);
			_menuEndTimePickerContainer = null;
			_menuEndTimePicker.PropertyChanged -= new PropertyChangedEventHandler(menuEndTimePicker_PropertyChanged);
			_menuEndTimePicker = null;
		}

		#endregion

		#region - Row Menu Support -

		internal void ReplaceValue(string colName, string orgValue, string orgSecType, string newValue)
		{
			try
			{
				lock (BatchGrid)
				{
					orgValue = orgValue.ToUpper();
					orgSecType = orgSecType.ToUpper();
					newValue = newValue.ToUpper();

					foreach (DataGridViewRow row in BatchGrid.Rows)
					{
						ReplaceValue(row, colName, orgValue, orgSecType, newValue);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}
		internal void ReplaceValue(DataGridViewRow row, string colName, string orgValue, string orgSecType, string newValue)
		{
			string currentSecType = "";
			if (BatchGrid.IsMarketDataBatchGrid)
			{
				if (row.Cells["SecType"].Value != null && row.Cells["SecType"].Value.ToString() != "")
				{
					currentSecType = row.Cells["SecType"].Value.ToString().ToUpper();
				}
			}

			string currentValue = "";
			if (row.Cells[colName].Value != null && row.Cells[colName].Value.ToString() != "")
			{
				currentValue = row.Cells[colName].Value.ToString().ToUpper();
			}

			if (currentSecType != "")
			{
				if (currentValue != "")
				{
					if (orgValue == currentValue && orgSecType == currentSecType)
					{
						row.Cells[colName].Value = newValue;
					}
				}
				else
				{
					if (orgValue == "" && orgSecType == currentSecType)
					{
						row.Cells[colName].Value = newValue;
					}
				}
			}
			else
			{
				if (currentValue != "")
				{
					if (orgValue == currentValue)
					{
						row.Cells[colName].Value = newValue;
					}
				}
				else
				{
					if (orgValue == "")
					{
						row.Cells[colName].Value = newValue;
					}
				}
			}
		}

		internal void ReplaceValue(string colName, string orgValue, string newValue)
		{
			ReplaceValue(colName, orgValue, newValue, false);
		}
		internal void ReplaceValue(string colName, string orgValue, string newValue, bool selectedRowsOnly)
		{
			try
			{
				lock (BatchGrid)
				{
					orgValue = orgValue.ToUpper();
					newValue = newValue.ToUpper();

					if (selectedRowsOnly)
					{
						foreach (DataGridViewRow row in BatchGrid.SelectedRows)
						{
							ReplaceValue(row, colName, orgValue, newValue);
						}
					}
					else
					{
						foreach (DataGridViewRow row in BatchGrid.Rows)
						{
							ReplaceValue(row, colName, orgValue, newValue);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}
		internal void ReplaceValue(DataGridViewRow row, string colName, string orgValue, string newValue)
		{
			if (row.Cells[colName].Value != null && row.Cells[colName].Value.ToString() != "")
			{
				if (orgValue == row.Cells[colName].Value.ToString().ToUpper())
				{
					row.Cells[colName].Value = newValue;
				}
			}
			else
			{
				if (orgValue == "")
				{
					row.Cells[colName].Value = newValue;
				}
			}
		}

		private void ReplaceShortLenderValue(string shortLender, bool selectedOnly)
		{
			try
			{
				lock (BatchGrid)
				{
					shortLender = shortLender.ToUpper();

					if (selectedOnly)
					{
						foreach (DataGridViewRow row in BatchGrid.SelectedRows)
						{
							ReplaceShortLenderValue(row, shortLender);
						}
					}
					else
					{
						foreach (DataGridViewRow row in BatchGrid.Rows)
						{
							ReplaceShortLenderValue(row, shortLender);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}
		private void ReplaceShortLenderValue(DataGridViewRow row, string shortLender)
		{
			if (row.Cells["Side"].Value != null && row.Cells["Side"].Value.ToString() != "")
			{
				if (row.Cells["Side"].Value.ToString().ToUpper() == "SHORT")
				{
					row.Cells["ShortLender"].Value = shortLender;
				}
			}
		}

		private void ReplaceOrderTypeValue(string orgValue, string newValue)
		{
			try
			{
				lock (BatchGrid.RocGridTable)
				{
					string colName = "OrderType";
					orgValue = orgValue.ToUpper();
					newValue = newValue.ToUpper();

					foreach (DataRow row in BatchGrid.RocGridTable.Rows)
					{
						if (row[colName] != null && row[colName].ToString() != "")
						{
							if (orgValue == row[colName].ToString().ToUpper())
							{
								row[colName] = newValue;
							}
						}
						else
						{
							if (orgValue == "")
							{
								row[colName] = newValue;
							}
						}

						switch (newValue)
						{
							case "MKT":
							case "MOC":
								row["Price"] = DBNull.Value;
								break;
							case "MOO":
								row["Price"] = DBNull.Value;
								row["Duration"] = "OPG";
								break;
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		internal void ResetDataRow()
		{
			ResetDataRow(false, false);
		}
		private void ResetDataRow(bool rejectedOnly)
		{
			ResetDataRow(false, rejectedOnly);
		}
		private void ResetDataRow(bool selectedOnly, bool rejectedOnly)
		{
			try
			{
				lock (BatchGrid)
				{
					if (selectedOnly)
					{
						foreach (DataGridViewRow row in BatchGrid.SelectedRows)
						{
							if (!rejectedOnly || (rejectedOnly && IsRejected(row)))
							{
								row.Cells["Status"].Value = DBNull.Value;
								row.Cells["OrderID"].Value = DBNull.Value;
							}
						}
					}
					else
					{
						foreach (DataGridViewRow row in BatchGrid.Rows)
						{
							if (!rejectedOnly || (rejectedOnly && IsRejected(row)))
							{
								row.Cells["Status"].Value = DBNull.Value;
								row.Cells["OrderID"].Value = DBNull.Value;
							}
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private bool IsRejected(DataGridViewRow row)
		{
			try
			{
				if (row.Cells["Status"].Value != null && row.Cells["Status"].Value.ToString() != "" && (long)row.Cells["Status"].Value == CSVEx.CSVFieldIDs.StatusCodes.Rejected)
				{
					return true;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
			return false;
		}

		private void AddDataRow()
		{
			try
			{
				lock (BatchGrid.RocGridTable)
				{
					BatchGrid.RocGridTable.Rows.InsertAt(BatchGrid.RocGridTable.NewRow(), BatchGrid.RowLocation);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void DeleteDataRow()
		{
			try
			{
				lock (BatchGrid.RocGridTable)
				{
					if (BatchGrid.RocGridTable.Rows.Count > BatchGrid.RowLocation)
					{
						BatchGrid.RocGridTable.Rows.RemoveAt(BatchGrid.RowLocation);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void DeleteSelectedDataRow()
		{
			try
			{
				lock (BatchGrid.RocGridTable)
				{
					foreach (DataGridViewRow row in BatchGrid.SelectedRows)
					{
						BatchGrid.Rows.Remove(row);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public virtual void ClearDataRow() { }

		#endregion
	}
}
