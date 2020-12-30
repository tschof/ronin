using System;
using System.IO;
using System.Data;
using Log = Common.Log;

namespace ROC
{
	public static class HelperSettings
	{
		private const string SETTINGS_FILENAME = "Settings.xml";

		private static DataTable _settingsTable;

		private static DataTable getSettingsTable()
		{
			if (_settingsTable == null)
			{
				_settingsTable = new DataTable("Settings");
				_settingsTable.Columns.Add("Key");
				_settingsTable.Columns.Add("Val");
			}
			return _settingsTable;
		}

		public static void Load()
		{
			// Look for the settings XML file in these folders, using the first found.
			string[] folders = new string[] {
				Configuration.Path.Default.SettingPath,
				Path.Combine(Configuration.Path.Default.ProfilePath, "Settings"),
				@"..\Settings",
				"."
			};

			DataTable table = getSettingsTable();
			bool foundSettings = false;

			try {
				foreach (string folder in folders) {
					if (HelperFile.Load(table, folder, SETTINGS_FILENAME)) {
						log($"Loaded settings from {SETTINGS_FILENAME} in {folder}.");
						foundSettings = true;
						break;
					}
				}

				if (!foundSettings) {
					log($"Can't find settings file {SETTINGS_FILENAME}.");
					return;
				}

				bool dirtyRoc = false, dirtyRom = false, dirtySound = false, dirtyUser = false;

				foreach (DataRow row in table.Rows)
				{
					log("Updated {0} = {1}", row["Key"], row["Val"]);

					switch (row["Key"].ToString())
					{
						case "ROC.ExchangesSupportExtendedMarketOrderType":
							if (Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType != HelperOrderTypeByExchange.ExchangesSupportExtendedMarketOrderType)
							{
								log("Override {0} = {1}", row["Key"], HelperOrderTypeByExchange.ExchangesSupportExtendedMarketOrderType);
								Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType = HelperOrderTypeByExchange.ExchangesSupportExtendedMarketOrderType;
								dirtyRoc = true;
							}
							break;
						case "ROC.ExchangesSupportExtendedLimitOrderType":
							if (Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType != HelperOrderTypeByExchange.ExchangesSupportExtendedLimitOrderType)
							{
								log("Override {0} = {1}", row["Key"], HelperOrderTypeByExchange.ExchangesSupportExtendedLimitOrderType);
								Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType = HelperOrderTypeByExchange.ExchangesSupportExtendedLimitOrderType;
								dirtyRoc = true;
							}
							break;
						case "ROC.ExchangesSupportStopOrderType":
							if (Configuration.ROC.Default.ExchangesSupportStopOrderType != HelperOrderTypeByExchange.ExchangesSupportStopOrderType)
							{
								log("Override {0} = {1}", row["Key"], HelperOrderTypeByExchange.ExchangesSupportStopOrderType);
								Configuration.ROC.Default.ExchangesSupportStopOrderType = HelperOrderTypeByExchange.ExchangesSupportStopOrderType;
								dirtyRoc = true;
							}
							break;
						case "ROC.ExchangesSupportLimitStopOrderType":
							if (Configuration.ROC.Default.ExchangesSupportLimitStopOrderType != HelperOrderTypeByExchange.ExchangesSupportLimitStopOrderType)
							{
								log("Override {0} = {1}", row["Key"], HelperOrderTypeByExchange.ExchangesSupportLimitStopOrderType);
								Configuration.ROC.Default.ExchangesSupportLimitStopOrderType = HelperOrderTypeByExchange.ExchangesSupportLimitStopOrderType;
								dirtyRoc = true;
							}
							break;
						case "ROC.ExchangesSupportPegMidOrderType":
							if (Configuration.ROC.Default.ExchangesSupportPegMidOrderType != HelperOrderTypeByExchange.ExchangesSupportPegMidOrderType)
							{
								log("Override {0} = {1}", row["Key"], HelperOrderTypeByExchange.ExchangesSupportPegMidOrderType);
								Configuration.ROC.Default.ExchangesSupportPegMidOrderType = HelperOrderTypeByExchange.ExchangesSupportPegMidOrderType;
								dirtyRoc = true;
							}
							break;
						case "ROC.ExchangesSupportPegMktOrderType":
							if (Configuration.ROC.Default.ExchangesSupportPegMktOrderType != HelperOrderTypeByExchange.ExchangesSupportPegMktOrderType)
							{
								log("Override {0} = {1}", row["Key"], HelperOrderTypeByExchange.ExchangesSupportPegMktOrderType);
								Configuration.ROC.Default.ExchangesSupportPegMktOrderType = HelperOrderTypeByExchange.ExchangesSupportPegMktOrderType;
								dirtyRoc = true;
							}
							break;
						case "ROC.ExchangesSupportPegPriOrderType":
							if (Configuration.ROC.Default.ExchangesSupportPegPriOrderType != HelperOrderTypeByExchange.ExchangesSupportPegPriOrderType)
							{
								log("Override {0} = {1}", row["Key"], HelperOrderTypeByExchange.ExchangesSupportPegPriOrderType);
								Configuration.ROC.Default.ExchangesSupportPegPriOrderType = HelperOrderTypeByExchange.ExchangesSupportPegPriOrderType;
								dirtyRoc = true;
							}
							break;
						case "ROC.BookDepthLimit":
							Configuration.ROC.Default.BookDepthLimit = Convert.ToInt32(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.MemoryFlushInterval":
							Configuration.ROC.Default.MemoryFlushInterval = Convert.ToInt32(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.MemoryFlushLimit":
							Configuration.ROC.Default.MemoryFlushLimit = Convert.ToInt32(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusAutoSpreadTicket":
							Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusPlotList":
							Configuration.ROC.Default.EnableAutoFocusPlotList = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusWatchList":
							Configuration.ROC.Default.EnableAutoFocusWatchList = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusOrders":
							Configuration.ROC.Default.EnableAutoFocusOrders = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusPositions":
							Configuration.ROC.Default.EnableAutoFocusPositions = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusBatchTicket":
							Configuration.ROC.Default.EnableAutoFocusBatchTicket = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusMarketDataBatchTicket":
							Configuration.ROC.Default.EnableAutoFocusMarketDataBatchTicket = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusFutureTicket":
							Configuration.ROC.Default.EnableAutoFocusFutureTicket = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusOptionTicket":
							Configuration.ROC.Default.EnableAutoFocusOptionTicket = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusQuickTicket":
							Configuration.ROC.Default.EnableAutoFocusQuickTicket = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusStockTicket":
							Configuration.ROC.Default.EnableAutoFocusStockTicket = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableAutoFocusTrades":
							Configuration.ROC.Default.EnableAutoFocusTrades = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.LargeScreenSize":
							Configuration.ROC.Default.LargeScreenSize = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.ShowLevel2AsDefault":
							Configuration.ROC.Default.ShowLevel2AsDefault = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.ShowQuickTicketAsDefault":
							Configuration.ROC.Default.ShowQuickTicketAsDefault = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.EnableQuickTicketAutoCenter":
							Configuration.ROC.Default.EnableQuickTicketAutoCenter = Convert.ToBoolean(row["Val"]);
							dirtyRoc = true;
							break;
						case "ROC.QuickTicketAutoCenterRange":
							Configuration.ROC.Default.QuickTicketAutoCenterRange = Convert.ToInt32(row["Val"]);
							dirtyRoc = true;
							break;

						// ROM
						case "ROM.CancelOnDisconnect":
							Configuration.ROM.Default.CancelOnDisconnect = Convert.ToBoolean(row["Val"]);
							dirtyRom = true;
							break;

						// SOUND
						case "SOUND.Open":
							Configuration.SOUND.Default.Open = row["Val"].ToString();
							dirtySound = true;
							break;
						case "SOUND.Filled":
							Configuration.SOUND.Default.Filled = row["Val"].ToString();
							dirtySound = true;
							break;
						case "SOUND.Rejected":
							Configuration.SOUND.Default.Rejected = row["Val"].ToString();
							dirtySound = true;
							break;
						case "SOUND.Canceled":
							Configuration.SOUND.Default.Canceled = row["Val"].ToString();
							dirtySound = true;
							break;
						case "SOUND.FilledAndCanceled":
							Configuration.SOUND.Default.FilledAndCanceled = row["Val"].ToString();
							dirtySound = true;
							break;
						case "SOUND.PartiallyFilled":
							Configuration.SOUND.Default.PartiallyFilled = row["Val"].ToString();
							dirtySound = true;
							break;
						case "SOUND.Replaced":
							Configuration.SOUND.Default.Replaced = row["Val"].ToString();
							dirtySound = true;
							break;
						case "SOUND.EnableOpen":
							Configuration.SOUND.Default.EnableOpen = Convert.ToBoolean(row["Val"]);
							dirtySound = true;
							break;
						case "SOUND.EnableFilled":
							Configuration.SOUND.Default.EnableFilled = Convert.ToBoolean(row["Val"]);
							dirtySound = true;
							break;
						case "SOUND.EnableRejected":
							Configuration.SOUND.Default.EnableRejected = Convert.ToBoolean(row["Val"]);
							dirtySound = true;
							break;
						case "SOUND.EnableCanceled":
							Configuration.SOUND.Default.EnableCanceled = Convert.ToBoolean(row["Val"]);
							dirtySound = true;
							break;
						case "SOUND.EnablePartiallyFilled":
							Configuration.SOUND.Default.EnablePartiallyFilled = Convert.ToBoolean(row["Val"]);
							dirtySound = true;
							break;
						case "SOUND.EnableReplaced":
							Configuration.SOUND.Default.EnableReplaced = Convert.ToBoolean(row["Val"]);
							dirtySound = true;
							break;
						case "SOUND.EnableFilledAndCanceled":
							Configuration.SOUND.Default.EnableFilledAndCanceled = Convert.ToBoolean(row["Val"]);
							dirtySound = true;
							break;

						// User
						case "USER.DesktopHeight":
							Configuration.User.Default.DesktopHeight = Convert.ToInt32(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.DesktopWidth":
							Configuration.User.Default.DesktopWidth = Convert.ToInt32(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.UseMarketData":
							Configuration.User.Default.UseMarketData = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.UseTPOS":
							Configuration.User.Default.UseTPOS = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.UseROMDatabase":
							Configuration.User.Default.UseROMDatabase = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.ShowOnlySelectedExchange":
							Configuration.User.Default.ShowOnlySelectedExchange = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.DoNotLoadOPRAParticipant":
							Configuration.User.Default.DoNotLoadOPRAParticipant = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.LastUserName":
							Configuration.User.Default.LastUserName = row["Val"].ToString();
							dirtyUser = true;
							break;
						case "USER.ConfirmOnCancellAll":
							Configuration.User.Default.ConfirmOnCancellAll = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.LastCSVFile":
							if (Configuration.User.Default.LastCSVFile != row["Val"].ToString())
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.LastCSVFile);
								Configuration.User.Default.LastCSVFile = row["Val"].ToString();
								dirtyUser = true;
							}
							break;
						case "USER.CSVThreadPriority":
							if (Configuration.User.Default.CSVThreadPriority != Convert.ToInt32(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.CSVThreadPriority);
								Configuration.User.Default.CSVThreadPriority = Convert.ToInt32(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.MDThreadPriority":
							if (Configuration.User.Default.MDThreadPriority != Convert.ToInt32(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.MDThreadPriority);
								Configuration.User.Default.MDThreadPriority = Convert.ToInt32(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.UIUpdateRate":
							if (Configuration.User.Default.UIUpdateRate != Convert.ToInt32(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.UIUpdateRate);
								Configuration.User.Default.UIUpdateRate = Convert.ToInt32(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.UseUIUpdateRate":
							if (Configuration.User.Default.UseUIUpdateRate != Convert.ToBoolean(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.UseUIUpdateRate);
								Configuration.User.Default.UseUIUpdateRate = Convert.ToBoolean(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.UseStockAutoCancel":
							if (Configuration.User.Default.UseStockAutoCancel != Convert.ToBoolean(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.UseStockAutoCancel);
								Configuration.User.Default.UseStockAutoCancel = Convert.ToBoolean(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.StockAutoCancelTime":
							if (Configuration.User.Default.StockAutoCancelTime != row["Val"].ToString())
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.StockAutoCancelTime);
								Configuration.User.Default.StockAutoCancelTime = row["Val"].ToString();
								dirtyUser = true;
							}
							break;
						case "USER.UseFutureAutoCancel":
							if (Configuration.User.Default.UseFutureAutoCancel != Convert.ToBoolean(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.UseFutureAutoCancel);
								Configuration.User.Default.UseFutureAutoCancel = Convert.ToBoolean(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.FutureAutoCancelTime":
							if (Configuration.User.Default.FutureAutoCancelTime != row["Val"].ToString())
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.FutureAutoCancelTime);
								Configuration.User.Default.FutureAutoCancelTime = row["Val"].ToString();
								dirtyUser = true;
							}
							break;
						case "USER.UseOptionAutoCancel":
							if (Configuration.User.Default.UseOptionAutoCancel != Convert.ToBoolean(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.UseOptionAutoCancel);
								Configuration.User.Default.UseOptionAutoCancel = Convert.ToBoolean(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.OptionAutoCancelTime":
							if (Configuration.User.Default.OptionAutoCancelTime != row["Val"].ToString())
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.OptionAutoCancelTime);
								Configuration.User.Default.OptionAutoCancelTime = row["Val"].ToString();
								dirtyUser = true;
							}
							break;
						case "USER.SkipGTCandGTDonAuto":
							if (Configuration.User.Default.SkipGTCandGTDonAuto != Convert.ToBoolean(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.SkipGTCandGTDonAuto);
								Configuration.User.Default.SkipGTCandGTDonAuto = Convert.ToBoolean(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.OrderAutoFocusLast":
							if (Configuration.User.Default.OrderAutoFocusLast != Convert.ToBoolean(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.OrderAutoFocusLast);
								Configuration.User.Default.OrderAutoFocusLast = Convert.ToBoolean(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.OrderAvgPriceResolution":
							if (Configuration.User.Default.OrderAvgPriceResolution != Convert.ToInt32(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.OrderAvgPriceResolution);
								Configuration.User.Default.OrderAvgPriceResolution = Convert.ToInt32(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.ShowInternalStatus":
							if (Configuration.User.Default.ShowInternalStatus = Convert.ToBoolean(row["Val"]))
							{
								log("Origional {0} = {1}", row["Key"], Configuration.User.Default.ShowInternalStatus);
								Configuration.User.Default.ShowInternalStatus = Convert.ToBoolean(row["Val"]);
								dirtyUser = true;
							}
							break;
						case "USER.DelayUIProcess":
							Configuration.User.Default.DelayUIProcess = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
						case "USER.AutoFocusOnControls":
							Configuration.User.Default.AutoFocusOnControls = Convert.ToBoolean(row["Val"]);
							dirtyUser = true;
							break;
					}
				}

				if (dirtyRoc)
					Configuration.ROC.Default.Save();
				if (dirtyRom)
					Configuration.ROM.Default.Save();
				if (dirtySound)
					Configuration.SOUND.Default.Save();
				if (dirtyUser)
					Configuration.User.Default.Save();
			} catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public static void Save()
		{
			try
			{
				var table = getSettingsTable();
				table.Rows.Clear();

				table.Rows.Add(new object[] { 
					"ROC.ExchangesSupportExtendedMarketOrderType", 
					Configuration.ROC.Default.ExchangesSupportExtendedMarketOrderType } );
				table.Rows.Add(new object[] { 
					"ROC.ExchangesSupportExtendedLimitOrderType", 
					Configuration.ROC.Default.ExchangesSupportExtendedLimitOrderType });
				table.Rows.Add(new object[] { 
					"ROC.ExchangesSupportStopOrderType", 
					Configuration.ROC.Default.ExchangesSupportStopOrderType });
				table.Rows.Add(new object[] { 
					"ROC.ExchangesSupportLimitStopOrderType", 
					Configuration.ROC.Default.ExchangesSupportLimitStopOrderType });
				table.Rows.Add(new object[] { 
					"ROC.ExchangesSupportPegMidOrderType", 
					Configuration.ROC.Default.ExchangesSupportPegMidOrderType });
				table.Rows.Add(new object[] { 
					"ROC.ExchangesSupportPegMktOrderType", 
					Configuration.ROC.Default.ExchangesSupportPegMktOrderType });
				table.Rows.Add(new object[] { 
					"ROC.ExchangesSupportPegPriOrderType", 
					Configuration.ROC.Default.ExchangesSupportPegPriOrderType });
				table.Rows.Add(new object[] { 
					"ROC.UseGroupSubscription", 
					Configuration.ROC.Default.UseGroupSubscription });
				table.Rows.Add(new object[] { 
					"ROC.BookDepthLimit", 
					Configuration.ROC.Default.BookDepthLimit });
				table.Rows.Add(new object[] { 
					"ROC.MemoryFlushInterval", 
					Configuration.ROC.Default.MemoryFlushInterval });
				table.Rows.Add(new object[] { 
					"ROC.MemoryFlushLimit", 
					Configuration.ROC.Default.MemoryFlushLimit });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusAutoSpreadTicket", 
					Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusPlotList", 
					Configuration.ROC.Default.EnableAutoFocusPlotList });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusWatchList", 
					Configuration.ROC.Default.EnableAutoFocusWatchList });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusOrders", 
					Configuration.ROC.Default.EnableAutoFocusOrders });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusPositions", 
					Configuration.ROC.Default.EnableAutoFocusPositions });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusBatchTicket", 
					Configuration.ROC.Default.EnableAutoFocusBatchTicket });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusMarketDataBatchTicket", 
					Configuration.ROC.Default.EnableAutoFocusMarketDataBatchTicket });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusFutureTicket", 
					Configuration.ROC.Default.EnableAutoFocusFutureTicket });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusOptionTicket", 
					Configuration.ROC.Default.EnableAutoFocusOptionTicket });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusQuickTicket", 
					Configuration.ROC.Default.EnableAutoFocusQuickTicket });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusStockTicket", 
					Configuration.ROC.Default.EnableAutoFocusStockTicket });
				table.Rows.Add(new object[] { 
					"ROC.EnableAutoFocusTrades", 
					Configuration.ROC.Default.EnableAutoFocusTrades });
				table.Rows.Add(new object[] { 
					"ROC.LargeScreenSize", 
					Configuration.ROC.Default.LargeScreenSize });
				table.Rows.Add(new object[] { 
					"ROC.ShowLevel2AsDefault", 
					Configuration.ROC.Default.ShowLevel2AsDefault });
				table.Rows.Add(new object[] { 
					"ROC.ShowQuickTicketAsDefault", 
					Configuration.ROC.Default.ShowQuickTicketAsDefault });
				table.Rows.Add(new object[] { 
					"ROC.EnableQuickTicketAutoCenter", 
					Configuration.ROC.Default.EnableQuickTicketAutoCenter });
				table.Rows.Add(new object[] { 
					"ROC.QuickTicketAutoCenterRange", 
					Configuration.ROC.Default.QuickTicketAutoCenterRange });

