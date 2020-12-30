using System;
using System.Windows.Forms;
using Log = Common.Log;
//using System.Diagnostics;

namespace ROC
{
	public static class Program
	{
		[STAThread]
		public static void Main(string[] args)
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);

			if (args.Length > 0)
			{
				foreach (string arg in args)
				{
					switch (arg.ToUpper())
					{
						case "-REALTIME":
							System.Diagnostics.Process.GetCurrentProcess().PriorityClass = System.Diagnostics.ProcessPriorityClass.RealTime;
							break;
						case "-ADMIN":
							GLOBAL.AdminMode = true;
							break;
					}
				}
			}

			if (!GLOBAL.AdminMode && HelperProcessMonitor.AlreadyRunning("ROC"))
			{
				DialogResult dr = MessageBox.Show("ROC is already running, Do you want to restart?", "Notification", MessageBoxButtons.YesNo);
				if (dr == DialogResult.Yes)
				{
					HelperProcessMonitor.KillOpenProcess("ROC");
				}
				else
				{
					return;
				}
			}

			if (System.Diagnostics.Process.GetCurrentProcess().PriorityClass != System.Diagnostics.ProcessPriorityClass.RealTime)
			{
				System.Diagnostics.Process.GetCurrentProcess().PriorityClass = System.Diagnostics.ProcessPriorityClass.High;
			}

			//GLOBAL.BENCHAMRK.Mark = new Benchmark(null);

			#region - Log Files -

			try
			{
				Log.StartSingle(Configuration.Path.Default.LogPath);
			} catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
				MessageBox.Show(ex.Message + " " + ex.StackTrace);
			}

			#endregion

			#region - Log Version -

			GLOBAL.HROC.AddToStatusLogs("ROC Version ", GLOBAL.ROCVersion.ToString(), " Starting..." );

			#endregion

			#region - Settings Load -

			GLOBAL.HROC.AddToStatusLogs("ROC Setting Loadding... " );

			// Load Local User Settings
			HelperSettings.Load();

			if (Configuration.ROM.Default.SkipGTCandGTD && !Configuration.User.Default.SkipGTCandGTDonAuto)
			{
				GLOBAL.HROC.AddToStatusLogs("Sync ROM/ROC Skip GTC and GTD Flag!" );
				Configuration.User.Default.SkipGTCandGTDonAuto = Configuration.ROM.Default.SkipGTCandGTD;
				Configuration.User.Default.Save();
			}

			#endregion

			#region - Map Files -

			try
			{
				// Load Future Map
				GLOBAL.HROC.AddToStatusLogs("Load Future Map PlusOne ", HelperFuture.PlusOne.Count );
				GLOBAL.HROC.AddToStatusLogs("Load Future Map DoNotTranslate ", HelperFuture.DoNotTranslate.Count );

				// Load Option Map;
				GLOBAL.HROC.AddToStatusLogs("Load Option Map PlusOne ", HelperOption.PlusOne.Count );
				GLOBAL.HROC.AddToStatusLogs("Load Option Map ShowAs64TH ", HelperOption.ShowAs64TH.Count );
				GLOBAL.HROC.AddToStatusLogs("Load Option Map Exchange ", HelperOption.exchanges.Count );
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			#endregion

			// Use to be in the Main Form Load
			GLOBAL.HProcess.StartProcess();

			GLOBAL.InitializeROM();

			if (GLOBAL.ROCLoginCompleted)
			{
				GLOBAL.UserUIProfile.Load();

				try
				{
					// Allways load from ROM Database by default, this will automatically set to false are a sucessful shut down.
					//Configuration.User.Default.UseROMDatabase = true;
					//Configuration.User.Default.Save();

					if (GLOBAL.MainForm != null)
					{
						Application.Run(GLOBAL.HWindows.OpenWindow(GLOBAL.MainForm));
					}
					else
					{
						Application.Run(GLOBAL.HWindows.OpenWindow(new frmMain()));
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}

				GLOBAL.MDSsDisconnect();
			}
			else
			{
				if (GLOBAL.MDSsConnected)
				{
					GLOBAL.HROC.AddToStatusLogs("MDS Not Connected! ", GLOBAL.ROCVersion.ToString(), " Starting Failed." );
					//MessageBox.Show("Cannot Connect to One or All MDSs", "Start Up Failed");
				}
				else if (GLOBAL.HRDS.Status != HelperRDS.StatusTypes.Done)
				{
					GLOBAL.HROC.AddToStatusLogs("RDS Not Connected! ", GLOBAL.ROCVersion.ToString(), " Starting Failed." );
					//MessageBox.Show("Cannot Connect to RDS", "Start Up Failed");
				}
				
				GLOBAL.HROM.LogStatusMessages();
				GLOBAL.HROM.LogRomMessages();

				GLOBAL.HRDS.LogStatusMessages();
			}

			// Must ShutDown Every Time
			GLOBAL.HProcess.StopProcess();

			GLOBAL.HROC.AddToStatusLogs("Disconnect from ROM..." );
			GLOBAL.HROM.Disconnect();

			// Log last bit of ROM Messages
			GLOBAL.HROM.LogStatusMessages();

			// Log last bit of ROC Messages
			GLOBAL.HROC.LogStatusMessages();

			// Save Local User Settings
			HelperSettings.Save();

			GLOBAL.HROC.AddToStatusLogs("ROC Version ", GLOBAL.ROCVersion.ToString(), " Ending..." );
		}
	}
}