namespace FormEx
{
	partial class VistaWindowCaptionBar
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(VistaWindowCaptionBar));
			this.imageListClose = new System.Windows.Forms.ImageList(this.components);
			this.imageListMaximize = new System.Windows.Forms.ImageList(this.components);
			this.imageListMinimize = new System.Windows.Forms.ImageList(this.components);
			this.imageListRestore = new System.Windows.Forms.ImageList(this.components);
			this.panelManagerCaptionBar = new PanelEx.PanelManager();
			this.mainPanel = new PanelEx.ManagedPanel();
			this.vistaPanelBackground = new PanelEx.VistaPanel();
			this.panelLabel = new PanelEx.VistaPanel();
			this.txtCaption = new System.Windows.Forms.TextBox();
			this.lblCaption = new LabelEx.LabelVista();
			this.cmdDisplay = new ButtonEx.VistaButton();
			this.imageListIcons = new System.Windows.Forms.ImageList(this.components);
			this.cmdRefresh = new ButtonEx.VistaButton();
			this.cmdFit = new ButtonEx.VistaButton();
			this.cmdHelp = new ButtonEx.VistaButton();
			this.cmdMinimize = new ButtonEx.VistaButton();
			this.cmdMaximize = new ButtonEx.VistaButton();
			this.cmdClose = new ButtonEx.VistaButton();
			this.lblShowAll = new ButtonEx.VistaButton();
			this.lblPin = new ButtonEx.VistaButton();
			this.lblIcon = new ButtonEx.VistaButton();
			this.panelManagerCaptionBar.SuspendLayout();
			this.mainPanel.SuspendLayout();
			this.vistaPanelBackground.SuspendLayout();
			this.panelLabel.SuspendLayout();
			this.SuspendLayout();
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
			// imageListMaximize
			// 
			this.imageListMaximize.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListMaximize.ImageStream")));
			this.imageListMaximize.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListMaximize.Images.SetKeyName(0, "Maximize.png");
			this.imageListMaximize.Images.SetKeyName(1, "MaximizeDisabled.png");
			this.imageListMaximize.Images.SetKeyName(2, "MaximizePressed.png");
			this.imageListMaximize.Images.SetKeyName(3, "MaximizeHot.png");
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
			// imageListRestore
			// 
			this.imageListRestore.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListRestore.ImageStream")));
			this.imageListRestore.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListRestore.Images.SetKeyName(0, "Restore.png");
			this.imageListRestore.Images.SetKeyName(1, "RestoreDisabled.png");
			this.imageListRestore.Images.SetKeyName(2, "RestorePressed.png");
			this.imageListRestore.Images.SetKeyName(3, "RestoreHot.png");
			// 
			// panelManagerCaptionBar
			// 
			this.panelManagerCaptionBar.Controls.Add(this.mainPanel);
			this.panelManagerCaptionBar.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelManagerCaptionBar.Location = new System.Drawing.Point(0, 0);
			this.panelManagerCaptionBar.Name = "panelManagerCaptionBar";
			this.panelManagerCaptionBar.SelectedIndex = 0;
			this.panelManagerCaptionBar.SelectedPanel = this.mainPanel;
			this.panelManagerCaptionBar.Size = new System.Drawing.Size(797, 26);
			this.panelManagerCaptionBar.TabIndex = 0;
			this.panelManagerCaptionBar.TabStop = false;
			// 
			// mainPanel
			// 
			this.mainPanel.Controls.Add(this.vistaPanelBackground);
			this.mainPanel.Location = new System.Drawing.Point(0, 0);
			this.mainPanel.Name = "mainPanel";
			this.mainPanel.Size = new System.Drawing.Size(797, 26);
			this.mainPanel.Text = "panelMain";
			// 
			// vistaPanelBackground
			// 
			this.vistaPanelBackground.BackColor = System.Drawing.Color.Black;
			this.vistaPanelBackground.Controls.Add(this.panelLabel);
			this.vistaPanelBackground.Controls.Add(this.cmdDisplay);
			this.vistaPanelBackground.Controls.Add(this.cmdRefresh);
			this.vistaPanelBackground.Controls.Add(this.cmdFit);
			this.vistaPanelBackground.Controls.Add(this.cmdHelp);
			this.vistaPanelBackground.Controls.Add(this.cmdMinimize);
			this.vistaPanelBackground.Controls.Add(this.cmdMaximize);
			this.vistaPanelBackground.Controls.Add(this.cmdClose);
			this.vistaPanelBackground.Controls.Add(this.lblShowAll);
			this.vistaPanelBackground.Controls.Add(this.lblPin);
			this.vistaPanelBackground.Controls.Add(this.lblIcon);
			this.vistaPanelBackground.Dock = System.Windows.Forms.DockStyle.Fill;
			this.vistaPanelBackground.Location = new System.Drawing.Point(0, 0);
			this.vistaPanelBackground.Name = "vistaPanelBackground";
			this.vistaPanelBackground.Size = new System.Drawing.Size(797, 26);
			this.vistaPanelBackground.TabIndex = 0;
			// 
			// panelLabel
			// 
			this.panelLabel.BackColor = System.Drawing.Color.Black;
			this.panelLabel.Controls.Add(this.txtCaption);
			this.panelLabel.Controls.Add(this.lblCaption);
			this.panelLabel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelLabel.Location = new System.Drawing.Point(259, 0);
			this.panelLabel.Name = "panelLabel";
			this.panelLabel.Padding = new System.Windows.Forms.Padding(3);
			this.panelLabel.Size = new System.Drawing.Size(405, 26);
			this.panelLabel.TabIndex = 0;
			// 
			// txtCaption
			// 
			this.txtCaption.BackColor = System.Drawing.SystemColors.MenuHighlight;
			this.txtCaption.Dock = System.Windows.Forms.DockStyle.Fill;
			this.txtCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtCaption.ForeColor = System.Drawing.Color.White;
			this.txtCaption.Location = new System.Drawing.Point(3, 3);
			this.txtCaption.Margin = new System.Windows.Forms.Padding(10);
			this.txtCaption.Name = "txtCaption";
			this.txtCaption.Size = new System.Drawing.Size(399, 20);
			this.txtCaption.TabIndex = 0;
			this.txtCaption.TabStop = false;
			this.txtCaption.Visible = false;
			this.txtCaption.WordWrap = false;
			this.txtCaption.DoubleClick += new System.EventHandler(this.txtCaption_DoubleClick);
			this.txtCaption.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtCaption_KeyPress);
			// 
			// lblCaption
			// 
			this.lblCaption.Animate = true;
			this.lblCaption.AutoColor = false;
			this.lblCaption.DefaultColor = System.Drawing.Color.White;
			this.lblCaption.DefaultText = "";
			this.lblCaption.DisplayFactor = 1;
			this.lblCaption.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblCaption.DownColor = System.Drawing.Color.Red;
			this.lblCaption.Is64Th = false;
			this.lblCaption.IsCurrency = false;
			this.lblCaption.IsFraction = false;
			this.lblCaption.IsPercent = false;
			this.lblCaption.Location = new System.Drawing.Point(3, 3);
			this.lblCaption.MaxDecimal = 7;
			this.lblCaption.Name = "lblCaption";
			this.lblCaption.Shape = LabelEx.LabelVista.Shapes.Pill;
			this.lblCaption.Size = new System.Drawing.Size(399, 20);
			this.lblCaption.TabIndex = 1002;
			this.lblCaption.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblCaption.TickSize = 0.01;
			this.lblCaption.UpColor = System.Drawing.Color.LimeGreen;
			this.lblCaption.Value = "";
			this.lblCaption.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lblCaption_MouseDown);
			this.lblCaption.MouseHover += new System.EventHandler(this.lblCaption_MouseHover);
			// 
			// cmdDisplay
			// 
			this.cmdDisplay.AutoSize = true;
			this.cmdDisplay.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdDisplay.BackColor = System.Drawing.Color.Black;
			this.cmdDisplay.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdDisplay.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdDisplay.ImageIndex = 6;
			this.cmdDisplay.ImageList = this.imageListIcons;
			this.cmdDisplay.Location = new System.Drawing.Point(192, 0);
			this.cmdDisplay.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdDisplay.Name = "cmdDisplay";
			this.cmdDisplay.Size = new System.Drawing.Size(67, 26);
			this.cmdDisplay.TabIndex = 0;
			this.cmdDisplay.TabStop = false;
			this.cmdDisplay.Text = "Display";
			this.cmdDisplay.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdDisplay.Click += new System.EventHandler(this.cmdDisplay_Click);
			// 
			// imageListIcons
			// 
			this.imageListIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListIcons.ImageStream")));
			this.imageListIcons.TransparentColor = System.Drawing.Color.Transparent;
			this.imageListIcons.Images.SetKeyName(0, "Dart.png");
			this.imageListIcons.Images.SetKeyName(1, "Lock.png");
			this.imageListIcons.Images.SetKeyName(2, "Key.png");
			this.imageListIcons.Images.SetKeyName(3, "Fit.png");
			this.imageListIcons.Images.SetKeyName(4, "Help.png");
			this.imageListIcons.Images.SetKeyName(5, "Refresh.png");
			this.imageListIcons.Images.SetKeyName(6, "Display.png");
			this.imageListIcons.Images.SetKeyName(7, "Orders.png");
			this.imageListIcons.Images.SetKeyName(8, "Trades.png");
			this.imageListIcons.Images.SetKeyName(9, "Pinned.png");
			this.imageListIcons.Images.SetKeyName(10, "UnPinned.png");
			this.imageListIcons.Images.SetKeyName(11, "ToHigh.png");
			// 
			// cmdRefresh
			// 
			this.cmdRefresh.AutoSize = true;
			this.cmdRefresh.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdRefresh.BackColor = System.Drawing.Color.Black;
			this.cmdRefresh.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdRefresh.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdRefresh.ImageIndex = 5;
			this.cmdRefresh.ImageList = this.imageListIcons;
			this.cmdRefresh.Location = new System.Drawing.Point(122, 0);
			this.cmdRefresh.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdRefresh.Name = "cmdRefresh";
			this.cmdRefresh.Size = new System.Drawing.Size(70, 26);
			this.cmdRefresh.TabIndex = 0;
			this.cmdRefresh.TabStop = false;
			this.cmdRefresh.Text = "Refresh";
			this.cmdRefresh.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdRefresh.MouseHover += new System.EventHandler(this.lblCaption_MouseHover);
			this.cmdRefresh.Click += new System.EventHandler(this.cmdRefresh_Click);
			// 
			// cmdFit
			// 
			this.cmdFit.AutoSize = true;
			this.cmdFit.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdFit.BackColor = System.Drawing.Color.Black;
			this.cmdFit.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdFit.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdFit.ImageIndex = 3;
			this.cmdFit.ImageList = this.imageListIcons;
			this.cmdFit.Location = new System.Drawing.Point(78, 0);
			this.cmdFit.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdFit.Name = "cmdFit";
			this.cmdFit.Size = new System.Drawing.Size(44, 26);
			this.cmdFit.TabIndex = 0;
			this.cmdFit.TabStop = false;
			this.cmdFit.Text = "Fit";
			this.cmdFit.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdFit.MouseHover += new System.EventHandler(this.lblCaption_MouseHover);
			this.cmdFit.Click += new System.EventHandler(this.cmdFit_Click);
			// 
			// cmdHelp
			// 
			this.cmdHelp.AutoSize = true;
			this.cmdHelp.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdHelp.BackColor = System.Drawing.Color.Black;
			this.cmdHelp.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdHelp.ImageIndex = 4;
			this.cmdHelp.ImageList = this.imageListIcons;
			this.cmdHelp.Location = new System.Drawing.Point(664, 0);
			this.cmdHelp.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdHelp.Name = "cmdHelp";
			this.cmdHelp.Size = new System.Drawing.Size(55, 26);
			this.cmdHelp.TabIndex = 0;
			this.cmdHelp.TabStop = false;
			this.cmdHelp.Text = "Help";
			this.cmdHelp.TextImageRelation = System.Windows.Forms.TextImageRelation.TextBeforeImage;
			this.cmdHelp.MouseHover += new System.EventHandler(this.lblCaption_MouseHover);
			this.cmdHelp.Click += new System.EventHandler(this.cmdHelp_Click);
			// 
			// cmdMinimize
			// 
			this.cmdMinimize.AutoSize = true;
			this.cmdMinimize.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdMinimize.BackColor = System.Drawing.Color.Black;
			this.cmdMinimize.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdMinimize.ImageKey = "Minimize.png";
			this.cmdMinimize.ImageList = this.imageListMinimize;
			this.cmdMinimize.Location = new System.Drawing.Point(719, 0);
			this.cmdMinimize.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdMinimize.Name = "cmdMinimize";
			this.cmdMinimize.Size = new System.Drawing.Size(26, 26);
			this.cmdMinimize.TabIndex = 0;
			this.cmdMinimize.TabStop = false;
			this.cmdMinimize.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdMinimize.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdMinimize.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Button_MouseMove);
			this.cmdMinimize.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			this.cmdMinimize.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdMinimize.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			// 
			// cmdMaximize
			// 
			this.cmdMaximize.AutoSize = true;
			this.cmdMaximize.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdMaximize.BackColor = System.Drawing.Color.Black;
			this.cmdMaximize.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdMaximize.ImageIndex = 0;
			this.cmdMaximize.ImageList = this.imageListMaximize;
			this.cmdMaximize.Location = new System.Drawing.Point(745, 0);
			this.cmdMaximize.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdMaximize.Name = "cmdMaximize";
			this.cmdMaximize.Size = new System.Drawing.Size(26, 26);
			this.cmdMaximize.TabIndex = 0;
			this.cmdMaximize.TabStop = false;
			this.cmdMaximize.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdMaximize.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdMaximize.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Button_MouseMove);
			this.cmdMaximize.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			this.cmdMaximize.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdMaximize.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			// 
			// cmdClose
			// 
			this.cmdClose.AutoSize = true;
			this.cmdClose.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdClose.BackColor = System.Drawing.Color.Black;
			this.cmdClose.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdClose.ImageIndex = 0;
			this.cmdClose.ImageList = this.imageListClose;
			this.cmdClose.Location = new System.Drawing.Point(771, 0);
			this.cmdClose.MinimumSize = new System.Drawing.Size(26, 26);
			this.cmdClose.Name = "cmdClose";
			this.cmdClose.Size = new System.Drawing.Size(26, 26);
			this.cmdClose.TabIndex = 0;
			this.cmdClose.TabStop = false;
			this.cmdClose.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdClose.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdClose.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Button_MouseMove);
			this.cmdClose.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			this.cmdClose.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdClose.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			// 
			// lblShowAll
			// 
			this.lblShowAll.AutoSize = true;
			this.lblShowAll.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.lblShowAll.BackColor = System.Drawing.Color.White;
			this.lblShowAll.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblShowAll.ForeColor = System.Drawing.Color.Black;
			this.lblShowAll.ImageIndex = 11;
			this.lblShowAll.ImageList = this.imageListIcons;
			this.lblShowAll.Location = new System.Drawing.Point(52, 0);
			this.lblShowAll.MinimumSize = new System.Drawing.Size(26, 26);
			this.lblShowAll.Name = "lblShowAll";
			this.lblShowAll.Size = new System.Drawing.Size(26, 26);
			this.lblShowAll.TabIndex = 0;
			this.lblShowAll.TabStop = false;
			this.lblShowAll.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.lblShowAll.Click += new System.EventHandler(this.lblShowAll_Click);
			// 
			// lblPin
			// 
			this.lblPin.AutoSize = true;
			this.lblPin.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.lblPin.BackColor = System.Drawing.Color.White;
			this.lblPin.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblPin.ForeColor = System.Drawing.Color.Black;
			this.lblPin.ImageIndex = 10;
			this.lblPin.ImageList = this.imageListIcons;
			this.lblPin.Location = new System.Drawing.Point(26, 0);
			this.lblPin.MinimumSize = new System.Drawing.Size(26, 26);
			this.lblPin.Name = "lblPin";
			this.lblPin.Size = new System.Drawing.Size(26, 26);
			this.lblPin.TabIndex = 0;
			this.lblPin.TabStop = false;
			this.lblPin.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.lblPin.Click += new System.EventHandler(this.lblPin_Click);
			// 
			// lblIcon
			// 
			this.lblIcon.AutoSize = true;
			this.lblIcon.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.lblIcon.BackColor = System.Drawing.Color.White;
			this.lblIcon.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblIcon.ForeColor = System.Drawing.Color.Black;
			this.lblIcon.ImageIndex = 0;
			this.lblIcon.ImageList = this.imageListIcons;
			this.lblIcon.Location = new System.Drawing.Point(0, 0);
			this.lblIcon.MinimumSize = new System.Drawing.Size(26, 26);
			this.lblIcon.Name = "lblIcon";
			this.lblIcon.Size = new System.Drawing.Size(26, 26);
			this.lblIcon.TabIndex = 0;
			this.lblIcon.TabStop = false;
			this.lblIcon.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.lblIcon.MouseHover += new System.EventHandler(this.lblCaption_MouseHover);
			this.lblIcon.Click += new System.EventHandler(this.lblIcon_Click);
			// 
			// VistaWindowCaptionBar
			// 
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
			this.BackColor = System.Drawing.Color.Transparent;
			this.Controls.Add(this.panelManagerCaptionBar);
			this.DoubleBuffered = true;
			this.Name = "VistaWindowCaptionBar";
			this.Size = new System.Drawing.Size(797, 26);
			this.Load += new System.EventHandler(this.VistaWindowCaptionBar_Load);
			this.MouseHover += new System.EventHandler(this.lblCaption_MouseHover);
			this.MouseEnter += new System.EventHandler(this.lblCaption_MouseHover);
			this.panelManagerCaptionBar.ResumeLayout(false);
			this.mainPanel.ResumeLayout(false);
			this.vistaPanelBackground.ResumeLayout(false);
			this.vistaPanelBackground.PerformLayout();
			this.panelLabel.ResumeLayout(false);
			this.panelLabel.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.PanelManager panelManagerCaptionBar;
		private PanelEx.ManagedPanel mainPanel;
		private PanelEx.VistaPanel vistaPanelBackground;
		private ButtonEx.VistaButton cmdMinimize;
		private ButtonEx.VistaButton cmdMaximize;
		private ButtonEx.VistaButton cmdClose;
		private ButtonEx.VistaButton cmdHelp;
		private System.Windows.Forms.ImageList imageListClose;
		private System.Windows.Forms.ImageList imageListMaximize;
		private System.Windows.Forms.ImageList imageListMinimize;
		private System.Windows.Forms.ImageList imageListRestore;
		private ButtonEx.VistaButton cmdFit;
		private ButtonEx.VistaButton cmdRefresh;
		private ButtonEx.VistaButton cmdDisplay;
		private System.Windows.Forms.ImageList imageListIcons;
		private PanelEx.VistaPanel panelLabel;
		private System.Windows.Forms.TextBox txtCaption;
		public LabelEx.LabelVista lblCaption;
		private ButtonEx.VistaButton lblIcon;
		private ButtonEx.VistaButton lblPin;
		private ButtonEx.VistaButton lblShowAll;

	}
}
