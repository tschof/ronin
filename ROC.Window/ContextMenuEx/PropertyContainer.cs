using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

namespace ContextMenuEx
{
	[ToolboxItem(false)]
	[System.ComponentModel.DesignerCategory("Code")]
	public class PropertyContainer : ToolStripDropDown
	{
		#region - Local Variable -

		private Control m_contents;

        private ToolStripControlHost m_host;

        private bool m_fade = true;
        private const int frames = 5;
        private const int totalduration = 200;
        private const int frameduration = totalduration / frames;

		#endregion

		#region - Constructor -

		public PropertyContainer(Control contents)
        {
			if (contents == null)
			{
				throw new ArgumentNullException("contents can not be null.");
			}

            m_contents = contents;
            contents.Location = Point.Empty;

            m_host = new ToolStripControlHost(contents);
            m_host.AutoSize = false;
            m_host.Padding = m_host.Margin = Padding.Empty;

            this.Padding = this.Margin = Padding.Empty;
            m_fade = SystemInformation.IsMenuAnimationEnabled && SystemInformation.IsMenuFadeEnabled;
           
            this.Items.Add(m_host);
		}

		#endregion

		#region - Show -

		public void Show(Control control)
		{
			Show(control, control.ClientRectangle);
		}
		public void Show(Form f, Point p)
		{
			Show(f, new Rectangle(p, new Size(0, 0)));
		}
        private void Show(Control control, Rectangle area)
        {
            if (control == null)
                throw new ArgumentNullException("control can not be null");

            Point location = control.PointToScreen(new Point(area.Left, area.Top + area.Height));

            Rectangle screen = Screen.FromControl(control).WorkingArea;

            if (location.X + Size.Width > (screen.Left + screen.Width))
                location.X = (screen.Left + screen.Width) - Size.Width;

            if (location.Y + Size.Height > (screen.Top + screen.Height))
                location.Y -= Size.Height + area.Height;

            location = control.PointToClient(location);

            Show(control, location, ToolStripDropDownDirection.BelowRight);
		}

		#endregion

		#region - Events -

		protected override bool ProcessDialogKey(Keys keyData)
		{
			//prevent alt from closing it and allow alt+menumonic to work
			if ((keyData & Keys.Alt) == Keys.Alt)
			{
				return false;
			}

			// solve the tab problem
			if (Keys.Tab == keyData)
			{
				Control lastControl = null;
				foreach (Control c in m_contents.Controls)
				{
					if (c.Focused)
					{
						lastControl = c;
					}
				}

				m_contents.SelectNextControl(lastControl, true, true, true, true);
			}


			if ((keyData & Keys.Shift) == Keys.Shift)
				if ((keyData & Keys.Tab) == Keys.Tab)
				{
					Control firstControl = null;
					foreach (Control c in m_contents.Controls)
					{
						if (c.Focused)
						{
							firstControl = c;
						}
					}

					m_contents.SelectNextControl(firstControl, false, true, true, true);
				}

			return base.ProcessDialogKey(keyData);
		}

		//fade it while displaying it
        protected override void SetVisibleCore(bool visible)
        {
            double opacity = Opacity;
            if (visible && m_fade) Opacity = 0;
            base.SetVisibleCore(visible);
            if (!visible || !m_fade) return;
            for (int i = 1; i <= frames; i++)
            {
                if (i > 1)
                {
                    System.Threading.Thread.Sleep(frameduration);
                }
                Opacity = opacity * (double)i / (double)frames;
            }
            Opacity = opacity;
        }

        protected override void OnOpened(EventArgs e)
        {
            m_contents.Focus();

            base.OnOpened(e);
		}

		#endregion
	}
}
