using System;
using System.Windows.Forms;

using FormEx;
using System.ComponentModel;

using DataGridViewEx;
using System.Collections.Generic;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmAlert : VistaWindowBorder
	{
		public frmAlert()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			FitClicked += new EventHandler(frmAlert_FitClicked);
			RefreshClicked += new EventHandler(frmAlert_RefreshClicked);
			HelpClicked += new EventHandler(frmAlert_HelpClicked);

			FormClosing += new FormClosingEventHandler(frmAlert_FormClosing);

			rocAlertList.Initialize();
		}

		#region - Form Events -

		private void frmAlert_FitClicked(object sender, EventArgs e)
		{
			SetColumnMode(rocAlertList, DataGridViewAutoSizeColumnMode.AllCells);
			ResizeToFit(rocAlertList);
			SetColumnMode(rocAlertList, DataGridViewAutoSizeColumnMode.None);
		}

		private void frmAlert_RefreshClicked(object sender, EventArgs e)
		{
		}

		private void frmAlert_HelpClicked(object sender, EventArgs e)
		{
		}

		private void frmAlert_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}

		#endregion

		#region - Binding -

		public void UpdateAlert(System.Collections.IEnumerable alerts)
		{
			bool empty = rocAlertList.ROCGridData.Count == 0;

			foreach (AlertListData alert in alerts)
				rocAlertList.ROCGridData.Add(alert);

			if (empty && (rocAlertList.ROCGridData.Count > 0))
				frmAlert_FitClicked(null, EventArgs.Empty); // Was empty, now has alerts.

			rocAlertList.ScrollToLastRow();
		}

		#endregion
	}
}