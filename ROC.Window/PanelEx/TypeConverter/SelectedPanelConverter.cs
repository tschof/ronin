using System;
using System.ComponentModel;

namespace PanelEx.TypeConverter
{
	public class SelectedPanelConverter : ReferenceConverter
	{
		public SelectedPanelConverter() : base(typeof(ManagedPanel)) { }

		protected override bool IsValueAllowed(ITypeDescriptorContext context, object value)
		{
			if (context != null)
			{
				PanelManager pm = (PanelManager)context.Instance;
				return pm.ManagedPanels.Contains((ManagedPanel)value);
			}
			return false;
		}
	}
}
