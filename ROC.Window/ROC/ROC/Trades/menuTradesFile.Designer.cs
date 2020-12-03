namespace ROC
{
	partial class menuTradesFile
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
			this.cmdSaveAs = new ButtonEx.VistaButton();
			this.cmdPrint = new ButtonEx.VistaButton();
			this.cmdCopy = new ButtonEx.VistaButton();
			this.cmdClone = new ButtonEx.VistaButton();
			this.SuspendLayout();
			// 
			// cmdSaveAs
			// 
			this.cmdSaveAs.AutoSize = true;
			this.cmdSaveAs.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdSaveAs.BackColor = System.Drawing.Color.Black;
			this.cmdSaveAs.Dock = System.Windows.Forms.DockStyle.Fill;
			this.cmdSaveAs.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdSaveAs.Image = global::ROC.Properties.Resources.Save;
			this.cmdSaveAs.Location = new System.Drawing.Point(185, 0);
			this.cmdSaveAs.Name = "cmdSaveAs";
			this.cmdSaveAs.OuterBorderColor = System.Drawing.Color.Black;
			this.cmdSaveAs.Size = new System.Drawing.Size(81, 24);
			this.cmdSaveAs.TabIndex = 4;
			this.cmdSaveAs.Text = "Save As...";
			this.cmdSaveAs.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdSaveAs.Click += new System.EventHandler(this.cmdSaveAs_Click);
			// 
			// cmdPrint
			// 
			this.cmdPrint.AutoSize = true;
			this.cmdPrint.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdPrint.BackColor = System.Drawing.Color.Black;
			this.cmdPrint.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdPrint.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdPrint.Image = global::ROC.Properties.Resources.Print;
			this.cmdPrint.Location = new System.Drawing.Point(131, 0);
			this.cmdPrint.Name = "cmdPrint";
			this.cmdPrint.OuterBorderColor = System.Drawing.Color.Black;
			this.cmdPrint.Size = new System.Drawing.Size(54, 24);
			this.cmdPrint.TabIndex = 6;
			this.cmdPrint.Text = "Print";
			this.cmdPrint.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdPrint.Click += new System.EventHandler(this.cmdPrint_Click);
			// 
			// cmdCopy
			// 
			this.cmdCopy.AutoSize = true;
			this.cmdCopy.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdCopy.BackColor = System.Drawing.Color.Black;
			this.cmdCopy.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdCopy.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdCopy.Image = global::ROC.Properties.Resources.Copy;
			this.cmdCopy.Location = new System.Drawing.Point(60, 0);
			this.cmdCopy.Name = "cmdCopy";
			this.cmdCopy.OuterBorderColor = System.Drawing.Color.Black;
			this.cmdCopy.Size = new System.Drawing.Size(71, 24);
			this.cmdCopy.TabIndex = 7;
			this.cmdCopy.Text = "Copy All";
			this.cmdCopy.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdCopy.Click += new System.EventHandler(this.cmdCopy_Click);
			// 
			// cmdClone
			// 
			this.cmdClone.AutoSize = true;
			this.cmdClone.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.cmdClone.BackColor = System.Drawing.Color.Black;
			this.cmdClone.Dock = System.Windows.Forms.DockStyle.Left;
			this.cmdClone.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cmdClone.Image = global::ROC.Properties.Resources.Clone;
			this.cmdClone.Location = new System.Drawing.Point(0, 0);
			this.cmdClone.Name = "cmdClone";
			this.cmdClone.OuterBorderColor = System.Drawing.Color.Black;
			this.cmdClone.Size = new System.Drawing.Size(60, 24);
			this.cmdClone.TabIndex = 5;
			this.cmdClone.Text = "Clone";
			this.cmdClone.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cmdClone.Click += new System.EventHandler(this.cmdClone_Click);
			// 
			// menuTradesFile
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.Controls.Add(this.cmdSaveAs);
			this.Controls.Add(this.cmdPrint);
			this.Controls.Add(this.cmdCopy);
			this.Controls.Add(this.cmdClone);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "menuTradesFile";
			this.Size = new System.Drawing.Size(266, 24);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private ButtonEx.VistaButton cmdSaveAs;
		private ButtonEx.VistaButton cmdPrint;
		private ButtonEx.VistaButton cmdCopy;
		private ButtonEx.VistaButton cmdClone;

	}
}
