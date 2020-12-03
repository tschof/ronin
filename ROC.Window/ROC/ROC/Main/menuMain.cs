using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class menuMain : UserControl, INotifyPropertyChanged
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

		public menuMain()
		{
			InitializeComponent();

			SetStatus();
		}

		public void SetStatus()
		{
			if (GLOBAL.HROM.Status == HelperROM.StatusTypes.LoggedIn)
			{
				lblROMStatus.BackColor = Color.LimeGreen;
			}
			else
			{
				lblROMStatus.BackColor = Color.Red;
			}

			if (GLOBAL.HRDS.Status != HelperRDS.StatusTypes.GotError &&
				GLOBAL.HRDS.Status != HelperRDS.StatusTypes.Failed)
			{
				lblRDSStatus.BackColor = Color.LimeGreen;
			}
			else
			{
				lblRDSStatus.BackColor = Color.Yellow;
			}

			if (GLOBAL.UseMDSBackup)
			{
				cmdMDS.Text = "MDS.BAK";
			}
			else
			{
				cmdMDS.Text = "MDS";
			}

			if (Configuration.User.Default.UseMarketData)
			{
				cmdMDS.Enabled = true;
				lblMDSStatus.Visible = true;

				if (GLOBAL.MDSsConnected)
				{
					lblMDSStatus.BackColor = Color.LimeGreen;
				}
				else
				{
					lblMDSStatus.BackColor = Color.Yellow;
				}
			}
			else
			{
				cmdMDS.Enabled = false;
				lblMDSStatus.Visible = false;
			}
		}

		private void cmdROM_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("ROM"));
		}

		private void cmdRDS_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("RDS"));
		}

		private void cmdMDS_Click(object sender, EventArgs e)
		{
			if (Configuration.User.Default.UseMarketData)
			{
				onPropertyChanged(new PropertyChangedEventArgs("MDS"));
			}
		}

		private void cmdUserProfile_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("UserProfile"));
		}

		private void cmdIM_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("IM"));
		}

		private void cmdAlerts_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Alerts"));
		}

		private void cmdClock_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Clock"));
		}

		private void cmdSettings_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Settings"));
		}

		private void cmdZeroAllWindows_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("ZeroAll"));
		}

		private void cmdAbout_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("About"));
		}
	}
}
