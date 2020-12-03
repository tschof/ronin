namespace ROC
{
	partial class frmMain
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

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
			this.notifyIconROC = new System.Windows.Forms.NotifyIcon(this.components);
			this.cmdROC = new ButtonEx.VistaButton();
			this.lblCaption = new LabelEx.LabelVista();
			this.cmdTrades = new ButtonEx.VistaButton();
			this.cmdPositions = new ButtonEx.VistaButton();
			this.cmdOrders = new ButtonEx.VistaButton();
			this.cmdPlotList = new ButtonEx.VistaButton();
			this.cmdClose = new ButtonEx.VistaButton();
			this.imageListClose = new System.Windows.Forms.ImageList(this.components);
			this.cmdMinimize = new ButtonEx.VistaButton();
			this.imageListMinimize = new System.Windows.Forms.ImageList(this.components);
			this.cmdWatchList = new ButtonEx.VistaButton();
			this.lblCaptionEnd = new LabelEx.LabelVista();
			this.cmdWindows = new ButtonEx.VistaButton();
			this.cmdFiles = new ButtonEx.VistaButton();
			this.cmdCustomTickets = new ButtonEx.VistaButton();
			this.cmdStandardTickets = new ButtonEx.VistaButton();
			this.SuspendLayout();
			// 
			// notifyIconROC
			// 
			this.notifyIconROC.BalloonTipIcon = System.Windows.Forms.ToolTipIcon.Info;
			this.notifyIconROC.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIconROC.Icon")));
			this.notifyIconROC.Text = "ROC";
			this.notifyIconROC.MouseMove += new System.Windows.Forms.MouseEventHandler(this.notifyIconROC_ShowMenu);
			this.notifyIconROC.MouseClick += new System.Windows.Forms.MouseEventHandler(this.notifyIconROC_ShowMenu);
			this.notifyIconROC.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIconROC_MouseDoubleClick);
			// 
			// cmdROC
			// 
			this.cmdROC.BackColor = System.Drawing.Color.Black;
			this.cmdROC.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdROC.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdROC.Location = new System.Drawing.Point(0, 0);
			this.cmdROC.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdROC.Name = "cmdROC";
			this.cmdROC.Size = new System.Drawing.Size(60, 26);
			this.cmdROC.TabIndex = 31;
			this.cmdROC.Text = "ROC";
			this.cmdROC.UserSplitSize = 20;
			this.cmdROC.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdROC.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdROC.MouseSplitDown += new System.EventHandler(this.cmdROC_Click);
			this.cmdROC.Click += new System.EventHandler(this.cmdROC_Click);
			// 
			// lblCaption
			// 
			this.lblCaption.Animate = true;
			this.lblCaption.AutoColor = false;
			this.lblCaption.DefaultColor = System.Drawing.Color.White;
			this.lblCaption.DefaultText = "";
			this.lblCaption.DisplayFactor = 1;
			this.lblCaption.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblCaption.DownColor = System.Drawing.Color.Red;
			this.lblCaption.IsCurrency = false;
			this.lblCaption.IsFraction = false;
			this.lblCaption.IsPercent = false;
			this.lblCaption.Location = new System.Drawing.Point(60, 0);
			this.lblCaption.MaxDecimal = 7;
			this.lblCaption.MinimumSize = new System.Drawing.Size(26, 26);
			this.lblCaption.Name = "lblCaption";
			this.lblCaption.Size = new System.Drawing.Size(60, 26);
			this.lblCaption.TabIndex = 61;
			this.lblCaption.TickSize = 0.01;
			this.lblCaption.UpColor = System.Drawing.Color.LimeGreen;
			this.lblCaption.Value = "";
			this.lblCaption.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.lblCaption.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.lblCaption.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ToolBarMove_MouseDown);
			// 
			// cmdTrades
			// 
			this.cmdTrades.AutoSize = true;
			this.cmdTrades.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdTrades.BackColor = System.Drawing.Color.Black;
			this.cmdTrades.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdTrades.Location = new System.Drawing.Point(590, 0);
			this.cmdTrades.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdTrades.Name = "cmdTrades";
			this.cmdTrades.Size = new System.Drawing.Size(50, 26);
			this.cmdTrades.TabIndex = 73;
			this.cmdTrades.Text = "Trades";
			this.cmdTrades.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdTrades.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdTrades.Click += new System.EventHandler(this.cmdTrades_Click);
			// 
			// cmdPositions
			// 
			this.cmdPositions.AutoSize = true;
			this.cmdPositions.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdPositions.BackColor = System.Drawing.Color.Black;
			this.cmdPositions.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdPositions.Location = new System.Drawing.Point(640, 0);
			this.cmdPositions.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdPositions.Name = "cmdPositions";
			this.cmdPositions.Size = new System.Drawing.Size(59, 26);
			this.cmdPositions.TabIndex = 72;
			this.cmdPositions.Text = "Positions";
			this.cmdPositions.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdPositions.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdPositions.Click += new System.EventHandler(this.cmdPositions_Click);
			// 
			// cmdOrders
			// 
			this.cmdOrders.AutoSize = true;
			this.cmdOrders.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdOrders.BackColor = System.Drawing.Color.Black;
			this.cmdOrders.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdOrders.Location = new System.Drawing.Point(542, 0);
			this.cmdOrders.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdOrders.Name = "cmdOrders";
			this.cmdOrders.Size = new System.Drawing.Size(48, 26);
			this.cmdOrders.TabIndex = 71;
			this.cmdOrders.Text = "Orders";
			this.cmdOrders.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdOrders.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdOrders.Click += new System.EventHandler(this.cmdOrders_Click);
			// 
			// cmdPlotList
			// 
			this.cmdPlotList.AutoSize = true;
			this.cmdPlotList.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdPlotList.BackColor = System.Drawing.Color.Black;
			this.cmdPlotList.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdPlotList.Location = new System.Drawing.Point(248, 0);
			this.cmdPlotList.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdPlotList.Name = "cmdPlotList";
			this.cmdPlotList.Size = new System.Drawing.Size(54, 26);
			this.cmdPlotList.TabIndex = 66;
			this.cmdPlotList.Text = "Plot List";
			this.cmdPlotList.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdPlotList.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdPlotList.Click += new System.EventHandler(this.cmdPlotList_Click);
			// 
			// cmdClose
			// 
			this.cmdClose.AutoSize = true;
			this.cmdClose.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdClose.BackColor = System.Drawing.Color.Black;
			this.cmdClose.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdClose.ImageIndex = 1;
			this.cmdClose.ImageList = this.imageListClose;
			this.cmdClose.Location = new System.Drawing.Point(924, 0);
			this.cmdClose.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdClose.Name = "cmdClose";
			this.cmdClose.Size = new System.Drawing.Size(26, 26);
			this.cmdClose.TabIndex = 77;
			this.cmdClose.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdClose.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdClose.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdClose.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			this.cmdClose.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdClose.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			// 
			// imageListClose
			// 
			this.imageListClose.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListClose.ImageStream")));
			this.imageListClose.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListClose.Images.SetKeyName(0, "Close.png");
			this.imageListClose.Images.SetKeyName(1, "CloseDisabled.png");
			this.imageListClose.Images.SetKeyName(2, "ClosePressed.png");
			this.imageListClose.Images.SetKeyName(3, "CloseHot.png");
			// 
			// cmdMinimize
			// 
			this.cmdMinimize.AutoSize = true;
			this.cmdMinimize.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdMinimize.BackColor = System.Drawing.Color.Black;
			this.cmdMinimize.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdMinimize.ImageIndex = 1;
			this.cmdMinimize.ImageList = this.imageListMinimize;
			this.cmdMinimize.Location = new System.Drawing.Point(898, 0);
			this.cmdMinimize.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdMinimize.Name = "cmdMinimize";
			this.cmdMinimize.Size = new System.Drawing.Size(26, 26);
			this.cmdMinimize.TabIndex = 79;
			this.cmdMinimize.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdMinimize.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdMinimize.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdMinimize.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			this.cmdMinimize.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdMinimize.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			// 
			// imageListMinimize
			// 
			this.imageListMinimize.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListMinimize.ImageStream")));
			this.imageListMinimize.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListMinimize.Images.SetKeyName(0, "Minimize.png");
			this.imageListMinimize.Images.SetKeyName(1, "MinimizeDisabled.png");
			this.imageListMinimize.Images.SetKeyName(2, "MinimizePressed.png");
			this.imageListMinimize.Images.SetKeyName(3, "MinimizeHot.png");
			// 
			// cmdWatchList
			// 
			this.cmdWatchList.AutoSize = true;
			this.cmdWatchList.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdWatchList.BackColor = System.Drawing.Color.Black;
			this.cmdWatchList.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdWatchList.Location = new System.Drawing.Point(180, 0);
			this.cmdWatchList.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdWatchList.Name = "cmdWatchList";
			this.cmdWatchList.Size = new System.Drawing.Size(68, 26);
			this.cmdWatchList.TabIndex = 82;
			this.cmdWatchList.Text = "Watch List";
			this.cmdWatchList.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdWatchList.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdWatchList.Click += new System.EventHandler(this.cmdWatchList_Click);
			// 
			// lblCaptionEnd
			// 
			this.lblCaptionEnd.Animate = true;
			this.lblCaptionEnd.AutoColor = false;
			this.lblCaptionEnd.DefaultColor = System.Drawing.Color.White;
			this.lblCaptionEnd.DefaultText = "";
			this.lblCaptionEnd.DisplayFactor = 1;
			this.lblCaptionEnd.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblCaptionEnd.DownColor = System.Drawing.Color.Red;
			this.lblCaptionEnd.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblCaptionEnd.IsCurrency = false;
			this.lblCaptionEnd.IsFraction = false;
			this.lblCaptionEnd.IsPercent = false;
			this.lblCaptionEnd.Location = new System.Drawing.Point(779, 0);
			this.lblCaptionEnd.MaxDecimal = 7;
			this.lblCaptionEnd.MinimumSize = new System.Drawing.Size(26, 26);
			this.lblCaptionEnd.Name = "lblCaptionEnd";
			this.lblCaptionEnd.Padding = new System.Windows.Forms.Padding(2);
			this.lblCaptionEnd.Size = new System.Drawing.Size(119, 26);
			this.lblCaptionEnd.TabIndex = 78;
			this.lblCaptionEnd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.lblCaptionEnd.TickSize = 0.01;
			this.lblCaptionEnd.UpColor = System.Drawing.Color.LimeGreen;
			this.lblCaptionEnd.Value = "";
			this.lblCaptionEnd.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.lblCaptionEnd.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.lblCaptionEnd.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ToolBarMove_MouseDown);
			// 
			// cmdWindows
			// 
			this.cmdWindows.BackColor = System.Drawing.Color.Black;
			this.cmdWindows.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdWindows.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdWindows.Location = new System.Drawing.Point(699, 0);
			this.cmdWindows.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdWindows.Name = "cmdWindows";
			this.cmdWindows.Size = new System.Drawing.Size(80, 26);
			this.cmdWindows.TabIndex = 83;
			this.cmdWindows.Text = "Windows";
			this.cmdWindows.UserSplitSize = 20;
			this.cmdWindows.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdWindows.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdWindows.MouseSplitDown += new System.EventHandler(this.cmdWindows_Click);
			this.cmdWindows.Click += new System.EventHandler(this.cmdWindows_Click);
			// 
			// cmdFiles
			// 
			this.cmdFiles.BackColor = System.Drawing.Color.Black;
			this.cmdFiles.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdFiles.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdFiles.Location = new System.Drawing.Point(120, 0);
			this.cmdFiles.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdFiles.Name = "cmdFiles";
			this.cmdFiles.Size = new System.Drawing.Size(60, 26);
			this.cmdFiles.TabIndex = 84;
			this.cmdFiles.Text = "Files";
			this.cmdFiles.UserSplitSize = 20;
			this.cmdFiles.MouseEnter += new System.EventHandler(this.ToolBarActive_MouseEnter);
			this.cmdFiles.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdFiles.MouseSplitDown += new System.EventHandler(this.cmdFiles_Click);
			this.cmdFiles.Click += new System.EventHandler(this.cmdFiles_Click);
			// 
			// cmdCustomTickets
			// 
			this.cmdCustomTickets.AutoSize = true;
			this.cmdCustomTickets.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCustomTickets.BackColor = System.Drawing.Color.Black;
			this.cmdCustomTickets.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdCustomTickets.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdCustomTickets.Location = new System.Drawing.Point(427, 0);
			this.cmdCustomTickets.MinimumSize = new System.Drawing.Size(115, 26);
			this.cmdCustomTickets.Name = "cmdCustomTickets";
			this.cmdCustomTickets.Size = new System.Drawing.Size(115, 26);
			this.cmdCustomTickets.TabIndex = 86;
			this.cmdCustomTickets.Text = "Custom Tickets";
			this.cmdCustomTickets.UserSplitSize = 20;
			this.cmdCustomTickets.MouseSplitClick += new System.EventHandler(this.cmdCustomTickets_Click);
			this.cmdCustomTickets.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdCustomTickets.MouseSplitDown += new System.EventHandler(this.cmdCustomTickets_Click);
			this.cmdCustomTickets.Click += new System.EventHandler(this.cmdCustomTickets_Click);
			// 
			// cmdStandardTickets
			// 
			this.cmdStandardTickets.AutoSize = true;
			this.cmdStandardTickets.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdStandardTickets.BackColor = System.Drawing.Color.Black;
			this.cmdStandardTickets.ButtonType = ButtonEx.VistaButton.VistaButtonTypes.Split;
			this.cmdStandardTickets.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdStandardTickets.Location = new System.Drawing.Point(302, 0);
			this.cmdStandardTickets.MinimumSize = new System.Drawing.Size(125, 26);
			this.cmdStandardTickets.Name = "cmdStandardTickets";
			this.cmdStandardTickets.Size = new System.Drawing.Size(125, 26);
			this.cmdStandardTickets.TabIndex = 87;
			this.cmdStandardTickets.Text = "Standard Tickets";
			this.cmdStandardTickets.UserSplitSize = 20;
			this.cmdStandardTickets.MouseSplitClick += new System.EventHandler(this.cmdStandardTickets_Click);
			this.cmdStandardTickets.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.cmdStandardTickets.MouseSplitDown += new System.EventHandler(this.cmdStandardTickets_Click);
			this.cmdStandardTickets.Click += new System.EventHandler(this.cmdStandardTickets_Click);
			// 
			// frmMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.ClientSize = new System.Drawing.Size(950, 26);
			this.Controls.Add(this.lblCaptionEnd);
			this.Controls.Add(this.cmdWindows);
			this.Controls.Add(this.cmdPositions);
			this.Controls.Add(this.cmdTrades);
			this.Controls.Add(this.cmdOrders);
			this.Controls.Add(this.cmdCustomTickets);
			this.Controls.Add(this.cmdMinimize);
			this.Controls.Add(this.cmdClose);
			this.Controls.Add(this.cmdStandardTickets);
			this.Controls.Add(this.cmdPlotList);
			this.Controls.Add(this.cmdWatchList);
			this.Controls.Add(this.cmdFiles);
			this.Controls.Add(this.lblCaption);
			this.Controls.Add(this.cmdROC);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximumSize = new System.Drawing.Size(950, 26);
			this.MinimumSize = new System.Drawing.Size(950, 3);
			this.Name = "frmMain";
			this.OnTop = true;
			this.ROCWindowType = FormEx.VistaWindowBase.ROCWindowTypes.Main;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "ROC";
			this.TopMost = true;
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_MouseMove);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private ButtonEx.VistaButton cmdROC;
		private LabelEx.LabelVista lblCaption;
		private ButtonEx.VistaButton cmdTrades;
		private ButtonEx.VistaButton cmdPositions;
		private ButtonEx.VistaButton cmdOrders;
		private ButtonEx.VistaButton cmdPlotList;
		private ButtonEx.VistaButton cmdClose;
		private ButtonEx.VistaButton cmdMinimize;
		private ButtonEx.VistaButton cmdWatchList;
		private LabelEx.LabelVista lblCaptionEnd;
		private ButtonEx.VistaButton cmdWindows;
		private ButtonEx.VistaButton cmdFiles;
		private System.Windows.Forms.ImageList imageListClose;
		private System.Windows.Forms.ImageList imageListMinimize;
		private System.Windows.Forms.NotifyIcon notifyIconROC;
		private ButtonEx.VistaButton cmdCustomTickets;
		private ButtonEx.VistaButton cmdStandardTickets;
	}
}