				table.Rows.Add(new object[] { 
					"ROM.CancelOnDisconnect", 
					Configuration.ROM.Default.CancelOnDisconnect });

				table.Rows.Add(new object[] { 
					"SOUND.Open", 
					Configuration.SOUND.Default.Open });
				table.Rows.Add(new object[] { 
					"SOUND.Filled", 
					Configuration.SOUND.Default.Filled });
				table.Rows.Add(new object[] { 
					"SOUND.Rejected", 
					Configuration.SOUND.Default.Rejected });
				table.Rows.Add(new object[] { 
					"SOUND.Canceled", 
					Configuration.SOUND.Default.Canceled });
				table.Rows.Add(new object[] { 
					"SOUND.FilledAndCanceled", 
					Configuration.SOUND.Default.FilledAndCanceled });
				table.Rows.Add(new object[] { 
					"SOUND.PartiallyFilled", 
					Configuration.SOUND.Default.PartiallyFilled });
				table.Rows.Add(new object[] { 
					"SOUND.Replaced", 
					Configuration.SOUND.Default.Replaced });
				table.Rows.Add(new object[] { 
					"SOUND.EnableOpen", 
					Configuration.SOUND.Default.EnableOpen });
				table.Rows.Add(new object[] { 
					"SOUND.EnableFilled", 
					Configuration.SOUND.Default.EnableFilled });
				table.Rows.Add(new object[] { 
					"SOUND.EnableRejected", 
					Configuration.SOUND.Default.EnableRejected });
				table.Rows.Add(new object[] { 
					"SOUND.EnableCanceled", 
					Configuration.SOUND.Default.EnableCanceled });
				table.Rows.Add(new object[] { 
					"SOUND.EnablePartiallyFilled", 
					Configuration.SOUND.Default.EnablePartiallyFilled });
				table.Rows.Add(new object[] { 
					"SOUND.EnableReplaced", 
					Configuration.SOUND.Default.EnableReplaced });
				table.Rows.Add(new object[] { 
					"SOUND.EnableFilledAndCanceled", 
					Configuration.SOUND.Default.EnableFilledAndCanceled });

