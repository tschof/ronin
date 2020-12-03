namespace ROC
{
	partial class frmUserProfile
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
			this.CaptionBar = new FormEx.VistaWindowCaptionBar();
			this.rocUserProfile = new DataGridViewEx.ROCUserProfileList();
			((System.ComponentModel.ISupportInitialize)(this.rocUserProfile)).BeginInit();
			this.SuspendLayout();
			// 
			// CaptionBar
			// 
			this.CaptionBar.BackColor = System.Drawing.Color.Transparent;
			this.CaptionBar.Dock = System.Windows.Forms.DockStyle.Top;
			this.CaptionBar.Location = new System.Drawing.Point(0, 0);
			this.CaptionBar.Name = "CaptionBar";
			this.CaptionBar.ShowFit = true;
			this.CaptionBar.ShowRefresh = true;
			this.CaptionBar.Size = new System.Drawing.Size(292, 26);
			this.CaptionBar.TabIndex = 0;
			// 
			// rocUserProfile
			// 
			this.rocUserProfile.AllowUserToAddRows = false;
			this.rocUserProfile.AllowUserToDeleteRows = false;
			this.rocUserProfile.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rocUserProfile.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
			this.rocUserProfile.ImageList = null;
			this.rocUserProfile.Location = new System.Drawing.Point(0, 26);
			this.rocUserProfile.Name = "rocUserProfile";
			this.rocUserProfile.Size = new System.Drawing.Size(292, 166);
			this.rocUserProfile.TabIndex = 2;
			// 
			// frmUserProfile
			// 
			this.AnimationType = FormEx.VistaWindowBase.AnimationTypes.Slide;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Caption = "User Profile";
			this.ClientSize = new System.Drawing.Size(292, 192);
			this.Controls.Add(this.rocUserProfile);
			this.Controls.Add(this.CaptionBar);
			this.MaximumSize = new System.Drawing.Size(600, 1000);
			this.MinimumSize = new System.Drawing.Size(300, 200);
			this.Name = "frmUserProfile";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			((System.ComponentModel.ISupportInitialize)(this.rocUserProfile)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private FormEx.VistaWindowCaptionBar CaptionBar;
		private DataGridViewEx.ROCUserProfileList rocUserProfile;
	}
}