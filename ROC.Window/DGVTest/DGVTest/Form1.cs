using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using GridEx;

namespace DGVTest
{
	public partial class Form1 : Form
	{
		private Timer _uiTimer;

		public Form1()
		{
			InitializeComponent();

			dgvEx1.SetWatchListGrid();
			for (int i = 0; i < 1000; i++)
			{
				dgvEx1.WatchList.Add(new WatchListData());
			}
			StartTimer();
		}

		private void StartTimer()
		{
			if (_uiTimer == null)
			{
				_uiTimer = new Timer();
				_uiTimer.Tick += new EventHandler(uiTimer_Tick);
				_uiTimer.Interval = 100;
				_uiTimer.Enabled = true;
			}
		}

		private void StopTimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Stop();
				_uiTimer.Tick -= new EventHandler(uiTimer_Tick);
				_uiTimer = null;
			}
		}

		private Random r = new Random();
		private void uiTimer_Tick(object sender, EventArgs e)
		{
			foreach (WatchListData item in dgvEx1.WatchList)
			{
				item.Bid = Math.Round(r.NextDouble(), 7);
				item.Ask = Math.Round(r.NextDouble(), 7);
				item.Last = Math.Round(r.NextDouble(), 7);
			}
		}
	}
}