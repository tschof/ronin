using System;
using System.ComponentModel.Design;

namespace PanelEx.Editor
{
	public class ManagedPanelCollectionEditor : CollectionEditor
	{
		public ManagedPanelCollectionEditor(Type type) : base(type) { }

		protected override Type CreateCollectionItemType()
		{
			return typeof(ManagedPanel);
		}
	}
}
