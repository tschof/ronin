using System;
using System.ComponentModel;
using System.Collections.Generic;
using SerializationEx;

namespace BindingListEx
{
	//SearchableSortableBindingList
	public abstract class BindingListBase<T> : BindingList<T>
	{
		#region Sorting

		private bool _isSorted;
		private PropertyDescriptor _sortProperty;
		private ListSortDirection _sortDirection;

		protected override bool SupportsSortingCore
		{
			get 
			{ 
				return true; 
			}
		}

		protected override ListSortDirection SortDirectionCore
		{
			get 
			{ 
				return _sortDirection; 
			}
		}

		protected override PropertyDescriptor SortPropertyCore
		{
			get 
			{ 
				return _sortProperty; 
			}
		}

		protected override void ApplySortCore(PropertyDescriptor property, ListSortDirection direction)
		{

			// Get list to sort
			// Note: this.Items is a non-sortable ICollection<T>
			List<T> items = this.Items as List<T>;

			// Apply and set the sort, if items to sort
			if (items != null)
			{
				PropertyComparer<T> pc = new PropertyComparer<T>(property, direction);
				items.Sort(pc);
				_isSorted = true;
			}
			else
			{
				_isSorted = false;
			}

			_sortProperty = property;
			_sortDirection = direction;

			this.OnListChanged(new ListChangedEventArgs(ListChangedType.Reset, -1));
		}

		protected override bool IsSortedCore
		{
			get { return _isSorted; }
		}

		protected override void RemoveSortCore()
		{
			_isSorted = false;
			this.OnListChanged(new ListChangedEventArgs(ListChangedType.Reset, -1));
		}

		#endregion

		#region - Persistence -

		public abstract string Export();
		public abstract void Import(string s);

		#endregion

		#region - Searching -

		protected override bool SupportsSearchingCore
		{
			get { return true; }
		}

		protected override int FindCore(PropertyDescriptor property, object key)
		{

			// Specify search columns
			if (property == null) return -1;

			// Get list to search
			List<T> items = this.Items as List<T>;

			// Traverse list for value
			foreach (T item in items)
			{
				// Test column search value
				string value = (string)property.GetValue(item);

				// If value is the search value, return the index of the data item
				if ((string)key == value) return IndexOf(item);
			}
			return -1;
		}

		#endregion
	}
}
