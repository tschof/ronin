using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

namespace ROC
{
	internal class HelperAutomationTimer : INotifyPropertyChanged
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

		#region - Property With Events -

		private bool _autoEvent = false;
		public bool AutoEvent
		{
			get
			{
				return _autoEvent;
			}
			set
			{
				_autoEvent = value;
				if (_autoEvent == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs(_type));
				}
			}
		}

		#endregion

		#region - Local Variable -

		private Timer _autoTimer = null;
		private DateTime _lastSechduledTime = new DateTime();
		private string _type = "";

		#endregion

		public HelperAutomationTimer(string type)
		{
			_type = type;
		}

		internal void CheckAutoTimer(DateTime sechduledTime)
		{
			if (((TimeSpan)_lastSechduledTime.Subtract(sechduledTime)).TotalMilliseconds != 0)
			{
				_lastSechduledTime = sechduledTime;

				// Reset Auto Timer
				AutoTimer_Stop();
				AutoTimer_Start();
			}
		}

		private void AutoTimer_Start()
		{
			TimeSpan ts = _lastSechduledTime.Subtract(DateTime.Now);
			if (ts.TotalMilliseconds <= 0)
			{
				// For Tomorrow
				ts = _lastSechduledTime.AddDays(1).Subtract(DateTime.Now);
			}

			if (ts.TotalMilliseconds > 0)
			{
				if (_autoTimer == null)
				{
					_autoTimer = new Timer();
					_autoTimer.Interval = (int)ts.TotalMilliseconds;
					_autoTimer.Tick += new EventHandler(AutoTimer_Tick);
					_autoTimer.Enabled = true;
				}
			}
		}

		private void AutoTimer_Stop()
		{
			if (_autoTimer != null)
			{
				_autoTimer.Stop();
				_autoTimer.Tick -= new EventHandler(AutoTimer_Tick);
			}
			_autoTimer = null;
		}

		private void AutoTimer_Tick(object sender, EventArgs e)
		{
			AutoTimer_Stop();

			AutoEvent = true;

			AutoTimer_Start();
		}
	}
}
