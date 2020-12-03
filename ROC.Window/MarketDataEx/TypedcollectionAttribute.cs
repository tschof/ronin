using System;
using System.Collections.Generic;
using System.Text;

namespace MarketDataEx
{
	[AttributeUsage(AttributeTargets.All)]
	public class TypedCollectionAttribute : Attribute
	{
		private Type UnderlyingType;

		public TypedCollectionAttribute(Type underlyingType)
		{
			UnderlyingType = underlyingType;
		}

		public Type CollectionType
		{
			get { return UnderlyingType; }
		}
	}
}