				table.Rows.Add(new object[] { 
					"USER.DesktopHeight", 
					Configuration.User.Default.DesktopHeight });
				table.Rows.Add(new object[] { 
					"USER.DesktopWidth", 
					Configuration.User.Default.DesktopWidth });
				table.Rows.Add(new object[] { 
					"USER.UseMarketData", 
					Configuration.User.Default.UseMarketData });
				table.Rows.Add(new object[] { 
					"USER.UseTPOS", 
					Configuration.User.Default.UseTPOS });
				table.Rows.Add(new object[] { 
					"USER.UseROMDatabase", 
					Configuration.User.Default.UseROMDatabase });
				table.Rows.Add(new object[] { 
					"USER.ShowOnlySelectedExchange", 
					Configuration.User.Default.ShowOnlySelectedExchange });
				table.Rows.Add(new object[] { 
					"USER.DoNotLoadOPRAParticipant", 
					Configuration.User.Default.DoNotLoadOPRAParticipant });
				//userTable.Rows.Add(new object[] { 
				//    "USER.PlayAllStatusSound", 
				//    Configuration.User.Default.PlayAllStatusSound });
				table.Rows.Add(new object[] { 
					"USER.LastUserName", 
					Configuration.User.Default.LastUserName });
				table.Rows.Add(new object[] { 
					"USER.ConfirmOnCancellAll", 
					Configuration.User.Default.ConfirmOnCancellAll });
				table.Rows.Add(new object[] { 
					"USER.LastCSVFile", 
					Configuration.User.Default.LastCSVFile });
				table.Rows.Add(new object[] { 
					"USER.CSVThreadPriority", 
					Configuration.User.Default.CSVThreadPriority });
				table.Rows.Add(new object[] { 
					"USER.MDThreadPriority", 
					Configuration.User.Default.MDThreadPriority });
				table.Rows.Add(new object[] { 
					"USER.UIUpdateRate", 
					Configuration.User.Default.UIUpdateRate });
				table.Rows.Add(new object[] { 
					"USER.UseUIUpdateRate", 
					Configuration.User.Default.UseUIUpdateRate });
				table.Rows.Add(new object[] { 
					"USER.UseStockAutoCancel", 
					Configuration.User.Default.UseStockAutoCancel });
				table.Rows.Add(new object[] { 
					"USER.StockAutoCancelTime", 
					Configuration.User.Default.StockAutoCancelTime });
				table.Rows.Add(new object[] { 
					"USER.UseFutureAutoCancel", 
					Configuration.User.Default.UseFutureAutoCancel });
				table.Rows.Add(new object[] { 
					"USER.FutureAutoCancelTime", 
					Configuration.User.Default.FutureAutoCancelTime });
				table.Rows.Add(new object[] { 
					"USER.UseOptionAutoCancel", 
					Configuration.User.Default.UseOptionAutoCancel });
				table.Rows.Add(new object[] { 
					"USER.OptionAutoCancelTime", 
					Configuration.User.Default.OptionAutoCancelTime });
				table.Rows.Add(new object[] { 
					"USER.SkipGTCandGTDonAuto", 
					Configuration.User.Default.SkipGTCandGTDonAuto });
				table.Rows.Add(new object[] { 
					"USER.OrderAutoFocusLast", 
					Configuration.User.Default.OrderAutoFocusLast });
				table.Rows.Add(new object[] { 
					"USER.OrderAvgPriceResolution", 
					Configuration.User.Default.OrderAvgPriceResolution });
				table.Rows.Add(new object[] { 
					"USER.ShowInternalStatus", 
					Configuration.User.Default.ShowInternalStatus });
				table.Rows.Add(new object[] { 
					"USER.DelayUIProcess", 
					Configuration.User.Default.DelayUIProcess });
				table.Rows.Add(new object[] { 
					"USER.AutoFocusOnControls", 
					Configuration.User.Default.AutoFocusOnControls });

				if (!Configuration.Path.Default.SettingPath.Contains(Configuration.Path.Default.ProfilePath))
				{
					// Put the settings into the Profile Folder
					Configuration.Path.Default.SettingPath = Configuration.Path.Default.ProfilePath + @"Settings\";
					Configuration.Path.Default.Save();
				}
				HelperFile.Save(table, Configuration.Path.Default.SettingPath, SETTINGS_FILENAME);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private static void log(string format, params object[] args)
		{
			Log.Info(Log.Target.ROC, string.Format(format, args));
		}
	}
}
