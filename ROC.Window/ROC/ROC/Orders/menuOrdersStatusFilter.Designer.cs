namespace ROC
{
	partial class menuOrdersStatusFilter
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
			this.lblFilterFilled = new System.Windows.Forms.Label();
			this.lblFilterPartialFilled = new System.Windows.Forms.Label();
			this.lblFilterReplaced = new System.Windows.Forms.Label();
			this.lblFilterOpen = new System.Windows.Forms.Label();
			this.lblFilterRejected = new System.Windows.Forms.Label();
			this.lblFilterCancelled = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblFilterFilled
			// 
			this.lblFilterFilled.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterFilled.Location = new System.Drawing.Point(0, 0);
			this.lblFilterFilled.Name = "lblFilterFilled";
			this.lblFilterFilled.Size = new System.Drawing.Size(48, 24);
			this.lblFilterFilled.TabIndex = 0;
			this.lblFilterFilled.Text = "Filled";
			this.lblFilterFilled.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterFilled.Click += new System.EventHandler(this.lblFilterFilled_Click);
			// 
			// lblFilterPartialFilled
			// 
			this.lblFilterPartialFilled.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterPartialFilled.Location = new System.Drawing.Point(48, 0);
			this.lblFilterPartialFilled.Name = "lblFilterPartialFilled";
			this.lblFilterPartialFilled.Size = new System.Drawing.Size(72, 24);
			this.lblFilterPartialFilled.TabIndex = 1;
			this.lblFilterPartialFilled.Text = "Partial Filled";
			this.lblFilterPartialFilled.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterPartialFilled.Click += new System.EventHandler(this.lblFilterPartialFilled_Click);
			// 
			// lblFilterReplaced
			// 
			this.lblFilterReplaced.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterReplaced.Location = new System.Drawing.Point(120, 0);
			this.lblFilterReplaced.Name = "lblFilterReplaced";
			this.lblFilterReplaced.Size = new System.Drawing.Size(63, 24);
			this.lblFilterReplaced.TabIndex = 2;
			this.lblFilterReplaced.Text = "Replaced";
			this.lblFilterReplaced.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterReplaced.Click += new System.EventHandler(this.lblFilterReplaced_Click);
			// 
			// lblFilterOpen
			// 
			this.lblFilterOpen.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterOpen.Location = new System.Drawing.Point(183, 0);
			this.lblFilterOpen.Name = "lblFilterOpen";
			this.lblFilterOpen.Size = new System.Drawing.Size(42, 24);
			this.lblFilterOpen.TabIndex = 3;
			this.lblFilterOpen.Text = "Open";
			this.lblFilterOpen.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterOpen.Click += new System.EventHandler(this.lblFilterOpen_Click);
			// 
			// lblFilterRejected
			// 
			this.lblFilterRejected.Dock = System.Windows.Forms.DockStyle.Left;
			this.lblFilterRejected.Location = new System.Drawing.Point(225, 0);
			this.lblFilterRejected.Name = "lblFilterRejected";
			this.lblFilterRejected.Size = new System.Drawing.Size(61, 24);
			this.lblFilterRejected.TabIndex = 4;
			this.lblFilterRejected.Text = "Rejected";
			this.lblFilterRejected.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterRejected.Click += new System.EventHandler(this.lblFilterRejected_Click);
			// 
			// lblFilterCancelled
			// 
			this.lblFilterCancelled.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lblFilterCancelled.Location = new System.Drawing.Point(286, 0);
			this.lblFilterCancelled.Name = "lblFilterCancelled";
			this.lblFilterCancelled.Size = new System.Drawing.Size(64, 24);
			this.lblFilterCancelled.TabIndex = 5;
			this.lblFilterCancelled.Text = "Cancelled";
			this.lblFilterCancelled.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.lblFilterCancelled.Click += new System.EventHandler(this.lblFilterCancelled_Click);
			// 
			// menuOrdersStatusFilter
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Black;
			this.Controls.Add(this.lblFilterCancelled);
			this.Controls.Add(this.lblFilterRejected);
			this.Controls.Add(this.lblFilterOpen);
			this.Controls.Add(this.lblFilterReplaced);
			this.Controls.Add(this.lblFilterPartialFilled);
			this.Controls.Add(this.lblFilterFilled);
			this.ForeColor = System.Drawing.Color.White;
			this.Name = "menuOrdersStatusFilter";
			this.Size = new System.Drawing.Size(350, 24);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lblFilterFilled;
		private System.Windows.Forms.Label lblFilterPartialFilled;
		private System.Windows.Forms.Label lblFilterReplaced;
		private System.Windows.Forms.Label lblFilterOpen;
		private System.Windows.Forms.Label lblFilterRejected;
		private System.Windows.Forms.Label lblFilterCancelled;
	}
}
