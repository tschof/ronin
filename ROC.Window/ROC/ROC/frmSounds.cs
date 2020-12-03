using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FormEx;
using System.Media;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmSounds : VistaWindowBorder
	{
		#region - Constructor -

		public frmSounds()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			DGVSounds.Rows.Clear();

			DGVSounds.Rows.Add("Open", Configuration.SOUND.Default.Open.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableOpen);
			DGVSounds.Rows.Add("Replaced", Configuration.SOUND.Default.Replaced.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableReplaced);
			DGVSounds.Rows.Add("Filled", Configuration.SOUND.Default.Filled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableFilled);
			DGVSounds.Rows.Add("FilledAndCanceled", Configuration.SOUND.Default.FilledAndCanceled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableCanceled);
			DGVSounds.Rows.Add("PartiallyFilled", Configuration.SOUND.Default.PartiallyFilled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnablePartiallyFilled);
			DGVSounds.Rows.Add("Rejected", Configuration.SOUND.Default.Rejected.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableRejected);
			DGVSounds.Rows.Add("Canceled", Configuration.SOUND.Default.Canceled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableCanceled);

			FormClosing += new FormClosingEventHandler(frmSounds_FormClosing);

			DGVSounds.SelectionChanged += new EventHandler(DGVSounds_SelectionChanged);
			DGVSounds.CellMouseDown += new DataGridViewCellMouseEventHandler(DGVSounds_CellMouseDown);
			
			DGVSounds.MouseEnter += new EventHandler(DGVSounds_MouseEnter);
		}

		#endregion

		#region - Form Event -

		private void frmSounds_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}

		#endregion

		#region - Grid Event -

		private void DGVSounds_SelectionChanged(object sender, EventArgs e)
		{
			// Make sure nothing is selected
			foreach (DataGridViewCell cell in DGVSounds.SelectedCells)
			{
				cell.Selected = false;
			}
		}

		private void DGVSounds_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (DGVSounds.Columns[e.ColumnIndex].Name)
				{
					case "Browse":
						OpenFileDialog ofd = new OpenFileDialog();
						ofd.DefaultExt = "Sound";
						ofd.Filter = "Wav File (*.wav)|*.wav";
						ofd.AddExtension = true;
						ofd.RestoreDirectory = true;
						ofd.Title = "Load New Sound From";

						if (DGVSounds.Rows[e.RowIndex].Cells["Path"].Value != null && DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString() != "")
						{
							ofd.InitialDirectory = DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString();
						}
						else
						{
							ofd.InitialDirectory = "Sounds";
						}

						if (ofd.ShowDialog() == DialogResult.OK)
						{
							DGVSounds.Rows[e.RowIndex].Cells["Path"].Value = ofd.FileName;
						}
						break;
					case "Test":
						if (DGVSounds.Rows[e.RowIndex].Cells["Path"].Value != null && DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString() != "")
						{
							SoundPlayer sp = new SoundPlayer(DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString().Replace(@"..\", ""));
							sp.Play();
						}
						break;
				}
			}
		}

		private void DGVSounds_MouseEnter(object sender, EventArgs e)
		{
			ActivateGrid(DGVSounds);
		}

		#endregion

		private void cmdApply_Click(object sender, EventArgs e)
		{
			try
			{
				for (int rowCount = 0; rowCount < DGVSounds.Rows.Count; rowCount++)
				{
					switch (DGVSounds.Rows[rowCount].Cells["Type"].Value.ToString())
					{
						case "Open":
							Configuration.SOUND.Default.Open = GetSoundPath(rowCount);
							Configuration.SOUND.Default.EnableOpen = GetSoundEnableFlag(rowCount);
							break;
						case "Replaced":
							Configuration.SOUND.Default.Replaced = GetSoundPath(rowCount);
							Configuration.SOUND.Default.EnableReplaced = GetSoundEnableFlag(rowCount);
							break;
						case "Filled":
							Configuration.SOUND.Default.Filled = GetSoundPath(rowCount);
							Configuration.SOUND.Default.EnableFilled = GetSoundEnableFlag(rowCount);
							break;
						case "FilledAndCanceled":
							Configuration.SOUND.Default.FilledAndCanceled = GetSoundPath(rowCount);
							Configuration.SOUND.Default.EnableFilledAndCanceled = GetSoundEnableFlag(rowCount);
							break;
						case "PartiallyFilled":
							Configuration.SOUND.Default.PartiallyFilled = GetSoundPath(rowCount);
							Configuration.SOUND.Default.EnablePartiallyFilled = GetSoundEnableFlag(rowCount);
							break;
						case "Rejected":
							Configuration.SOUND.Default.Rejected = GetSoundPath(rowCount);
							Configuration.SOUND.Default.EnableRejected = GetSoundEnableFlag(rowCount);
							break;
						case "Canceled":
							Configuration.SOUND.Default.Canceled = GetSoundPath(rowCount);
							Configuration.SOUND.Default.EnableCanceled = GetSoundEnableFlag(rowCount);
							break;
					}
				}

				Configuration.SOUND.Default.Save();
				GLOBAL.ROCSounds.ReloadSound();
				Close();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private string GetSoundPath(int rowCount)
		{
			if (DGVSounds.Rows[rowCount].Cells["Path"].Value != null)
			{
				return DGVSounds.Rows[rowCount].Cells["Path"].Value.ToString();
			}
			else
			{
				return @"..\Sounds\NoSound.WAV";
			}
		}

		private bool GetSoundEnableFlag(int rowCount)
		{
			if (DGVSounds.Rows[rowCount].Cells["Enable"].Value != null)
			{
				return (bool)DGVSounds.Rows[rowCount].Cells["Enable"].Value;
			}
			else
			{
				return false;
			}
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}
	}
}