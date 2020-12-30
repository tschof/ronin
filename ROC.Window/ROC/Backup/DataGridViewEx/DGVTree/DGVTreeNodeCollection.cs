using System;
using System.Collections.Generic;
using System.Collections;

namespace DataGridViewEx
{
	public class DGVTreeNodeCollection : IList<DGVTreeNode>, IList
	{
		internal System.Collections.Generic.List<DGVTreeNode> _list;
		internal DGVTreeNode _owner;
		internal DGVTreeNodeCollection(DGVTreeNode owner)
		{
			this._owner = owner;
			this._list = new List<DGVTreeNode>();
		}

		#region Public Members
		public void Add(DGVTreeNode item)
		{
			// The row needs to exist in the child collection before the parent is notified.
			item._grid = this._owner._grid;

			bool hadChildren = this._owner.HasChildren;
			item._owner = this;

			this._list.Add(item);

			this._owner.AddChildNode(item);

			// if the owner didn't have children but now does (asserted) and it is sited update it
			if (!hadChildren && this._owner.IsSited)
			{
				this._owner._grid.InvalidateRow(this._owner.RowIndex);
			}
		}

		public DGVTreeNode Add(string text)
		{
			DGVTreeNode node = new DGVTreeNode();
			this.Add(node);

			node.Cells[0].Value = text;
			return node;
		}

		public DGVTreeNode Add(params object[] values)
		{
			DGVTreeNode node = new DGVTreeNode();
			this.Add(node);

			int cell = 0;

			if (values.Length > node.Cells.Count)
				throw new ArgumentOutOfRangeException("values");

			foreach (object o in values)
			{
				node.Cells[cell].Value = o;
				cell++;
			}
			return node;
		}

		public void Insert(int index, DGVTreeNode item)
		{
			// The row needs to exist in the child collection before the parent is notified.
			item._grid = this._owner._grid;
			item._owner = this;

			this._list.Insert(index, item);

			this._owner.InsertChildNode(index, item);
		}

		public bool Remove(DGVTreeNode item)
		{
			// The parent is notified first then the row is removed from the child collection.
			this._owner.RemoveChildNode(item);
			item._grid = null;
			return this._list.Remove(item);
		}

		public void RemoveAt(int index)
		{
			DGVTreeNode row = this._list[index];

			// The parent is notified first then the row is removed from the child collection.
			this._owner.RemoveChildNode(row);
			row._grid = null;
			this._list.RemoveAt(index);
		}

		public void Clear()
		{
			// The parent is notified first then the row is removed from the child collection.
			this._owner.ClearNodes();
			this._list.Clear();
		}

		public int IndexOf(DGVTreeNode item)
		{
			return this._list.IndexOf(item);
		}

		public DGVTreeNode this[int index]
		{
			get
			{
				return this._list[index];
			}
			set
			{
				throw new Exception("The method or operation is not implemented.");
			}
		}

		public bool Contains(DGVTreeNode item)
		{
			return this._list.Contains(item);
		}

		public void CopyTo(DGVTreeNode[] array, int arrayIndex)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		public int Count
		{
			get { return this._list.Count; }
		}

		public bool IsReadOnly
		{
			get { return false; }
		}
		#endregion

		#region IList Interface
		void System.Collections.IList.Remove(object value)
		{
			this.Remove(value as DGVTreeNode);
		}


		int System.Collections.IList.Add(object value)
		{
			DGVTreeNode item = value as DGVTreeNode;
			this.Add(item);
			return item.Index;
		}

		void System.Collections.IList.RemoveAt(int index)
		{
			this.RemoveAt(index);
		}


		void System.Collections.IList.Clear()
		{
			this.Clear();
		}

		bool System.Collections.IList.IsReadOnly
		{
			get { return this.IsReadOnly; }
		}

		bool System.Collections.IList.IsFixedSize
		{
			get { return false; }
		}

		int System.Collections.IList.IndexOf(object item)
		{
			return this.IndexOf(item as DGVTreeNode);
		}

		void System.Collections.IList.Insert(int index, object value)
		{
			this.Insert(index, value as DGVTreeNode);
		}
		int System.Collections.ICollection.Count
		{
			get { return this.Count; }
		}
		bool System.Collections.IList.Contains(object value)
		{
			return this.Contains(value as DGVTreeNode);
		}
		void System.Collections.ICollection.CopyTo(Array array, int index)
		{
			throw new Exception("The method or operation is not implemented.");
		}
		object System.Collections.IList.this[int index]
		{
			get
			{
				return this[index];
			}
			set
			{
				throw new Exception("The method or operation is not implemented.");
			}
		}



		#region IEnumerable<ExpandableRow> Members

		public IEnumerator<DGVTreeNode> GetEnumerator()
		{
			return this._list.GetEnumerator();
		}

		#endregion


		#region IEnumerable Members

		System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
		{
			return this.GetEnumerator();
		}

		#endregion
		#endregion

		#region ICollection Members

		bool System.Collections.ICollection.IsSynchronized
		{
			get { throw new Exception("The method or operation is not implemented."); }
		}

		object System.Collections.ICollection.SyncRoot
		{
			get { throw new Exception("The method or operation is not implemented."); }
		}

		#endregion
	}
}
