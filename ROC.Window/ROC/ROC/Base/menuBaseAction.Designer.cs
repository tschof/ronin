namespace ROC
{
	partial class menuBaseAction
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
			this.vistaPanelStatus = new PanelEx.VistaPanel();
			this.dspFilterMenu = new LabelEx.LabelRotatable();
			this.vistaPanelStatus.SuspendLayout();
			this.SuspendLayout();
			// 
			// vistaPanelStatus
			// 
			this.vistaPanelStatus.BackColor = System.Drawing.Color.Black;
			this.vistaPanelStatus.Controls.Add(this.dspFilterMenu);
			this.vistaPanelStatus.Dock = System.Windows.Forms.DockStyle.Left;
			this.vistaPanelStatus.Location = new System.Drawing.Point(0, 2);
			this.vistaPanelStatus.Name = "vistaPanelStatus";
			this.vistaPanelStatus.Padding = new System.Windows.Forms.Padding(5);
			this.vistaPanelStatus.Size = new System.Drawing.Size(22, 84);
			this.vistaPanelStatus.TabIndex = 2;
			// 
			// dspFilterMenu
			// 
			this.dspFilterMenu.AutoColor = false;
			this.dspFilterMenu.AutoEllipsis = true;
			this.dspFilterMenu.DefaultColor = System.Drawing.Color.White;
			this.dspFilterMenu.DefaultText = "Commands...";
			this.dspFilterMenu.DisplayFactor = 1;
			this.dspFilterMenu.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.dspFilterMenu.DownColor = System.Drawing.Color.Red;
			this.dspFilterMenu.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.dspFilterMenu.IsCurrency = false;
			this.dspFilterMenu.IsFraction = false;
			this.dspFilterMenu.Location = new System.Drawing.Point(5, 5);
			this.dspFilterMenu.MaxDecimal = 7;
			this.dspFilterMenu.Name = "dspFilterMenu";
			this.dspFilterMenu.RotationAngle = 270;
			this.dspFilterMenu.Size = new System.Drawing.Size(12, 74);
			this.dspFilterMenu.TabIndex = 3;
			this.dspFilterMenu.Text = "Commands...";
			this.dspFilterMenu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.dspFilterMenu.TextDirection = LabelEx.LabelRotatable.Direction.Clockwise;
			this.dspFilterMenu.TextOrientation = LabelEx.LabelRotatable.Orientation.Rotate;
			this.dspFilterMenu.TickSize = 0.01;
			this.dspFilterMenu.UpColor = System.Drawing.Color.LimeGreen;
			this.dspFilterMenu.Value = "";
			// 
			// menuBaseAction
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.Controls.Add(this.vistaPanelStatus);
			this.ForeColor = System.Drawing.SystemColors.ControlText;
			this.Name = "menuBaseAction";
			this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
			this.Size = new System.Drawing.Size(110, 88);
			this.vistaPanelStatus.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private PanelEx.VistaPanel vistaPanelStatus;
		private LabelEx.LabelRotatable dspFilterMenu;
	}
}
