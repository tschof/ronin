namespace ROC
{
	partial class menuBaseVertical
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
			this.panelROCMenu = new PanelEx.VistaPanel();
			this.dspMainMenu = new LabelEx.LabelRotatable();
			this.labelRotatable1 = new LabelEx.LabelRotatable();
			this.labelRotatable2 = new LabelEx.LabelRotatable();
			this.panelROCMenu.SuspendLayout();
			this.SuspendLayout();
			// 
			// panelROCMenu
			// 
			this.panelROCMenu.BackColor = System.Drawing.Color.Black;
			this.panelROCMenu.Controls.Add(this.labelRotatable2);
			this.panelROCMenu.Controls.Add(this.labelRotatable1);
			this.panelROCMenu.Controls.Add(this.dspMainMenu);
			this.panelROCMenu.Dock = System.Windows.Forms.DockStyle.Left;
			this.panelROCMenu.Location = new System.Drawing.Point(0, 0);
			this.panelROCMenu.Name = "panelROCMenu";
			this.panelROCMenu.Padding = new System.Windows.Forms.Padding(5);
			this.panelROCMenu.Size = new System.Drawing.Size(22, 264);
			this.panelROCMenu.TabIndex = 2;
			// 
			// dspMainMenu
			// 
			this.dspMainMenu.DefaultText = "Menu";
			this.dspMainMenu.DisplayFactor = 1;
			this.dspMainMenu.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.dspMainMenu.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspMainMenu.Location = new System.Drawing.Point(5, 194);
			this.dspMainMenu.Name = "dspMainMenu";
			this.dspMainMenu.RotationAngle = 270;
			this.dspMainMenu.Size = new System.Drawing.Size(12, 65);
			this.dspMainMenu.TabIndex = 0;
			this.dspMainMenu.Text = "Menu";
			this.dspMainMenu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspMainMenu.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.dspMainMenu.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.dspMainMenu.TickSize = 0.01;
			this.dspMainMenu.Value = "";
			// 
			// labelRotatable1
			// 
			this.labelRotatable1.DefaultText = "";
			this.labelRotatable1.DisplayFactor = 1;
			this.labelRotatable1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.labelRotatable1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelRotatable1.Location = new System.Drawing.Point(5, 129);
			this.labelRotatable1.Name = "labelRotatable1";
			this.labelRotatable1.RotationAngle = 270;
			this.labelRotatable1.Size = new System.Drawing.Size(12, 65);
			this.labelRotatable1.TabIndex = 1;
			this.labelRotatable1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.labelRotatable1.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.labelRotatable1.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.labelRotatable1.TickSize = 0.01;
			this.labelRotatable1.Value = "";
			// 
			// labelRotatable2
			// 
			this.labelRotatable2.DefaultText = "";
			this.labelRotatable2.DisplayFactor = 1;
			this.labelRotatable2.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.labelRotatable2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelRotatable2.Location = new System.Drawing.Point(5, 64);
			this.labelRotatable2.Name = "labelRotatable2";
			this.labelRotatable2.RotationAngle = 270;
			this.labelRotatable2.Size = new System.Drawing.Size(12, 65);
			this.labelRotatable2.TabIndex = 2;
			this.labelRotatable2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.labelRotatable2.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.labelRotatable2.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.labelRotatable2.TickSize = 0.01;
			this.labelRotatable2.Value = "";
			// 
			// menuBaseVertical
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.panelROCMenu);
			this.Name = "menuBaseVertical";
			this.Size = new System.Drawing.Size(150, 264);
			this.panelROCMenu.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.VistaPanel panelROCMenu;
		private LabelEx.LabelRotatable dspMainMenu;
		private LabelEx.LabelRotatable labelRotatable1;
		private LabelEx.LabelRotatable labelRotatable2;

	}
}
