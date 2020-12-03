namespace FormEx
{
	partial class CaptionBarVistaWindow
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CaptionBarVistaWindow));
			this.imageListClose = new System.Windows.Forms.ImageList(this.components);
			this.imageListMaximize = new System.Windows.Forms.ImageList(this.components);
			this.imageListMinimize = new System.Windows.Forms.ImageList(this.components);
			this.imageListRestore = new System.Windows.Forms.ImageList(this.components);
			this.panelManagerCaptionBar = new PanelEx.PanelManager();
			this.mainPanel = new PanelEx.ManagedPanel();
			this.vistaPanelBackground = new PanelEx.VistaPanel();
			this.cmdFit = new ButtonEx.VistaButton();
			this.lblCaption = new LabelEx.VistaLabel();
			this.txtCaption = new System.Windows.Forms.TextBox();
			this.cmdHelp = new ButtonEx.VistaButton();
			this.cmdMinimize = new ButtonEx.VistaButton();
			this.cmdMaximize = new ButtonEx.VistaButton();
			this.cmdClose = new ButtonEx.VistaButton();
			this.cmdIcon = new ButtonEx.VistaButton();
			this.panelManagerCaptionBar.SuspendLayout();
			this.mainPanel.SuspendLayout();
			this.vistaPanelBackground.SuspendLayout();
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
			this.panelManagerCaptionBar.Size = new System.Drawing.Size(636, 26);
			this.panelManagerCaptionBar.TabIndex = 0;
			this.panelManagerCaptionBar.TabStop = false;
			// 
			// mainPanel
			// 
			this.mainPanel.Controls.Add(this.vistaPanelBackground);
			this.mainPanel.Location = new System.Drawing.Point(0, 0);
			this.mainPanel.Name = "mainPanel";
			this.mainPanel.Size = new System.Drawing.Size(636, 26);
			this.mainPanel.Text = "mainPanel";
			// 
			// vistaPanelBackground
			// 
			this.vistaPanelBackground.BackColor = System.Drawing.Color.Black;
			this.vistaPanelBackground.Controls.Add(this.lblCaption);
			this.vistaPanelBackground.Controls.Add(this.cmdFit);
			this.vistaPanelBackground.Controls.Add(this.txtCaption);
			this.vistaPanelBackground.Controls.Add(this.cmdHelp);
			this.vistaPanelBackground.Controls.Add(this.cmdMinimize);
			this.vistaPanelBackground.Controls.Add(this.cmdMaximize);
			this.vistaPanelBackground.Controls.Add(this.cmdClose);
			this.vistaPanelBackground.Controls.Add(this.cmdIcon);
			this.vistaPanelBackground.Dock = System.Windows.Forms.DockStyle.Fill;
			this.vistaPanelBackground.Location = new System.Drawing.Point(0, 0);
			this.vistaPanelBackground.Name = "vistaPanelBackground";
			this.vistaPanelBackground.Size = new System.Drawing.Size(636, 26);
			this.vistaPanelBackground.TabIndex = 0;
			// 
			// cmdFit
			// 
			this.cmdFit.BackColor = System.Drawing.Color.Silver;
			this.cmdFit.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdFit.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdFit.Location = new System.Drawing.Point(26, 0);
			this.cmdFit.Name = "cmdFit";
			this.cmdFit.Size = new System.Drawing.Size(26, 26);
			this.cmdFit.TabIndex = 1001;
			this.cmdFit.TabStop = false;
			this.cmdFit.Text = "Fit";
			this.cmdFit.Click += new System.EventHandler(this.cmdFit_Click);
			// 
			// lblCaption
			// 
			this.lblCaption.Animate = true;
			this.lblCaption.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblCaption.Location = new System.Drawing.Point(52, 0);
			this.lblCaption.Name = "lblCaption";
			this.lblCaption.Size = new System.Drawing.Size(480, 26);
			this.lblCaption.TabIndex = 1000;
			this.lblCaption.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblCaption.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lblCaption_MouseDown);
			// 
			// txtCaption
			// 
			this.txtCaption.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.txtCaption.BackColor = System.Drawing.SystemColors.ControlLightLight;
			this.txtCaption.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.txtCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.txtCaption.ForeColor = System.Drawing.SystemColors.ControlText;
			this.txtCaption.Location = new System.Drawing.Point(26, 6);
			this.txtCaption.Margin = new System.Windows.Forms.Padding(10);
			this.txtCaption.Name = "txtCaption";
			this.txtCaption.Size = new System.Drawing.Size(506, 13);
			this.txtCaption.TabIndex = 0;
			this.txtCaption.Visible = false;
			this.txtCaption.WordWrap = false;
			this.txtCaption.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtCaption_KeyPress);
			// 
			// cmdHelp
			// 
			this.cmdHelp.BackColor = System.Drawing.Color.Silver;
			this.cmdHelp.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdHelp.Location = new System.Drawing.Point(532, 0);
			this.cmdHelp.Name = "cmdHelp";
			this.cmdHelp.Size = new System.Drawing.Size(26, 26);
			this.cmdHelp.TabIndex = 6;
			this.cmdHelp.TabStop = false;
			this.cmdHelp.Text = "?";
			this.cmdHelp.Click += new System.EventHandler(this.cmdHelp_Click);
			// 
			// cmdMinimize
			// 
			this.cmdMinimize.BackColor = System.Drawing.Color.Silver;
			this.cmdMinimize.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdMinimize.ImageKey = "Minimize.png";
			this.cmdMinimize.ImageList = this.imageListMinimize;
			this.cmdMinimize.Location = new System.Drawing.Point(558, 0);
			this.cmdMinimize.Name = "cmdMinimize";
			this.cmdMinimize.Size = new System.Drawing.Size(26, 26);
			this.cmdMinimize.TabIndex = 3;
			this.cmdMinimize.TabStop = false;
			this.cmdMinimize.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdMinimize.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdMinimize.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Button_MouseMove);
			this.cmdMinimize.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			this.cmdMinimize.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdMinimize.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			// 
			// cmdMaximize
			// 
			this.cmdMaximize.BackColor = System.Drawing.Color.Silver;
			this.cmdMaximize.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdMaximize.ImageIndex = 0;
			this.cmdMaximize.ImageList = this.imageListMaximize;
			this.cmdMaximize.Location = new System.Drawing.Point(584, 0);
			this.cmdMaximize.Name = "cmdMaximize";
			this.cmdMaximize.Size = new System.Drawing.Size(26, 26);
			this.cmdMaximize.TabIndex = 2;
			this.cmdMaximize.TabStop = false;
			this.cmdMaximize.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdMaximize.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdMaximize.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Button_MouseMove);
			this.cmdMaximize.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			this.cmdMaximize.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdMaximize.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			// 
			// cmdClose
			// 
			this.cmdClose.BackColor = System.Drawing.Color.Silver;
			this.cmdClose.Dock = System.Windows.Forms.DockStyle.Right;
			this.cmdClose.ImageIndex = 0;
			this.cmdClose.ImageList = this.imageListClose;
			this.cmdClose.Location = new System.Drawing.Point(610, 0);
			this.cmdClose.Name = "cmdClose";
			this.cmdClose.Size = new System.Drawing.Size(26, 26);
			this.cmdClose.TabIndex = 1;
			this.cmdClose.TabStop = false;
			this.cmdClose.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Button_MouseUp);
			this.cmdClose.MouseEnter += new System.EventHandler(this.Button_MouseEnter);
			this.cmdClose.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Button_MouseMove);
			this.cmdClose.MouseLeave += new System.EventHandler(this.Button_MouseLeave);
			this.cmdClose.MouseHover += new System.EventHandler(this.Button_MouseHover);
			this.cmdClose.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Button_MouseDown);
			// 
			// cmdIcon
			// 
			this.cmdIcon.BackColor = System.Drawing.Color.Silver;
			this.cmdIcon.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdIcon.Location = new System.Drawing.Point(0, 0);
			this.cmdIcon.Name = "cmdIcon";
			this.cmdIcon.Size = new System.Drawing.Size(26, 26);
			this.cmdIcon.TabIndex = 0;
			this.cmdIcon.TabStop = false;
			// 
			// CaptionBarVistaWindow
			// 
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
			this.BackColor = System.Drawing.Color.Transparent;
			this.Controls.Add(this.panelManagerCaptionBar);
			this.DoubleBuffered = true;
			this.Name = "CaptionBarVistaWindow";
			this.Size = new System.Drawing.Size(636, 26);
			this.Load += new System.EventHandler(this.VistaWindowCaptionBar_Load);
			this.MouseEnter += new System.EventHandler(this.VistaWindowCaptionBar_MouseEnter);
			this.panelManagerCaptionBar.ResumeLayout(false);
			this.mainPanel.ResumeLayout(false);
			this.vistaPanelBackground.ResumeLayout(false);
			this.vistaPanelBackground.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.PanelManager panelManagerCaptionBar;
		private PanelEx.ManagedPanel mainPanel;
		private PanelEx.VistaPanel vistaPanelBackground;
		private ButtonEx.VistaButton cmdMinimize;
		private ButtonEx.VistaButton cmdMaximize;
		private ButtonEx.VistaButton cmdClose;
		private ButtonEx.VistaButton cmdIcon;
		private System.Windows.Forms.TextBox txtCaption;
		private ButtonEx.VistaButton cmdHelp;
		private System.Windows.Forms.ImageList imageListClose;
		private System.Windows.Forms.ImageList imageListMaximize;
		private System.Windows.Forms.ImageList imageListMinimize;
		private System.Windows.Forms.ImageList imageListRestore;
		public LabelEx.VistaLabel lblCaption;
		private ButtonEx.VistaButton cmdFit;

	}
}
