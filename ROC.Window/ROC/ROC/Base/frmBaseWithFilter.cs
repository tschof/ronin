using System;
using System.Collections.Generic;
using System.Text;
using ContextMenuEx;
using System.Windows.Forms;
using ButtonEx;

namespace ROC.Base
{
	public class frmBaseWithFilter
	{
		private DataGridView _dgv;
		private VistaButton _button;
		private List<string> _filters;

		#region - Type Filter -

		private menuBaseSecurityFilter _menuSecFilter;
		private PropertyContainer _menuSecFilterContainer;

		public void Show(DataGridView dgv, VistaButton button, List<string> filters)
		{
			_dgv = dgv;
			_button = button;
			_filters = filters;

			if (_menuSecFilter == null)
			{
				_menuSecFilter = new menuOrdersFilter(rocOrdersList.FilterOutStock, rocOrdersList.FilterOutFuture, rocOrdersList.FilterOutOption);
				_menuSecFilter.PropertyChanged += new PropertyChangedEventHandler(menuOrdersFilter_PropertyChanged);
			}

			if (_menuSecFilterContainer == null)
			{
				_menuSecFilterContainer = new PropertyContainer(_menuSecFilter);
				_menuSecFilterContainer.Closed += new ToolStripDropDownClosedEventHandler(menuOrdersFilterContainer_Closed);
			}

			_menuSecFilterContainer.Show((VistaButton)sender);
		}

		private void menuOrdersFilter_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ShowStocks":
					rocOrdersList.FilterOutStock = _menuSecFilter.HideStocks;
					break;
				case "ShowFutures":
					rocOrdersList.FilterOutFuture = _menuSecFilter.HideFutures;
					break;
				case "ShowOptions":
					rocOrdersList.FilterOutOption = _menuSecFilter.HideOptions;
					break;
			}
		}

		private void menuOrdersFilterContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuSecFilterContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuOrdersFilterContainer_Closed);
			_menuSecFilterContainer = null;
			_menuSecFilter.PropertyChanged -= new PropertyChangedEventHandler(menuOrdersFilter_PropertyChanged);
			_menuSecFilter = null;
		}

		#endregion
	}
}
