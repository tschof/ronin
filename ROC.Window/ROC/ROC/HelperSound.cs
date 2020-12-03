using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Media;

namespace ROC
{
	public class HelperSound
	{
		[DllImport("winmm.dll", SetLastError = true)]
		private static extern bool PlaySound(string pszSound, UIntPtr hmod, uint fdwSound);

		[DllImport("winmm.dll", SetLastError = true)]
		private static extern bool PlaySound(byte[] pszSound, IntPtr hmod, SoundFlags fdwSound);

		[Flags]
		public enum SoundFlags
		{
			/// <summary>play synchronously (default)</summary>
			SND_SYNC = 0x0000,
			/// <summary>play asynchronously</summary>
			SND_ASYNC = 0x0001,
			/// <summary>silence (!default) if sound not found</summary>
			SND_NODEFAULT = 0x0002,
			/// <summary>pszSound points to a memory file</summary>
			SND_MEMORY = 0x0004,
			/// <summary>loop the sound until next sndPlaySound</summary>
			SND_LOOP = 0x0008,
			/// <summary>don't stop any currently playing sound</summary>
			SND_NOSTOP = 0x0010,
			/// <summary>Stop Playing Wave</summary>
			SND_PURGE = 0x40,
			/// <summary>don't wait if the driver is busy</summary>
			SND_NOWAIT = 0x00002000,
			/// <summary>name is a registry alias</summary>
			SND_ALIAS = 0x00010000,
			/// <summary>alias is a predefined id</summary>
			SND_ALIAS_ID = 0x00110000,
			/// <summary>name is file name</summary>
			SND_FILENAME = 0x00020000,
			/// <summary>name is resource name or atom</summary>
			SND_RESOURCE = 0x00040004
		}

