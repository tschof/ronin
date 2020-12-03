using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FormEx;

namespace FontDialogEx
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmPositions : VistaWindowBorder
	{
		public frmPositions()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			FitClicked += new EventHandler(frmPositions_FitClicked);
			FormClosing += new FormClosingEventHandler(frmPositions_FormClosing);

			rocPositions.Initialize();
		}

		private void frmPositions_FitClicked(object sender, EventArgs e)
		{
			SetColumnMode(rocPositions, DataGridViewAutoSizeColumnMode.AllCells);
			ResizeToFit(rocPositions);
			SetColumnMode(rocPositions, DataGridViewAutoSizeColumnMode.None);
		}

		private void frmPositions_FormClosing(object sender, FormClosingEventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}
	}
}