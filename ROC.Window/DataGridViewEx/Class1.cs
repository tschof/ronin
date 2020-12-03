//using System;
//using System.Collections.Generic;
//using System.Text;

//namespace DataGridViewEx
//{
//    public class OneClickEditDataGridView  : DataGridView

//    {

//        #region Operations

     

//        public void DefineSingleClickColumns(params DataGridViewColumn[] columns)

//        {

//            singleClickColumns.Clear();

             

//            foreach (DataGridViewColumn column in columns)

//            {

//                if (this.Columns.IndexOf(column) == -1)

//                {

//                    throw new ArgumentException("Instance of column (" + column.Name + ") is not in this DataGridView");

//                }

                 

//                singleClickColumns.Add(column);

//            }

//        }

     

//        public void DefineSingleClickColumns(params int[] columnIndexes)

//        {

//            singleClickColumns.Clear();

//            foreach (int columnIndex in columnIndexes)

//            {

//                if (columnIndex < 0 || columnIndex >= this.Columns.Count)

//                {

//                    throw new ArgumentOutOfRangeException("Column index (" + columnIndex + ") is out of range");

//                }

//                singleClickColumns.Add(this.Columns[columnIndex]);

//            }

//        }

     

//        protected void BaseOnMouseDown(MouseEventArgs e)

//        {

//            base.OnMouseDown(e);

//        }

         

//        #endregion

     

//        #region Overrides

     

//        protected override void OnMouseDown(MouseEventArgs e)

//        {

//            // If primary mouse button not down, do standard processing

//            if (e.Button != MouseButtons.Left)

//            {

//                base.OnMouseDown(e);

//                return;

//            }

             

//            // Get info on where user clicked

//            DataGridView.HitTestInfo hitInfo = HitTest(e.X, e.Y);

             

//            // If a cell wasn't clicked or column is read only, do standard processing

//            if (hitInfo.Type != DataGridViewHitTestType.Cell || this.Columns[hitInfo.ColumnIndex].ReadOnly)

//            {

//                base.OnMouseDown(e);

//                return;

//            }

             

//            // Get column clicked

//            DataGridViewColumn column = this.Columns[hitInfo.ColumnIndex];

             

//            // If column type already works with a single click, do standard processing

//            if (column is DataGridViewCheckBoxColumn || column is DataGridViewButtonColumn ||

//            column is DataGridViewLinkColumn || column is DataGridViewImageColumn)

//            {

//                base.OnMouseDown(e);

//                return;

//            }

             

//            // If functionality enabled for specific columns and column clicked is not

//            // one of these, do standard processing

//            if (singleClickColumns.Count >= 1 && singleClickColumns.IndexOf(column) == -1)

//            {

//                base.OnMouseDown(e);

//                return;

//            }

             

//            // Get clicked cell

//            DataGridViewCell clickedCell = this[hitInfo.ColumnIndex, hitInfo.RowIndex];

             

//            // If cell not current, try and make it so

//            if (this.CurrentCell != clickedCell)

//            {

//                // Allow standard processing make clicked cell current

//                base.OnMouseDown(e);

                 

//                // If this didn't happen (validation failed etc), abort

//                if (this.CurrentCell != clickedCell)

//                {

//                    return;

//                }

//            }

             

//            // If already in edit mode, do standard processing (will position caret)

//            if (this.CurrentCell.IsInEditMode)

//            {

//                base.OnMouseDown(e);

//                return;

//            }

             

//            // Enter edit mode

//            this.BeginEdit(false);

             

//            // For editing controls derived from TextBoxBase, ensure text is scrolled to the left

//            if (this.EditingControl is TextBoxBase)

//            {

//                TextBoxBase textBox = (TextBoxBase) this.EditingControl;

//                textBox.SelectionStart = 0;

//                textBox.ScrollToCaret();

//            }

             

//            // Calculate co-ordinates of where user clicked within editing control

//            System.Drawing.Point clickPoint = new System.Drawing.Point(

//                e.X - hitInfo.ColumnX - this.EditingControl.Left,

//                e.Y - hitInfo.RowY - this.EditingControl.Top);

                 

//            // Simulate mouse click within control

//            SimulateMouseClick(clickPoint);

//        }

         

//        #endregion

         

//        #region Implementation

         

//        const int WM_LBUTTONDOWN = 0x0201;

//        const int WM_LBUTTONUP = 0x0202;

         

//        private void SimulateMouseClick(System.Drawing.Point clickPoint)

//        {

//            // Check for child control within editing control

//            IntPtr childHwnd = ChildWindowFromPoint(this.EditingControl.Handle, clickPoint.X, clickPoint.Y);

             

//            if (childHwnd == null)

//            {

//                // Outside parent window - should not happen

//                return;

//            }

             

//            // If no child window, simulate mouse click within editing control

//            if (childHwnd == this.EditingControl.Handle)

//            {

//                SimulateMouseClick(this.EditingControl.Handle, clickPoint);

//            }

//            // Otherwise, simulate mouse click within child control

//            else

//            {

//                // Get screen co-ordinates of parent (edit control) and child

//                RECT parentRect, childRect;

                 

//                if (!GetWindowRect(this.EditingControl.Handle, out parentRect))

//                {

//                    return;

//                }

                 

//                if (!GetWindowRect(childHwnd, out childRect))

//                {

//                    return;

//                }

                 

//                // Determine co-ordinates of where user clicked within child control

//                System.Drawing.Point childClickPoint = new System.Drawing.Point(

//                    clickPoint.X - (childRect.Left - parentRect.Left),

//                    clickPoint.Y - (childRect.Top - parentRect.Top));

                 

//                // Simulate mouse click within child control

//                SimulateMouseClick(childHwnd, childClickPoint);

//            }

//        }

         

//        private void SimulateMouseClick(IntPtr hWnd, System.Drawing.Point clickPoint)

//        {

//            Int32 lParam = MakeLong(clickPoint.X, clickPoint.Y);

//            SendMessage(hWnd, WM_LBUTTONDOWN, 0, lParam);

//            SendMessage(hWnd, WM_LBUTTONUP, 0, lParam);

//        }

         

//        [System.Runtime.InteropServices.DllImport("user32.dll")]

//        static extern bool SendMessage(IntPtr hWnd, Int32 msg, Int32 wParam, Int32 lParam);

         

//        [System.Runtime.InteropServices.DllImport("user32.dll")]

//        static extern IntPtr ChildWindowFromPoint(IntPtr hwndParent, int x, int y);

         

//        [System.Runtime.InteropServices.DllImport("user32.dll")]

//        static extern bool GetWindowRect(System.IntPtr hWnd, out RECT lpRect);

         

//        List<DataGridViewColumn> singleClickColumns = new List<DataGridViewColumn>();

         

//        int MakeLong(int loWord, int hiWord)

//        {

//            return (hiWord << 16) | (loWord & 0xffff);

//        }

         

//        struct RECT

//        {

//            public System.Int32 Left;

//            public System.Int32 Top;

//            public System.Int32 Right;

//            public System.Int32 Bottom;

//        }

         

//        #endregion

//    }

//}
//}
