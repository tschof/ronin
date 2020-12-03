using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	public partial class itemBaseTicker : UserControl
	{
		public itemBaseTicker()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			panelManagerTicker.SelectedPanel = managedPanelMain;

			SymbolTradePair.PropertyChanged += new PropertyChangedEventHandler(SymbolTradePair_PropertyChanged);
		}

		private void SymbolTradePair_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "SymbolToAdd":
					break;
				case "SymbolToRemove":
					break;
				case "SymbolToSubscribe":
					break;
			}
		}
	}
}
