using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public class FutureMatrixSupport : BaseTicket
	{
		internal bool IsFuture
		{
			get
			{
				if (CurrentSecInfo != null &&
					CurrentSecInfo.SecType == CSVEx.CSVFieldIDs.SecutrityTypes.Future)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		internal sealed class MatrixPosition
		{
			public MatrixPosition(int columnIndex, int rowIndex)
			{
				ColumnIndex = columnIndex;
				RowIndex = rowIndex;
			}

			public int ColumnIndex = 0;
			public int RowIndex = 0;
		}

		private List<FutureMatrixColumn> _matrixColumns;
		internal List<FutureMatrixColumn> MatrixColumns
		{
			get
			{
				if (_matrixColumns == null)
				{
					_matrixColumns = new List<FutureMatrixColumn>();
				}
				return _matrixColumns;
			}
			set
			{
				_matrixColumns = value;
			}
		}


		// Used on first Load before IM is processed
		private Dictionary<string, MatrixPosition> _symbolDetailmatrix = null;
		internal Dictionary<string, MatrixPosition> SymbolDetailMatrix
		{
			get
			{
				if (_symbolDetailmatrix == null)
				{
					_symbolDetailmatrix = new Dictionary<string, MatrixPosition>();
				}
				return _symbolDetailmatrix;
			}
			set
			{
				_symbolDetailmatrix = value;
			}
		}

		// Used By Process Market Data
		private Dictionary<string, MatrixPosition> _mdSymbolMatrix = null;
		internal Dictionary<string, MatrixPosition> MDSymbolMatrix
		{
			get
			{
				if (_mdSymbolMatrix == null)
				{
					_mdSymbolMatrix = new Dictionary<string, MatrixPosition>();
				}
				return _mdSymbolMatrix;
			}
			set
			{
				_mdSymbolMatrix = value;
			}
		}
	}
}
