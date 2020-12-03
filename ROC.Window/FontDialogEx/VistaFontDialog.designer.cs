namespace FontDialogEx
{
	partial class VistaFontDialog
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
			this.checkBoxUnderline = new System.Windows.Forms.CheckBox();
			this.listBoxFontStyles = new System.Windows.Forms.ListBox();
			this.listBoxFontSizes = new System.Windows.Forms.ListBox();
			this.listBoxFontNames = new System.Windows.Forms.ListBox();
			this.lblSelectFontStyle = new LabelEx.LabelVista();
			this.lblFontPreview = new LabelEx.LabelVista();
			this.lblSelectedFontSize = new LabelEx.LabelVista();
			this.lblSelectedFontName = new LabelEx.LabelVista();
			this.cmdCancel = new ButtonEx.VistaButton();
			this.cmdApply = new ButtonEx.VistaButton();
			this.SuspendLayout();
			// 
			// checkBoxUnderline
			// 
			this.checkBoxUnderline.AutoSize = true;
			this.checkBoxUnderline.Location = new System.Drawing.Point(218, 90);
			this.checkBoxUnderline.Name = "checkBoxUnderline";
			this.checkBoxUnderline.Size = new System.Drawing.Size(71, 17);
			this.checkBoxUnderline.TabIndex = 19;
			this.checkBoxUnderline.Text = "Underline";
			this.checkBoxUnderline.UseVisualStyleBackColor = true;
			this.checkBoxUnderline.CheckedChanged += new System.EventHandler(this.SelectedFontChanged);
			// 
			// listBoxFontStyles
			// 
			this.listBoxFontStyles.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.listBoxFontStyles.FormattingEnabled = true;
			this.listBoxFontStyles.Items.AddRange(new object[] {
            "Regular",
            "Italic",
            "Bold",
            "Bold Italic"});
			this.listBoxFontStyles.Location = new System.Drawing.Point(215, 30);
			this.listBoxFontStyles.Name = "listBoxFontStyles";
			this.listBoxFontStyles.Size = new System.Drawing.Size(96, 54);
			this.listBoxFontStyles.TabIndex = 17;
			this.listBoxFontStyles.SelectedIndexChanged += new System.EventHandler(this.SelectedFontChanged);
			// 
			// listBoxFontSizes
			// 
			this.listBoxFontSizes.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.listBoxFontSizes.FormattingEnabled = true;
			this.listBoxFontSizes.Items.AddRange(new object[] {
            "6.75",
            "8.25",
            "9.00",
            "9.75",
            "11.25",
            "12.00",
            "14.25"});
			this.listBoxFontSizes.Location = new System.Drawing.Point(146, 30);
			this.listBoxFontSizes.Name = "listBoxFontSizes";
			this.listBoxFontSizes.Size = new System.Drawing.Size(63, 93);
			this.listBoxFontSizes.TabIndex = 16;
			this.listBoxFontSizes.SelectedIndexChanged += new System.EventHandler(this.SelectedFontChanged);
			// 
			// listBoxFontNames
			// 
			this.listBoxFontNames.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.listBoxFontNames.FormattingEnabled = true;
			this.listBoxFontNames.Location = new System.Drawing.Point(3, 30);
			this.listBoxFontNames.Name = "listBoxFontNames";
			this.listBoxFontNames.Size = new System.Drawing.Size(137, 158);
			this.listBoxFontNames.TabIndex = 13;
			this.listBoxFontNames.SelectedIndexChanged += new System.EventHandler(this.SelectedFontChanged);
			// 
			// lblSelectFontStyle
			// 
			this.lblSelectFontStyle.ApplyShine = true;
			this.lblSelectFontStyle.DefaultText = "";
			this.lblSelectFontStyle.DisplayFactor = 1;
			this.lblSelectFontStyle.Location = new System.Drawing.Point(215, 4);
			this.lblSelectFontStyle.Name = "lblSelectFontStyle";
			this.lblSelectFontStyle.Size = new System.Drawing.Size(96, 23);
			this.lblSelectFontStyle.TabIndex = 18;
			this.lblSelectFontStyle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblSelectFontStyle.TickSize = 0.01;
			this.lblSelectFontStyle.Value = "";
			// 
			// lblFontPreview
			// 
			this.lblFontPreview.DefaultText = "";
			this.lblFontPreview.DisplayFactor = 1;
			this.lblFontPreview.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblFontPreview.Location = new System.Drawing.Point(146, 126);
			this.lblFontPreview.Name = "lblFontPreview";
			this.lblFontPreview.Size = new System.Drawing.Size(165, 62);
			this.lblFontPreview.TabIndex = 15;
			this.lblFontPreview.Text = "Aa Bb Cc Dd Ee";
			this.lblFontPreview.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFontPreview.TickSize = 0.01;
			this.lblFontPreview.Value = "";
			// 
			// lblSelectedFontSize
			// 
			this.lblSelectedFontSize.ApplyShine = true;
			this.lblSelectedFontSize.DefaultText = "";
			this.lblSelectedFontSize.DisplayFactor = 1;
			this.lblSelectedFontSize.Location = new System.Drawing.Point(146, 4);
			this.lblSelectedFontSize.Name = "lblSelectedFontSize";
			this.lblSelectedFontSize.Size = new System.Drawing.Size(63, 23);
			this.lblSelectedFontSize.TabIndex = 14;
			this.lblSelectedFontSize.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblSelectedFontSize.TickSize = 0.01;
			this.lblSelectedFontSize.Value = "";
			// 
			// lblSelectedFontName
			// 
			this.lblSelectedFontName.ApplyShine = true;
			this.lblSelectedFontName.DefaultText = "";
			this.lblSelectedFontName.DisplayFactor = 1;
			this.lblSelectedFontName.Location = new System.Drawing.Point(3, 4);
			this.lblSelectedFontName.Name = "lblSelectedFontName";
			this.lblSelectedFontName.Size = new System.Drawing.Size(137, 23);
			this.lblSelectedFontName.TabIndex = 12;
			this.lblSelectedFontName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblSelectedFontName.TickSize = 0.01;
			this.lblSelectedFontName.Value = "";
			// 
			// cmdCancel
			// 
			this.cmdCancel.BackColor = System.Drawing.Color.Black;
			this.cmdCancel.Location = new System.Drawing.Point(3, 194);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(75, 23);
			this.cmdCancel.TabIndex = 20;
			this.cmdCancel.Text = "Cancel";
			this.cmdCancel.Click += new System.EventHandler(this.cmdCancel_Click);
			// 
			// cmdApply
			// 
			this.cmdApply.BackColor = System.Drawing.Color.Black;
			this.cmdApply.Location = new System.Drawing.Point(236, 194);
			this.cmdApply.Name = "cmdApply";
			this.cmdApply.Size = new System.Drawing.Size(75, 23);
			this.cmdApply.TabIndex = 21;
			this.cmdApply.Text = "Apply";
			this.cmdApply.Click += new System.EventHandler(this.cmdApply_Click);
			// 
			// VistaFontDialog
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
			this.ClientSize = new System.Drawing.Size(313, 220);
			this.ControlBox = false;
			this.Controls.Add(this.cmdApply);
			this.Controls.Add(this.cmdCancel);
			this.Controls.Add(this.checkBoxUnderline);
			this.Controls.Add(this.lblSelectFontStyle);
			this.Controls.Add(this.listBoxFontStyles);
			this.Controls.Add(this.listBoxFontSizes);
			this.Controls.Add(this.lblFontPreview);
			this.Controls.Add(this.lblSelectedFontSize);
			this.Controls.Add(this.listBoxFontNames);
			this.Controls.Add(this.lblSelectedFontName);
			this.DoubleBuffered = true;
			this.ForeColor = System.Drawing.Color.White;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Name = "VistaFontDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.CheckBox checkBoxUnderline;
		private LabelEx.LabelVista lblSelectFontStyle;
		private System.Windows.Forms.ListBox listBoxFontStyles;
		private System.Windows.Forms.ListBox listBoxFontSizes;
		private LabelEx.LabelVista lblFontPreview;
		private LabelEx.LabelVista lblSelectedFontSize;
		private System.Windows.Forms.ListBox listBoxFontNames;
		private LabelEx.LabelVista lblSelectedFontName;
		private ButtonEx.VistaButton cmdCancel;
		private ButtonEx.VistaButton cmdApply;

	}
}
