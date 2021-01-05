using System;
using System.Windows.Forms;

using FormEx;
using DataGridViewEx;
using RDSEx;
using System.Drawing;
using System.Collections.Generic;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmUserProfile : VistaWindowBorder
	{
		public frmUserProfile()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			FitClicked += new EventHandler(frmUserProfile_FitClicked);
			RefreshClicked += new EventHandler(frmUserProfile_RefreshClicked);
			HelpClicked += new EventHandler(frmUserProfile_HelpClicked);

			FormClosing += new FormClosingEventHandler(frmUserProfile_FormClosing);

			rocUserProfile.Initialize();
			frmUserProfile_RefreshClicked(null, EventArgs.Empty);
			frmUserProfile_FitClicked(null, EventArgs.Empty);
		}

		#region - Form Event -

		private void frmUserProfile_FitClicked(object sender, EventArgs e)
		{
			SetColumnMode(rocUserProfile, DataGridViewAutoSizeColumnMode.AllCells);
			ResizeToFit(rocUserProfile);
			SetColumnMode(rocUserProfile, DataGridViewAutoSizeColumnMode.None);
		}

		private void frmUserProfile_RefreshClicked(object sender, EventArgs e)
		{
			LoadUserProfile(rocUserProfile);
		}

		private void frmUserProfile_HelpClicked(object sender, EventArgs e)
		{
		}

		private void frmUserProfile_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}

		#endregion

		#region - Display User Profil -

		private void LoadUserProfile(DGVTree grid)
		{
			DGVTreeNode node;

			grid.Nodes.Clear();

			if (GLOBAL.HRDS.GotUserProfiles)
			{
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					node = grid.Nodes.Add(new object[] { "Trader", trader.tradeFor });
					node.DefaultCellStyle.Font = rocUserProfile.NodeFont;

					node.Nodes.Add(new object[] { "TradeFor", trader.tradeFor });
					node.Nodes.Add(new object[] { "LocalAcAcrn", trader.localAcAcrn });

					LoadAccounts(AccountTypes.Stock, node, trader.CSAccounts);
					LoadAccounts(AccountTypes.Option, node, trader.OPTAccounts);
					LoadAccounts(AccountTypes.Future, node, trader.FUTAccounts);
				}
			}
		}

		private void LoadAccounts(string type, DGVTreeNode node, Dictionary<string, AccountMap> accounts)
		{
			DGVTreeNode acctNode;
			DGVTreeNode destNode;

			foreach (AccountMap acctMap in accounts.Values)
			{
				acctNode = node.Nodes.Add(new object[] { type, acctMap.localAcAcrn });
				acctNode.DefaultCellStyle.Font = rocUserProfile.NodeFont;

				acctNode.Nodes.Add(new object[] { "InterfaceID", acctMap.interfaceID });
				acctNode.Nodes.Add(new object[] { "account", acctMap.account });
				acctNode.Nodes.Add(new object[] { "type", acctMap.type });
				acctNode.Nodes.Add(new object[] { "clearingAcID", acctMap.clearingAcID });
				acctNode.Nodes.Add(new object[] { "localAcAcrn", acctMap.localAcAcrn });
				acctNode.Nodes.Add(new object[] { "firmAcr", acctMap.firmAcr });
				acctNode.Nodes.Add(new object[] { "omAcctType", acctMap.omAcctType });
				acctNode.Nodes.Add(new object[] { "capacity", acctMap.capacity });
				acctNode.Nodes.Add(new object[] { "clearingFirmID", acctMap.clearingFirmID });
				acctNode.Nodes.Add(new object[] { "occAcr", acctMap.occAcr });
				acctNode.Nodes.Add(new object[] { "homeExchange", acctMap.homeExchange });

				foreach (DestinationMap destMap in acctMap.Destinations)
				{
					destNode = acctNode.Nodes.Add(new object[] { "Dest", destMap.shortName });
					destNode.DefaultCellStyle.Font = rocUserProfile.NodeFont;

					destNode.Nodes.Add(new object[] { "destID", destMap.destID });
					destNode.Nodes.Add(new object[] { "secType", destMap.secType });
					destNode.Nodes.Add(new object[] { "shortName", destMap.shortName });
					destNode.Nodes.Add(new object[] { "giveupFirmID", destMap.giveupFirmID });
					destNode.Nodes.Add(new object[] { "descmtaFirmIDtID", destMap.cmtaFirmID });
					destNode.Nodes.Add(new object[] { "algoFlag", destMap.algoFlag });
				}
			}
		}

		#endregion
	}
}