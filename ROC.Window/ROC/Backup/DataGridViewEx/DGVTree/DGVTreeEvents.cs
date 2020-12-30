using System;
using System.Collections.Generic;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVTreeNodeEventBase
	{
		private DGVTreeNode _node;

		public DGVTreeNodeEventBase(DGVTreeNode node)
		{
			this._node = node;
		}

		public DGVTreeNode Node
		{
			get { return _node; }
		}
	}
	public class CollapsingEventArgs : CancelEventArgs
	{
		private DGVTreeNode _node;

		private CollapsingEventArgs() { }
		public CollapsingEventArgs(DGVTreeNode node)
			: base()
		{
			this._node = node;
		}
		public DGVTreeNode Node
		{
			get { return _node; }
		}

	}
	public class CollapsedEventArgs : DGVTreeNodeEventBase
	{
		public CollapsedEventArgs(DGVTreeNode node)
			: base(node)
		{
		}
	}

	public class ExpandingEventArgs: CancelEventArgs
	{
		private DGVTreeNode _node;

		private ExpandingEventArgs() { }
		public ExpandingEventArgs(DGVTreeNode node):base()
		{
			this._node = node;
		}
		public DGVTreeNode Node
		{
			get { return _node; }
		}

	}
	public class ExpandedEventArgs : DGVTreeNodeEventBase
	{
		public ExpandedEventArgs(DGVTreeNode node):base(node)
		{
		}
	}

	public delegate void ExpandingEventHandler(object sender, ExpandingEventArgs e);
	public delegate void ExpandedEventHandler(object sender, ExpandedEventArgs e);

	public delegate void CollapsingEventHandler(object sender, CollapsingEventArgs e);
	public delegate void CollapsedEventHandler(object sender, CollapsedEventArgs e);
}
