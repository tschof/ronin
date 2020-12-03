namespace ROC
{
	partial class menuBaseNotify
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.cmdExit = new ButtonEx.VistaButton();
			this.cmdShowAll = new ButtonEx.VistaButton();
			this.cmdPositions = new ButtonEx.VistaButton();
			this.cmdTrades = new ButtonEx.VistaButton();
			this.cmdOrders = new ButtonEx.VistaButton();
			this.cmdBatchTicket = new ButtonEx.VistaButton();
			this.cmdQuickTradeTicket = new ButtonEx.VistaButton();
			this.cmdFutureTicket = new ButtonEx.VistaButton();
			this.cmdOptionTicket = new ButtonEx.VistaButton();
			this.cmdStockTicket = new ButtonEx.VistaButton();
			this.cmdPlotList = new ButtonEx.VistaButton();
			this.cmdWatchList = new ButtonEx.VistaButton();
			this.panelROCMenu = new PanelEx.VistaPanel();
			this.dspMainMenu = new LabelEx.LabelRotatable();
			this.cmdHideAll = new ButtonEx.VistaButton();
			this.panelROCMenu.SuspendLayout();
			this.SuspendLayout();
			// 
			// cmdExit
			// 
			this.cmdExit.BackColor = System.Drawing.Color.Black;
			this.cmdExit.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdExit.Location = new System.Drawing.Point(22, 276);
			this.cmdExit.Name = "cmdExit";
			this.cmdExit.Size = new System.Drawing.Size(108, 23);
			this.cmdExit.TabIndex = 12;
			this.cmdExit.Text = "Exit";
			this.cmdExit.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdExit.Click += new System.EventHandler(this.cmdExit_Click);
			// 
			// cmdShowAll
			// 
			this.cmdShowAll.BackColor = System.Drawing.Color.DarkGreen;
			this.cmdShowAll.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdShowAll.Location = new System.Drawing.Point(22, 230);
			this.cmdShowAll.Name = "cmdShowAll";
			this.cmdShowAll.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterIn;
			this.cmdShowAll.Size = new System.Drawing.Size(108, 23);
			this.cmdShowAll.TabIndex = 11;
			this.cmdShowAll.Text = "Show All";
			this.cmdShowAll.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdShowAll.UserSplitSize = 20;
			this.cmdShowAll.Click += new System.EventHandler(this.cmdShowAll_Click);
			// 
			// cmdPositions
			// 
			this.cmdPositions.BackColor = System.Drawing.Color.Black;
			this.cmdPositions.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdPositions.Location = new System.Drawing.Point(22, 207);
			this.cmdPositions.Name = "cmdPositions";
			this.cmdPositions.Size = new System.Drawing.Size(108, 23);
			this.cmdPositions.TabIndex = 10;
			this.cmdPositions.Text = "Positions";
			this.cmdPositions.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdPositions.Click += new System.EventHandler(this.cmdPositions_Click);
			// 
			// cmdTrades
			// 
			this.cmdTrades.BackColor = System.Drawing.Color.Black;
			this.cmdTrades.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdTrades.Location = new System.Drawing.Point(22, 184);
			this.cmdTrades.Name = "cmdTrades";
			this.cmdTrades.Size = new System.Drawing.Size(108, 23);
			this.cmdTrades.TabIndex = 13;
			this.cmdTrades.Text = "Trades";
			this.cmdTrades.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdTrades.Click += new System.EventHandler(this.cmdTrades_Click);
			// 
			// cmdOrders
			// 
			this.cmdOrders.BackColor = System.Drawing.Color.Black;
			this.cmdOrders.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdOrders.Location = new System.Drawing.Point(22, 161);
			this.cmdOrders.Name = "cmdOrders";
			this.cmdOrders.Size = new System.Drawing.Size(108, 23);
			this.cmdOrders.TabIndex = 9;
			this.cmdOrders.Text = "Orders";
			this.cmdOrders.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdOrders.Click += new System.EventHandler(this.cmdOrders_Click);
			// 
			// cmdBatchTicket
			// 
			this.cmdBatchTicket.BackColor = System.Drawing.Color.Black;
			this.cmdBatchTicket.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdBatchTicket.Location = new System.Drawing.Point(22, 138);
			this.cmdBatchTicket.Name = "cmdBatchTicket";
			this.cmdBatchTicket.Size = new System.Drawing.Size(108, 23);
			this.cmdBatchTicket.TabIndex = 8;
			this.cmdBatchTicket.Text = "Batch Ticket";
			this.cmdBatchTicket.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdBatchTicket.Click += new System.EventHandler(this.cmdBatchTicket_Click);
			// 
			// cmdQuickTradeTicket
			// 
			this.cmdQuickTradeTicket.BackColor = System.Drawing.Color.Black;
			this.cmdQuickTradeTicket.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdQuickTradeTicket.Location = new System.Drawing.Point(22, 115);
			this.cmdQuickTradeTicket.Name = "cmdQuickTradeTicket";
			this.cmdQuickTradeTicket.Size = new System.Drawing.Size(108, 23);
			this.cmdQuickTradeTicket.TabIndex = 7;
			this.cmdQuickTradeTicket.Text = "Quick Trade Ticket";
			this.cmdQuickTradeTicket.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdQuickTradeTicket.Click += new System.EventHandler(this.cmdQuickTradeTicket_Click);
			// 
			// cmdFutureTicket
			// 
			this.cmdFutureTicket.BackColor = System.Drawing.Color.Black;
			this.cmdFutureTicket.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdFutureTicket.Location = new System.Drawing.Point(22, 92);
			this.cmdFutureTicket.Name = "cmdFutureTicket";
			this.cmdFutureTicket.Size = new System.Drawing.Size(108, 23);
			this.cmdFutureTicket.TabIndex = 6;
			this.cmdFutureTicket.Text = "Future Ticket";
			this.cmdFutureTicket.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdFutureTicket.Click += new System.EventHandler(this.cmdFutureTicket_Click);
			// 
			// cmdOptionTicket
			// 
			this.cmdOptionTicket.BackColor = System.Drawing.Color.Black;
			this.cmdOptionTicket.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdOptionTicket.Location = new System.Drawing.Point(22, 69);
			this.cmdOptionTicket.Name = "cmdOptionTicket";
			this.cmdOptionTicket.Size = new System.Drawing.Size(108, 23);
			this.cmdOptionTicket.TabIndex = 5;
			this.cmdOptionTicket.Text = "Option Ticket";
			this.cmdOptionTicket.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdOptionTicket.Click += new System.EventHandler(this.cmdOptionTicket_Click);
			// 
			// cmdStockTicket
			// 
			this.cmdStockTicket.BackColor = System.Drawing.Color.Black;
			this.cmdStockTicket.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdStockTicket.Location = new System.Drawing.Point(22, 46);
			this.cmdStockTicket.Name = "cmdStockTicket";
			this.cmdStockTicket.Size = new System.Drawing.Size(108, 23);
			this.cmdStockTicket.TabIndex = 4;
			this.cmdStockTicket.Text = "Stock Ticket";
			this.cmdStockTicket.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdStockTicket.Click += new System.EventHandler(this.cmdStockTicket_Click);
			// 
			// cmdPlotList
			// 
			this.cmdPlotList.BackColor = System.Drawing.Color.Black;
			this.cmdPlotList.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdPlotList.Location = new System.Drawing.Point(22, 23);
			this.cmdPlotList.Name = "cmdPlotList";
			this.cmdPlotList.Size = new System.Drawing.Size(108, 23);
			this.cmdPlotList.TabIndex = 3;
			this.cmdPlotList.Text = "Plot List";
			this.cmdPlotList.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdPlotList.Click += new System.EventHandler(this.cmdPlotList_Click);
			// 
			// cmdWatchList
			// 
			this.cmdWatchList.BackColor = System.Drawing.Color.Black;
			this.cmdWatchList.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdWatchList.Location = new System.Drawing.Point(22, 0);
			this.cmdWatchList.Name = "cmdWatchList";
			this.cmdWatchList.Size = new System.Drawing.Size(108, 23);
			this.cmdWatchList.TabIndex = 2;
			this.cmdWatchList.Text = "Watch List";
			this.cmdWatchList.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdWatchList.Click += new System.EventHandler(this.cmdWatchList_Click);
			// 
			// panelROCMenu
			// 
			this.panelROCMenu.BackColor = System.Drawing.Color.Black;
			this.panelROCMenu.Controls.Add(this.dspMainMenu);
			this.panelROCMenu.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelROCMenu.Location = new System.Drawing.Point(0, 0);
			this.panelROCMenu.Name = "panelROCMenu";
			this.panelROCMenu.Padding = new System.Windows.Forms.Padding(5);
			this.panelROCMenu.Size = new System.Drawing.Size(22, 299);
			this.panelROCMenu.TabIndex = 1;
			// 
			// dspMainMenu
			// 
			this.dspMainMenu.AutoColor = false;
			this.dspMainMenu.DefaultColor = System.Drawing.Color.White;
			this.dspMainMenu.DefaultText = "";
			this.dspMainMenu.DisplayFactor = 1;
			this.dspMainMenu.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.dspMainMenu.DownColor = System.Drawing.Color.Red;
			this.dspMainMenu.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspMainMenu.IsCurrency = false;
			this.dspMainMenu.IsFraction = false;
			this.dspMainMenu.Location = new System.Drawing.Point(5, 229);
			this.dspMainMenu.MaxDecimal = 7;
			this.dspMainMenu.Name = "dspMainMenu";
			this.dspMainMenu.RotationAngle = 270;
			this.dspMainMenu.Size = new System.Drawing.Size(12, 65);
			this.dspMainMenu.TabIndex = 0;
			this.dspMainMenu.Text = "Main Menu";
			this.dspMainMenu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspMainMenu.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.dspMainMenu.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.dspMainMenu.TickSize = 0.01;
			this.dspMainMenu.UpColor = System.Drawing.Color.LimeGreen;
			this.dspMainMenu.Value = "";
			// 
			// cmdHideAll
			// 
			this.cmdHideAll.BackColor = System.Drawing.Color.Maroon;
			this.cmdHideAll.Dock = System.Windows.Forms.DockStyle.Top;
			this.cmdHideAll.Location = new System.Drawing.Point(22, 253);
			this.cmdHideAll.Name = "cmdHideAll";
			this.cmdHideAll.ROCButtonType = ButtonEx.VistaButton.ROCButtonTypes.FilterOut;
			this.cmdHideAll.Size = new System.Drawing.Size(108, 23);
			this.cmdHideAll.TabIndex = 14;
			this.cmdHideAll.Text = "Hide All";
			this.cmdHideAll.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.cmdHideAll.UserSplitSize = 20;
			this.cmdHideAll.Click += new System.EventHandler(this.cmdHideAll_Click);
			// 
			// menuBaseNotify
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.cmdExit);
			this.Controls.Add(this.cmdHideAll);
			this.Controls.Add(this.cmdShowAll);
			this.Controls.Add(this.cmdPositions);
			this.Controls.Add(this.cmdTrades);
			this.Controls.Add(this.cmdOrders);
			this.Controls.Add(this.cmdBatchTicket);
			this.Controls.Add(this.cmdQuickTradeTicket);
			this.Controls.Add(this.cmdFutureTicket);
			this.Controls.Add(this.cmdOptionTicket);
			this.Controls.Add(this.cmdStockTicket);
			this.Controls.Add(this.cmdPlotList);
			this.Controls.Add(this.cmdWatchList);
			this.Controls.Add(this.panelROCMenu);
			this.MinimumSize = new System.Drawing.Size(130, 299);
			this.Name = "menuBaseNotify";
			this.Size = new System.Drawing.Size(130, 299);
			this.panelROCMenu.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private LabelEx.LabelRotatable dspMainMenu;
		private PanelEx.VistaPanel panelROCMenu;
		private ButtonEx.VistaButton cmdWatchList;
		private ButtonEx.VistaButton cmdPlotList;
		private ButtonEx.VistaButton cmdStockTicket;
		private ButtonEx.VistaButton cmdOptionTicket;
		private ButtonEx.VistaButton cmdFutureTicket;
		private ButtonEx.VistaButton cmdQuickTradeTicket;
		private ButtonEx.VistaButton cmdBatchTicket;
		private ButtonEx.VistaButton cmdOrders;
		private ButtonEx.VistaButton cmdPositions;
		private ButtonEx.VistaButton cmdShowAll;
		private ButtonEx.VistaButton cmdExit;
		private ButtonEx.VistaButton cmdTrades;
		private ButtonEx.VistaButton cmdHideAll;


	}
}
