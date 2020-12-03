using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;

namespace ROC
{
	public static class HelperSettings
	{
		private static DataTable _userTable;
		private static DataTable userTable
		{
			get
			{
				if (_userTable == null)
				{
					_userTable = new DataTable("Settings");
					_userTable.Columns.Add("Key");
					_userTable.Columns.Add("Val");
				}
				return _userTable;
			}
			set
			{
				_userTable = value;
			}
		}

		private static string _filename = "Settings.xml";

		public static void Load()
		{
			try
			{
				userTable = HelperFile.Load(userTable, Configuration.Path.Default.SettingPath, _filename);
				GLOBAL.HLog.ROC.Out("Loading [Setting] From Current Location " + Configuration.Path.Default.SettingPath + " File Name " + _filename);
				
				if (userTable.Rows.Count == 0)
				{
					if (!Configuration.Path.Default.SettingPath.Contains(Configuration.Path.Default.ProfilePath))
					{
						// Check New Location Under Profild Folder
						// Put the settings into the Profile Folder
						Configuration.Path.Default.SettingPath = Configuration.Path.Default.ProfilePath + @"Settings\";
						Configuration.Path.Default.Save();
						userTable = HelperFile.Load(userTable, Configuration.Path.Default.SettingPath, _filename);

						GLOBAL.HLog.ROC.Out("Loading [Setting] From New Location " + Configuration.Path.Default.SettingPath + " File Name " + _filename);
					}
					else
					{
						// Already Using New Location, but didn't find any, try old location again
						userTable = HelperFile.Load(userTable, @"..\Settings\", _filename);

						GLOBAL.HLog.ROC.Out("Loading [Setting] From Default Location " + @"..\Settings\" + " File Name " + _filename);
					}
				}

				foreach (DataRow row in userTable.Rows)
				{
					GLOBAL.HLog.ROC.Out(string.Format("Updated {0} = {1}", new object[] { row["Key"].ToString(), row["Val"].ToString() }));

					switch (row["Key"].ToString())
					{
						case "ROC.ExchangesSupportExtendedMarketOrderType":
							if (Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType != HelperOrderTypeByExchange.ExchangesSupportExtendedMarketOrderType)
							{
								GLOBAL.HLog.ROC.Out(string.Format("Override {0} = {1}", new object[] { row["Key"].ToString(), HelperOrderTypeByExchange.ExchangesSupportExtendedMarketOrderType }));
								Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType = HelperOrderTypeByExchange.ExchangesSupportExtendedMarketOrderType;
								//Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType = row["Val"].ToString();
								Configuration.ROC.Default.Save();
							}
							break;
						case "ROC.ExchangesSupportExtendedLimitOrderType":
							if (Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType != HelperOrderTypeByExchange.ExchangesSupportExtendedLimitOrderType)
							{
								GLOBAL.HLog.ROC.Out(string.Format("Override {0} = {1}", new object[] { row["Key"].ToString(), HelperOrderTypeByExchange.ExchangesSupportExtendedLimitOrderType }));
								Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType = HelperOrderTypeByExchange.ExchangesSupportExtendedLimitOrderType;
								//Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType = row["Val"].ToString();
								Configuration.ROC.Default.Save();
							}
							break;
						case "ROC.ExchangesSupportStopOrderType":
							if (Configuration.ROC.Default.ExchangesSupportStopOrderType != HelperOrderTypeByExchange.ExchangesSupportStopOrderType)
							{
								GLOBAL.HLog.ROC.Out(string.Format("Override {0} = {1}", new object[] { row["Key"].ToString(), HelperOrderTypeByExchange.ExchangesSupportStopOrderType }));
								Configuration.ROC.Default.ExchangesSupportStopOrderType = HelperOrderTypeByExchange.ExchangesSupportStopOrderType;
								//Configuration.ROC.Default.ExchangesSupportStopOrderType = row["Val"].ToString();
								Configuration.ROC.Default.Save();
							}
							break;
						case "ROC.ExchangesSupportLimitStopOrderType":
							if (Configuration.ROC.Default.ExchangesSupportLimitStopOrderType != HelperOrderTypeByExchange.ExchangesSupportLimitStopOrderType)
							{
								GLOBAL.HLog.ROC.Out(string.Format("Override {0} = {1}", new object[] { row["Key"].ToString(), HelperOrderTypeByExchange.ExchangesSupportLimitStopOrderType }));
								Configuration.ROC.Default.ExchangesSupportLimitStopOrderType = HelperOrderTypeByExchange.ExchangesSupportLimitStopOrderType;
								//Configuration.ROC.Default.ExchangesSupportLimitStopOrderType = row["Val"].ToString();
								Configuration.ROC.Default.Save();
							}
							break;
						case "ROC.ExchangesSupportPegMidOrderType":
							if (Configuration.ROC.Default.ExchangesSupportPegMidOrderType != HelperOrderTypeByExchange.ExchangesSupportPegMidOrderType)
							{
								GLOBAL.HLog.ROC.Out(string.Format("Override {0} = {1}", new object[] { row["Key"].ToString(), HelperOrderTypeByExchange.ExchangesSupportPegMidOrderType }));
								Configuration.ROC.Default.ExchangesSupportPegMidOrderType = HelperOrderTypeByExchange.ExchangesSupportPegMidOrderType;
								Configuration.ROC.Default.Save();
							}
							break;
						case "ROC.ExchangesSupportPegMktOrderType":
							if (Configuration.ROC.Default.ExchangesSupportPegMktOrderType != HelperOrderTypeByExchange.ExchangesSupportPegMktOrderType)
							{
								GLOBAL.HLog.ROC.Out(string.Format("Override {0} = {1}", new object[] { row["Key"].ToString(), HelperOrderTypeByExchange.ExchangesSupportPegMktOrderType }));
								Configuration.ROC.Default.ExchangesSupportPegMktOrderType = HelperOrderTypeByExchange.ExchangesSupportPegMktOrderType;
								Configuration.ROC.Default.Save();
							}
							break;
						case "ROC.ExchangesSupportPegPriOrderType":
							if (Configuration.ROC.Default.ExchangesSupportPegPriOrderType != HelperOrderTypeByExchange.ExchangesSupportPegPriOrderType)
							{
								GLOBAL.HLog.ROC.Out(string.Format("Override {0} = {1}", new object[] { row["Key"].ToString(), HelperOrderTypeByExchange.ExchangesSupportPegPriOrderType }));
								Configuration.ROC.Default.ExchangesSupportPegPriOrderType = HelperOrderTypeByExchange.ExchangesSupportPegPriOrderType;
								Configuration.ROC.Default.Save();
							}
							break;
						case "ROC.BookDepthLimit":
							Configuration.ROC.Default.BookDepthLimit = Convert.ToInt32(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.MemoryFlushInterval":
							Configuration.ROC.Default.MemoryFlushInterval = Convert.ToInt32(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.MemoryFlushLimit":
							Configuration.ROC.Default.MemoryFlushLimit = Convert.ToInt32(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusAutoSpreadTicket":
							Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusPlotList":
							Configuration.ROC.Default.EnableAutoFocusPlotList = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusWatchList":
							Configuration.ROC.Default.EnableAutoFocusWatchList = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusOrders":
							Configuration.ROC.Default.EnableAutoFocusOrders = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusPositions":
							Configuration.ROC.Default.EnableAutoFocusPositions = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusBatchTicket":
							Configuration.ROC.Default.EnableAutoFocusBatchTicket = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusMarketDataBatchTicket":
							Configuration.ROC.Default.EnableAutoFocusMarketDataBatchTicket = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusFutureTicket":
							Configuration.ROC.Default.EnableAutoFocusFutureTicket = Convert.ToBoolean(row["Val"]);
							break;
						case "ROC.EnableAutoFocusOptionTicket":
							Configuration.ROC.Default.EnableAutoFocusOptionTicket = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusQuickTicket":
							Configuration.ROC.Default.EnableAutoFocusQuickTicket = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusStockTicket":
							Configuration.ROC.Default.EnableAutoFocusStockTicket = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableAutoFocusTrades":
							Configuration.ROC.Default.EnableAutoFocusTrades = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.LargeScreenSize":
							Configuration.ROC.Default.LargeScreenSize = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.ShowLevel2AsDefault":
							Configuration.ROC.Default.ShowLevel2AsDefault = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.ShowQuickTicketAsDefault":
							Configuration.ROC.Default.ShowQuickTicketAsDefault = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.EnableQuickTicketAutoCenter":
							Configuration.ROC.Default.EnableQuickTicketAutoCenter = Convert.ToBoolean(row["Val"]);
							Configuration.ROC.Default.Save();
							break;
						case "ROC.QuickTicketAutoCenterRange":
							Configuration.ROC.Default.QuickTicketAutoCenterRange = Convert.ToInt32(row["Val"]);
							Configuration.ROC.Default.Save();
							break;

						case "ROM.CancelOnDisconnect":
							Configuration.ROM.Default.CancelOnDisconnect = Convert.ToBoolean(row["Val"]);
							Configuration.ROM.Default.Save();
							break;

						case "SOUND.Open":
							Configuration.SOUND.Default.Open = row["Val"].ToString();
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.Filled":
							Configuration.SOUND.Default.Filled = row["Val"].ToString();
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.Rejected":
							Configuration.SOUND.Default.Rejected = row["Val"].ToString();
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.Canceled":
							Configuration.SOUND.Default.Canceled = row["Val"].ToString();
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.FilledAndCanceled":
							Configuration.SOUND.Default.FilledAndCanceled = row["Val"].ToString();
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.PartiallyFilled":
							Configuration.SOUND.Default.PartiallyFilled = row["Val"].ToString();
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.Replaced":
							Configuration.SOUND.Default.Replaced = row["Val"].ToString();
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.EnableOpen":
							Configuration.SOUND.Default.EnableOpen = Convert.ToBoolean(row["Val"]);
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.EnableFilled":
							Configuration.SOUND.Default.EnableFilled = Convert.ToBoolean(row["Val"]);
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.EnableRejected":
							Configuration.SOUND.Default.EnableRejected = Convert.ToBoolean(row["Val"]);
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.EnableCanceled":
							Configuration.SOUND.Default.EnableCanceled = Convert.ToBoolean(row["Val"]);
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.EnablePartiallyFilled":
							Configuration.SOUND.Default.EnablePartiallyFilled = Convert.ToBoolean(row["Val"]);
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.EnableReplaced":
							Configuration.SOUND.Default.EnableReplaced = Convert.ToBoolean(row["Val"]);
							Configuration.SOUND.Default.Save();
							break;
						case "SOUND.EnableFilledAndCanceled":
							Configuration.SOUND.Default.EnableFilledAndCanceled = Convert.ToBoolean(row["Val"]);
							Configuration.SOUND.Default.Save();
							break;

						case "USER.DesktopHeight":
							Configuration.User.Default.DesktopHeight = Convert.ToInt32(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.DesktopWidth":
							Configuration.User.Default.DesktopWidth = Convert.ToInt32(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.UseMarketData":
							Configuration.User.Default.UseMarketData = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.UseTPOS":
							Configuration.User.Default.UseTPOS = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.UseROMDatabase":
							Configuration.User.Default.UseROMDatabase = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.ShowOnlySelectedExchange":
							Configuration.User.Default.ShowOnlySelectedExchange = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.DoNotLoadOPRAParticipant":
							Configuration.User.Default.DoNotLoadOPRAParticipant = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
						//case "USER.PlayAllStatusSound":
						//    Configuration.User.Default.PlayAllStatusSound = Convert.ToBoolean(row["Val"]);
						//    Configuration.User.Default.Save();
						//    break;
						case "USER.LastUserName":
							Configuration.User.Default.LastUserName = row["Val"].ToString();
							Configuration.User.Default.Save();
							break;
						case "USER.ConfirmOnCancellAll":
							Configuration.User.Default.ConfirmOnCancellAll = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.LastCSVFile":
							if (Configuration.User.Default.LastCSVFile != row["Val"].ToString())
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.LastCSVFile }));
								Configuration.User.Default.LastCSVFile = row["Val"].ToString();
								Configuration.User.Default.Save();
							}
							break;
						case "USER.CSVThreadPriority":
							if (Configuration.User.Default.CSVThreadPriority != Convert.ToInt32(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.CSVThreadPriority }));
								Configuration.User.Default.CSVThreadPriority = Convert.ToInt32(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.MDThreadPriority":
							if (Configuration.User.Default.MDThreadPriority != Convert.ToInt32(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.MDThreadPriority }));
								Configuration.User.Default.MDThreadPriority = Convert.ToInt32(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.UIUpdateRate":
							if (Configuration.User.Default.UIUpdateRate != Convert.ToInt32(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.UIUpdateRate }));
								Configuration.User.Default.UIUpdateRate = Convert.ToInt32(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.UseUIUpdateRate":
							if (Configuration.User.Default.UseUIUpdateRate != Convert.ToBoolean(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.UseUIUpdateRate }));
								Configuration.User.Default.UseUIUpdateRate = Convert.ToBoolean(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.UseStockAutoCancel":
							if (Configuration.User.Default.UseStockAutoCancel != Convert.ToBoolean(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.UseStockAutoCancel }));
								Configuration.User.Default.UseStockAutoCancel = Convert.ToBoolean(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.StockAutoCancelTime":
							if (Configuration.User.Default.StockAutoCancelTime != row["Val"].ToString())
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.StockAutoCancelTime }));
								Configuration.User.Default.StockAutoCancelTime = row["Val"].ToString();
								Configuration.User.Default.Save();
							}
							break;
						case "USER.UseFutureAutoCancel":
							if (Configuration.User.Default.UseFutureAutoCancel != Convert.ToBoolean(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.UseFutureAutoCancel }));
								Configuration.User.Default.UseFutureAutoCancel = Convert.ToBoolean(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.FutureAutoCancelTime":
							if (Configuration.User.Default.FutureAutoCancelTime != row["Val"].ToString())
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.FutureAutoCancelTime }));
								Configuration.User.Default.FutureAutoCancelTime = row["Val"].ToString();
								Configuration.User.Default.Save();
							}
							break;
						case "USER.UseOptionAutoCancel":
							if (Configuration.User.Default.UseOptionAutoCancel != Convert.ToBoolean(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.UseOptionAutoCancel }));
								Configuration.User.Default.UseOptionAutoCancel = Convert.ToBoolean(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.OptionAutoCancelTime":
							if (Configuration.User.Default.OptionAutoCancelTime != row["Val"].ToString())
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.OptionAutoCancelTime }));
								Configuration.User.Default.OptionAutoCancelTime = row["Val"].ToString();
								Configuration.User.Default.Save();
							}
							break;
						case "USER.SkipGTCandGTDonAuto":
							if (Configuration.User.Default.SkipGTCandGTDonAuto != Convert.ToBoolean(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.SkipGTCandGTDonAuto }));
								Configuration.User.Default.SkipGTCandGTDonAuto = Convert.ToBoolean(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.OrderAutoFocusLast":
							if (Configuration.User.Default.OrderAutoFocusLast != Convert.ToBoolean(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.OrderAutoFocusLast }));
								Configuration.User.Default.OrderAutoFocusLast = Convert.ToBoolean(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.OrderAvgPriceResolution":
							if (Configuration.User.Default.OrderAvgPriceResolution != Convert.ToInt32(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.OrderAvgPriceResolution }));
								Configuration.User.Default.OrderAvgPriceResolution = Convert.ToInt32(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.ShowInternalStatus":
							if (Configuration.User.Default.ShowInternalStatus = Convert.ToBoolean(row["Val"]))
							{
								GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", new object[] { row["Key"].ToString(), Configuration.User.Default.ShowInternalStatus }));
								Configuration.User.Default.ShowInternalStatus = Convert.ToBoolean(row["Val"]);
								Configuration.User.Default.Save();
							}
							break;
						case "USER.DelayUIProcess":
							Configuration.User.Default.DelayUIProcess = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
						case "USER.AutoFocusOnControls":
							Configuration.User.Default.AutoFocusOnControls = Convert.ToBoolean(row["Val"]);
							Configuration.User.Default.Save();
							break;
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public static void Save()
		{
			try
			{
				userTable.Rows.Clear();

				userTable.Rows.Add(new object[] { 
					"ROC.ExchangesSupportExtendedMarketOrderType", 
					Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType } );
				userTable.Rows.Add(new object[] { 
					"ROC.ExchangesSupportExtendedLimitOrderType", 
					Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType });
				userTable.Rows.Add(new object[] { 
					"ROC.ExchangesSupportStopOrderType", 
					Configuration.ROC.Default.ExchangesSupportStopOrderType });
				userTable.Rows.Add(new object[] { 
					"ROC.ExchangesSupportLimitStopOrderType", 
					Configuration.ROC.Default.ExchangesSupportLimitStopOrderType });
				userTable.Rows.Add(new object[] { 
					"ROC.ExchangesSupportPegMidOrderType", 
					Configuration.ROC.Default.ExchangesSupportPegMidOrderType });
				userTable.Rows.Add(new object[] { 
					"ROC.ExchangesSupportPegMktOrderType", 
					Configuration.ROC.Default.ExchangesSupportPegMktOrderType });
				userTable.Rows.Add(new object[] { 
					"ROC.ExchangesSupportPegPriOrderType", 
					Configuration.ROC.Default.ExchangesSupportPegPriOrderType });
				userTable.Rows.Add(new object[] { 
					"ROC.UseGroupSubscription", 
					Configuration.ROC.Default.UseGroupSubscription });
				userTable.Rows.Add(new object[] { 
					"ROC.BookDepthLimit", 
					Configuration.ROC.Default.BookDepthLimit });
				userTable.Rows.Add(new object[] { 
					"ROC.MemoryFlushInterval", 
					Configuration.ROC.Default.MemoryFlushInterval });
				userTable.Rows.Add(new object[] { 
					"ROC.MemoryFlushLimit", 
					Configuration.ROC.Default.MemoryFlushLimit });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusAutoSpreadTicket", 
					Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusPlotList", 
					Configuration.ROC.Default.EnableAutoFocusPlotList });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusWatchList", 
					Configuration.ROC.Default.EnableAutoFocusWatchList });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusOrders", 
					Configuration.ROC.Default.EnableAutoFocusOrders });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusPositions", 
					Configuration.ROC.Default.EnableAutoFocusPositions });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusBatchTicket", 
					Configuration.ROC.Default.EnableAutoFocusBatchTicket });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusMarketDataBatchTicket", 
					Configuration.ROC.Default.EnableAutoFocusMarketDataBatchTicket });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusFutureTicket", 
					Configuration.ROC.Default.EnableAutoFocusFutureTicket });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusOptionTicket", 
					Configuration.ROC.Default.EnableAutoFocusOptionTicket });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusQuickTicket", 
					Configuration.ROC.Default.EnableAutoFocusQuickTicket });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusStockTicket", 
					Configuration.ROC.Default.EnableAutoFocusStockTicket });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusTrades", 
					Configuration.ROC.Default.EnableAutoFocusTrades });
				userTable.Rows.Add(new object[] { 
					"ROC.LargeScreenSize", 
					Configuration.ROC.Default.LargeScreenSize });
				userTable.Rows.Add(new object[] { 
					"ROC.ShowLevel2AsDefault", 
					Configuration.ROC.Default.ShowLevel2AsDefault });
				userTable.Rows.Add(new object[] { 
					"ROC.ShowQuickTicketAsDefault", 
					Configuration.ROC.Default.ShowQuickTicketAsDefault });
				userTable.Rows.Add(new object[] { 
					"ROC.EnableQuickTicketAutoCenter", 
					Configuration.ROC.Default.EnableQuickTicketAutoCenter });
				userTable.Rows.Add(new object[] { 
					"ROC.QuickTicketAutoCenterRange", 
					Configuration.ROC.Default.QuickTicketAutoCenterRange });

				userTable.Rows.Add(new object[] { 
					"ROM.CancelOnDisconnect", 
					Configuration.ROM.Default.CancelOnDisconnect });

				userTable.Rows.Add(new object[] { 
					"SOUND.Open", 
					Configuration.SOUND.Default.Open });
				userTable.Rows.Add(new object[] { 
					"SOUND.Filled", 
					Configuration.SOUND.Default.Filled });
				userTable.Rows.Add(new object[] { 
					"SOUND.Rejected", 
					Configuration.SOUND.Default.Rejected });
				userTable.Rows.Add(new object[] { 
					"SOUND.Canceled", 
					Configuration.SOUND.Default.Canceled });
				userTable.Rows.Add(new object[] { 
					"SOUND.FilledAndCanceled", 
					Configuration.SOUND.Default.FilledAndCanceled });
				userTable.Rows.Add(new object[] { 
					"SOUND.PartiallyFilled", 
					Configuration.SOUND.Default.PartiallyFilled });
				userTable.Rows.Add(new object[] { 
					"SOUND.Replaced", 
					Configuration.SOUND.Default.Replaced });
				userTable.Rows.Add(new object[] { 
					"SOUND.EnableOpen", 
					Configuration.SOUND.Default.EnableOpen });
				userTable.Rows.Add(new object[] { 
					"SOUND.EnableFilled", 
					Configuration.SOUND.Default.EnableFilled });
				userTable.Rows.Add(new object[] { 
					"SOUND.EnableRejected", 
					Configuration.SOUND.Default.EnableRejected });
				userTable.Rows.Add(new object[] { 
					"SOUND.EnableCanceled", 
					Configuration.SOUND.Default.EnableCanceled });
				userTable.Rows.Add(new object[] { 
					"SOUND.EnablePartiallyFilled", 
					Configuration.SOUND.Default.EnablePartiallyFilled });
				userTable.Rows.Add(new object[] { 
					"SOUND.EnableReplaced", 
					Configuration.SOUND.Default.EnableReplaced });
				userTable.Rows.Add(new object[] { 
					"SOUND.EnableFilledAndCanceled", 
					Configuration.SOUND.Default.EnableFilledAndCanceled });

				userTable.Rows.Add(new object[] { 
					"USER.DesktopHeight", 
					Configuration.User.Default.DesktopHeight });
				userTable.Rows.Add(new object[] { 
					"USER.DesktopWidth", 
					Configuration.User.Default.DesktopWidth });
				userTable.Rows.Add(new object[] { 
					"USER.UseMarketData", 
					Configuration.User.Default.UseMarketData });
				userTable.Rows.Add(new object[] { 
					"USER.UseTPOS", 
					Configuration.User.Default.UseTPOS });
				userTable.Rows.Add(new object[] { 
					"USER.UseROMDatabase", 
					Configuration.User.Default.UseROMDatabase });
				userTable.Rows.Add(new object[] { 
					"USER.ShowOnlySelectedExchange", 
					Configuration.User.Default.ShowOnlySelectedExchange });
				userTable.Rows.Add(new object[] { 
					"USER.DoNotLoadOPRAParticipant", 
					Configuration.User.Default.DoNotLoadOPRAParticipant });
				//userTable.Rows.Add(new object[] { 
				//    "USER.PlayAllStatusSound", 
				//    Configuration.User.Default.PlayAllStatusSound });
				userTable.Rows.Add(new object[] { 
					"USER.LastUserName", 
					Configuration.User.Default.LastUserName });
				userTable.Rows.Add(new object[] { 
					"USER.ConfirmOnCancellAll", 
					Configuration.User.Default.ConfirmOnCancellAll });
				userTable.Rows.Add(new object[] { 
					"USER.LastCSVFile", 
					Configuration.User.Default.LastCSVFile });
				userTable.Rows.Add(new object[] { 
					"USER.CSVThreadPriority", 
					Configuration.User.Default.CSVThreadPriority });
				userTable.Rows.Add(new object[] { 
					"USER.MDThreadPriority", 
					Configuration.User.Default.MDThreadPriority });
				userTable.Rows.Add(new object[] { 
					"USER.UIUpdateRate", 
					Configuration.User.Default.UIUpdateRate });
				userTable.Rows.Add(new object[] { 
					"USER.UseUIUpdateRate", 
					Configuration.User.Default.UseUIUpdateRate });
				userTable.Rows.Add(new object[] { 
					"USER.UseStockAutoCancel", 
					Configuration.User.Default.UseStockAutoCancel });
				userTable.Rows.Add(new object[] { 
					"USER.StockAutoCancelTime", 
					Configuration.User.Default.StockAutoCancelTime });
				userTable.Rows.Add(new object[] { 
					"USER.UseFutureAutoCancel", 
					Configuration.User.Default.UseFutureAutoCancel });
				userTable.Rows.Add(new object[] { 
					"USER.FutureAutoCancelTime", 
					Configuration.User.Default.FutureAutoCancelTime });
				userTable.Rows.Add(new object[] { 
					"USER.UseOptionAutoCancel", 
					Configuration.User.Default.UseOptionAutoCancel });
				userTable.Rows.Add(new object[] { 
					"USER.OptionAutoCancelTime", 
					Configuration.User.Default.OptionAutoCancelTime });
				userTable.Rows.Add(new object[] { 
					"USER.SkipGTCandGTDonAuto", 
					Configuration.User.Default.SkipGTCandGTDonAuto });
				userTable.Rows.Add(new object[] { 
					"USER.OrderAutoFocusLast", 
					Configuration.User.Default.OrderAutoFocusLast });
				userTable.Rows.Add(new object[] { 
					"USER.OrderAvgPriceResolution", 
					Configuration.User.Default.OrderAvgPriceResolution });
				userTable.Rows.Add(new object[] { 
					"USER.ShowInternalStatus", 
					Configuration.User.Default.ShowInternalStatus });
				userTable.Rows.Add(new object[] { 
					"USER.DelayUIProcess", 
					Configuration.User.Default.DelayUIProcess });
				userTable.Rows.Add(new object[] { 
					"USER.AutoFocusOnControls", 
					Configuration.User.Default.AutoFocusOnControls });

				if (!Configuration.Path.Default.SettingPath.Contains(Configuration.Path.Default.ProfilePath))
				{
					// Put the settings into the Profile Folder
					Configuration.Path.Default.SettingPath = Configuration.Path.Default.ProfilePath + @"Settings\";
					Configuration.Path.Default.Save();
				}
				HelperFile.Save(userTable, Configuration.Path.Default.SettingPath, _filename);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}
	}
}
