using System;
using System.Windows.Forms;
using System.Collections.Generic;

using Common;
using FormEx;
using DataGridViewEx;
using RDSEx;

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
				foreach ((string _, TraderMap trader) in GLOBAL.HRDS.UserProfiles)
				{
					node = grid.Nodes.Add("Trader", trader.TradeFor);
					node.DefaultCellStyle.Font = rocUserProfile.NodeFont;

					node.Nodes.Add("TradeFor", trader.TradeFor);
					node.Nodes.Add("LocalAcAcrn", trader.LocalAcAcrn );

					LoadAccounts(TraderMap.AccountTypes.Stock, node, trader.CSAccounts);
					LoadAccounts(TraderMap.AccountTypes.Option, node, trader.OPTAccounts);
					LoadAccounts(TraderMap.AccountTypes.Future, node, trader.FUTAccounts);
				}
			}
		}

		private void LoadAccounts(string type, DGVTreeNode node, Dictionary<string, AccountMap> accounts)
		{
			DGVTreeNode acctNode;
			DGVTreeNode destNode;

			foreach ((string _, AccountMap acctMap) in accounts)
			{
				acctNode = node.Nodes.Add(type, acctMap.localAcAcrn);
				acctNode.DefaultCellStyle.Font = rocUserProfile.NodeFont;

				acctNode.Nodes.Add("InterfaceID", acctMap.interfaceID);
				acctNode.Nodes.Add("account", acctMap.account);
				acctNode.Nodes.Add("type", acctMap.type);
				acctNode.Nodes.Add("clearingAcID", acctMap.clearingAcID);
				acctNode.Nodes.Add("localAcAcrn", acctMap.localAcAcrn);
				acctNode.Nodes.Add("firmAcr", acctMap.firmAcr);
				acctNode.Nodes.Add("omAcctType", acctMap.omAcctType);
				acctNode.Nodes.Add("capacity", acctMap.capacity);
				acctNode.Nodes.Add("clearingFirmID", acctMap.clearingFirmID);
				acctNode.Nodes.Add("occAcr", acctMap.occAcr);
				acctNode.Nodes.Add("homeExchange", acctMap.homeExchange);

				foreach (DestinationMap destMap in acctMap.Destinations)
				{
					destNode = acctNode.Nodes.Add("Dest", destMap.shortName);
					destNode.DefaultCellStyle.Font = rocUserProfile.NodeFont;

					destNode.Nodes.Add("destID", destMap.destID);
					destNode.Nodes.Add("secType", destMap.secType);
					destNode.Nodes.Add("shortName", destMap.shortName);
					destNode.Nodes.Add("giveupFirmID", destMap.giveupFirmID);
					destNode.Nodes.Add("descmtaFirmIDtID", destMap.cmtaFirmID);
					destNode.Nodes.Add("algoFlag", destMap.algoFlag);
				}
			}
		}

		#endregion
	}
}