		public void APIPlay(string strFileName)
		{
			PlaySound(strFileName, UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}
		public void APIPlayOpen()
		{
			PlaySound(Configuration.SOUND.Default.Open.Replace(@"..\", ""), UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}
		public void APIPlayReplaced()
		{
			PlaySound(Configuration.SOUND.Default.Replaced.Replace(@"..\", ""), UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}
		public void APIPlayFilled()
		{
			PlaySound(Configuration.SOUND.Default.Filled.Replace(@"..\", ""), UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}
		public void APIPlayFilledAndCanceled()
		{
			PlaySound(Configuration.SOUND.Default.FilledAndCanceled.Replace(@"..\", ""), UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}
		public void APIPlayPartiallyFilled()
		{
			PlaySound(Configuration.SOUND.Default.PartiallyFilled.Replace(@"..\", ""), UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}
		public void APIPlayRejected()
		{
			PlaySound(Configuration.SOUND.Default.Rejected.Replace(@"..\", ""), UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}
		public void APIPlayCanceled()
		{
			PlaySound(Configuration.SOUND.Default.Canceled.Replace(@"..\", ""), UIntPtr.Zero, (uint)(SoundFlags.SND_FILENAME | SoundFlags.SND_ASYNC));
		}

		private SoundPlayer _openSound = new SoundPlayer(Configuration.SOUND.Default.Open.Replace(@"..\", ""));
		private SoundPlayer _replacedSound = new SoundPlayer(Configuration.SOUND.Default.Replaced.Replace(@"..\", ""));

		private SoundPlayer _filledSound = new SoundPlayer(Configuration.SOUND.Default.Filled.Replace(@"..\", ""));
		private SoundPlayer _filledAndCanceledSound = new SoundPlayer(Configuration.SOUND.Default.FilledAndCanceled.Replace(@"..\", ""));
		private SoundPlayer _partiallyFilledSound = new SoundPlayer(Configuration.SOUND.Default.PartiallyFilled.Replace(@"..\", ""));

		private SoundPlayer _rejectedSound = new SoundPlayer(Configuration.SOUND.Default.Rejected.Replace(@"..\", ""));
		private SoundPlayer _canceledSound = new SoundPlayer(Configuration.SOUND.Default.Canceled.Replace(@"..\", ""));

		public void PlayOpen()
		{
			PlayOpen("");
		}
		public void PlayOpen(string strFileName)
		{
			if (Configuration.SOUND.Default.EnableOpen)
			{
				if (strFileName != "")
				{
					_openSound = new SoundPlayer(strFileName);
				}
				try
				{
					if (_openSound.IsLoadCompleted)
					{
						_openSound.Stop();
						_openSound.Play();
					}
					else
					{
						_openSound.LoadAsync();
						_openSound.Play();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void PlayReplaced()
		{
			PlayReplaced("");
		}
		public void PlayReplaced(string strFileName)
		{
			if (Configuration.SOUND.Default.EnableReplaced)
			{
				if (strFileName != "")
				{
					_replacedSound = new SoundPlayer(strFileName);
				}
				try
				{
					if (_replacedSound.IsLoadCompleted)
					{
						_replacedSound.Stop();
						_replacedSound.Play();
					}
					else
					{
						_replacedSound.LoadAsync();
						_replacedSound.Play();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void PlayFilled()
		{
			PlayFilled("");
		}
		public void PlayFilled(string strFileName)
		{
			if (Configuration.SOUND.Default.EnableFilled)
			{
				if (strFileName != "")
				{
					_filledSound = new SoundPlayer(strFileName);
				}
				try
				{
					if (_filledSound.IsLoadCompleted)
					{
						_filledSound.Stop();
						_filledSound.Play();
					}
					else
					{
						_filledSound.LoadAsync();
						_filledSound.Play();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void PlayFilledAndCanceled()
		{
			PlayFilledAndCanceled("");
		}
		public void PlayFilledAndCanceled(string strFileName)
		{
			if (Configuration.SOUND.Default.EnableFilledAndCanceled)
			{
				if (strFileName != "")
				{
					_filledAndCanceledSound = new SoundPlayer(strFileName);
				}
				try
				{
					if (_filledAndCanceledSound.IsLoadCompleted)
					{
						_filledAndCanceledSound.Stop();
						_filledAndCanceledSound.Play();
					}
					else
					{
						_filledAndCanceledSound.LoadAsync();
						_filledAndCanceledSound.Play();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void PlayPartiallyFilled()
		{
			PlayPartiallyFilled("");
		}
		public void PlayPartiallyFilled(string strFileName)
		{
			if (Configuration.SOUND.Default.EnablePartiallyFilled)
			{
				if (strFileName != "")
				{
					_partiallyFilledSound = new SoundPlayer(strFileName);
				}
				try
				{
					if (_partiallyFilledSound.IsLoadCompleted)
					{
						_partiallyFilledSound.Stop();
						_partiallyFilledSound.Play();
					}
					else
					{
						_partiallyFilledSound.LoadAsync();
						_partiallyFilledSound.Play();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void PlayCanceled()
		{
			PlayCanceled("");
		}
		public void PlayCanceled(string strFileName)
		{
			if (Configuration.SOUND.Default.EnableCanceled)
			{
				if (strFileName != "")
				{
					_canceledSound = new SoundPlayer(strFileName);
				}
				try
				{
					if (_canceledSound.IsLoadCompleted)
					{
						_canceledSound.Stop();
						_canceledSound.Play();
					}
					else
					{
						_canceledSound.LoadAsync();
						_canceledSound.Play();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void PlayRejected()
		{
			PlayRejected("");
		}
		public void PlayRejected(string strFileName)
		{
			if (Configuration.SOUND.Default.EnableRejected)
			{
				if (strFileName != "")
				{
					_rejectedSound = new SoundPlayer(strFileName);
				}
				try
				{
					if (_rejectedSound.IsLoadCompleted)
					{
						_rejectedSound.Stop();
						_rejectedSound.Play();
					}
					else
					{
						_rejectedSound.LoadAsync();
						_rejectedSound.Play();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void ReloadSound()
		{
			_openSound = new SoundPlayer(Configuration.SOUND.Default.Open.Replace(@"..\", ""));
			_replacedSound = new SoundPlayer(Configuration.SOUND.Default.Replaced.Replace(@"..\", ""));

			_filledSound = new SoundPlayer(Configuration.SOUND.Default.Filled.Replace(@"..\", ""));
			_filledAndCanceledSound = new SoundPlayer(Configuration.SOUND.Default.FilledAndCanceled.Replace(@"..\", ""));
			_partiallyFilledSound = new SoundPlayer(Configuration.SOUND.Default.PartiallyFilled.Replace(@"..\", ""));

			_rejectedSound = new SoundPlayer(Configuration.SOUND.Default.Rejected.Replace(@"..\", ""));
			_canceledSound = new SoundPlayer(Configuration.SOUND.Default.Canceled.Replace(@"..\", ""));
		}
	}
}
