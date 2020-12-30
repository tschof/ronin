using System;
using System.Data;
using System.Collections;
using System.Collections.Generic;

namespace DataSetEx
{
	public class DataSetHelper
	{
		private class FieldInfo
		{
			public string RelationName;
			public string FieldName;	//source table field name
			public string FieldAlias;	//destination table field name
			public string Aggregate;
		}

		private ArrayList _fieldInfo; 
		private string _fieldList;
		private ArrayList _groupByFieldInfo; 
		private string _groupByFieldList;

		private DataSet _ds;
		public DataSet DS
		{
			get
			{
				return _ds;
			}
		}

		public DataSetHelper()
		{
		}
		public DataSetHelper(ref DataSet ds)
		{
			_ds = ds;
		}

		private void ParseFieldList(string fieldList, bool allowRelation)
		{
			/*
			 * This code parses FieldList into FieldInfo objects  and then 
			 * adds them to the m_FieldInfo private member
			 * 
			 * FieldList systax:  [relationname.]fieldname[ alias], ...
			*/
			if (_fieldList == fieldList) return;
			_fieldInfo = new System.Collections.ArrayList();
			_fieldList = fieldList;
			FieldInfo Field; string[] FieldParts; string[] Fields = fieldList.Split(',');
			int i;
			for (i = 0; i <= Fields.Length - 1; i++)
			{
				Field = new FieldInfo();
				//parse FieldAlias
				FieldParts = Fields[i].Trim().Split(' ');
				switch (FieldParts.Length)
				{
					case 1:
						//to be set at the end of the loop
						break;
					case 2:
						Field.FieldAlias = FieldParts[1];
						break;
					default:
						throw new Exception("Too many spaces in field definition: '" + Fields[i] + "'.");
				}
				//parse FieldName and RelationName
				FieldParts = FieldParts[0].Split('.');
				switch (FieldParts.Length)
				{
					case 1:
						Field.FieldName = FieldParts[0];
						break;
					case 2:
						if (allowRelation == false)
							throw new Exception("Relation specifiers not permitted in field list: '" + Fields[i] + "'.");
						Field.RelationName = FieldParts[0].Trim();
						Field.FieldName = FieldParts[1].Trim();
						break;
					default:
						throw new Exception("Invalid field definition: " + Fields[i] + "'.");
				}
				if (Field.FieldAlias == null)
					Field.FieldAlias = Field.FieldName;
				_fieldInfo.Add(Field);
			}
		}

		private void ParseGroupByFieldList(string fieldList)
		{
			/*
			* Parses FieldList into FieldInfo objects and adds them to the GroupByFieldInfo private member
			* 
			* FieldList syntax: fieldname[ alias]|operatorname(fieldname)[ alias],...
			* 
			* Supported Operators: count,sum,max,min,first,last
			*/
			if (_groupByFieldList == fieldList) return;
			_groupByFieldInfo = new System.Collections.ArrayList();
			FieldInfo Field; string[] FieldParts; string[] Fields = fieldList.Split(',');
			for (int i = 0; i <= Fields.Length - 1; i++)
			{
				Field = new FieldInfo();
				//Parse FieldAlias
				FieldParts = Fields[i].Trim().Split(' ');
				switch (FieldParts.Length)
				{
					case 1:
						//to be set at the end of the loop
						break;
					case 2:
						Field.FieldAlias = FieldParts[1];
						break;
					default:
						throw new ArgumentException("Too many spaces in field definition: '" + Fields[i] + "'.");
				}
				//Parse FieldName and Aggregate
				FieldParts = FieldParts[0].Split('(');
				switch (FieldParts.Length)
				{
					case 1:
						Field.FieldName = FieldParts[0];
						break;
					case 2:
						Field.Aggregate = FieldParts[0].Trim().ToLower();    //we're doing a case-sensitive comparison later
						Field.FieldName = FieldParts[1].Trim(' ', ')');
						break;
					default:
						throw new ArgumentException("Invalid field definition: '" + Fields[i] + "'.");
				}
				if (Field.FieldAlias == null)
				{
					if (Field.Aggregate == null)
						Field.FieldAlias = Field.FieldName;
					else
						Field.FieldAlias = Field.Aggregate + "of" + Field.FieldName;
				}
				_groupByFieldInfo.Add(Field);
			}
			_groupByFieldList = fieldList;
		}

		public DataTable CreateGroupByTable(
			string tableName, 
			DataTable sourceTable, 
			string fieldList)
		{
			/*
			 * Creates a table based on aggregates of fields of another table
			 * 
			 * RowFilter affects rows before GroupBy operation. No "Having" support
			 * though this can be emulated by subsequent filtering of the table that results
			 * 
			 *  FieldList syntax: fieldname[ alias]|aggregatefunction(fieldname)[ alias], ...
			*/
			if (fieldList == null)
			{
				throw new ArgumentException("You must specify at least one field in the field list.");
				//return CreateTable(TableName, SourceTable);
			}
			else
			{
				DataTable dt = new DataTable(tableName);
				ParseGroupByFieldList(fieldList);
				foreach (FieldInfo Field in _groupByFieldInfo)
				{
					DataColumn dc = sourceTable.Columns[Field.FieldName];
					if (Field.Aggregate == null)
						dt.Columns.Add(Field.FieldAlias, dc.DataType, dc.Expression);
					else
						dt.Columns.Add(Field.FieldAlias, dc.DataType);
				}

				if (_ds != null)
				{
					_ds.Tables.Add(dt);
				}
				return dt;
			}
		}

		public void InsertGroupByInto(
			DataTable destTable,
			DataTable sourceTable, 
			string fieldList, 
			string rowFilter, 
			string groupBy)
		{
			/*
			 * Copies the selected rows and columns from SourceTable and inserts them into DestTable
			 * FieldList has same format as CreateGroupByTable
			*/
			if (fieldList == null)
			{
				throw new ArgumentException("You must specify at least one field in the field list.");
			}
			
			ParseGroupByFieldList(fieldList);	//parse field list
			ParseFieldList(groupBy, false);			//parse field names to Group By into an arraylist
			
			DataRow[] rows = sourceTable.Select(rowFilter, groupBy);
			DataRow lastSourceRow = null;
			DataRow destRow = null; 
			bool sameRow; 
			int rowCount = 0;
			
			foreach (DataRow sourceRow in rows)
			{
				sameRow = false;
				if (lastSourceRow != null)
				{
					sameRow = true;
					foreach (FieldInfo field in _fieldInfo)
					{
						if (!ColumnEqual(lastSourceRow[field.FieldName], sourceRow[field.FieldName]))
						{
							sameRow = false;
							break;
						}
					}
					if (!sameRow)
					{
						destTable.Rows.Add(destRow);
					}
				}
				if (!sameRow)
				{
					destRow = destTable.NewRow();
					rowCount = 0;
				}
				rowCount += 1;

				foreach (FieldInfo field in _groupByFieldInfo)
				{
					switch (field.Aggregate)    //this test is case-sensitive
					{
						case null:				//implicit last
						case "":				//implicit last
						case "last":
							destRow[field.FieldAlias] = sourceRow[field.FieldName];
							break;
						case "first":
							if (rowCount == 1)
								destRow[field.FieldAlias] = sourceRow[field.FieldName];
							break;
						case "count":
							destRow[field.FieldAlias] = rowCount;
							break;
						case "sum":
							destRow[field.FieldAlias] = Add(destRow[field.FieldAlias], sourceRow[field.FieldName]);
							break;
						case "max":
							destRow[field.FieldAlias] = Max(destRow[field.FieldAlias], sourceRow[field.FieldName]);
							break;
						case "min":
							if (rowCount == 1)
								destRow[field.FieldAlias] = sourceRow[field.FieldName];
							else
								destRow[field.FieldAlias] = Min(destRow[field.FieldAlias], sourceRow[field.FieldName]);
							break;
					}
				}
				lastSourceRow = sourceRow;
			}
			if (destRow != null)
			{
				destTable.Rows.Add(destRow);
			}
		}

		private FieldInfo LocateFieldInfoByName(ArrayList fieldList, string name)
		{
			//Looks up a FieldInfo record based on FieldName
			foreach (FieldInfo field in fieldList)
			{
				if (field.FieldName == name)
				{
					return field;
				}
			}
			return null;
		}

		private bool ColumnEqual(object a, object b)
		{
			/*
			 * Compares two values to see if they are equal. Also compares DBNULL.Value.
			 * 
			 * Note: If your DataTable contains object fields, you must extend this
			 * function to handle them in a meaningful way if you intend to group on them.
			*/
			if ((a is DBNull) && (b is DBNull))
			{
				return true;    //both are null
			}
			if ((a is DBNull) || (b is DBNull))
			{
				return false;    //only one is null
			}
			return (a == b);    //value type standard comparison
		}

		private object Min(object a, object b)
		{
			//Returns MIN of two values - DBNull is less than all others
			if ((a is DBNull) || (b is DBNull))
			{
				return DBNull.Value;
			}
			if (((IComparable)a).CompareTo(b) == -1)
			{
				return a;
			}
			else
			{
				return b;
			}
		}

		private object Max(object a, object b)
		{
			//Returns Max of two values - DBNull is less than all others
			if (a is DBNull)
			{
				return b;
			}
			if (b is DBNull)
			{
				return a;
			}
			if (((IComparable)a).CompareTo(b) == 1)
			{
				return a;
			}
			else
			{
				return b;
			}
		}

		private object Add(object a, object b)
		{
			//Adds two values - if one is DBNull, then returns the other
			if (a is DBNull)
			{
				return b;
			}
			if (b is DBNull)
			{
				return a;
			}

			return ((decimal)a + (decimal)b);
		}

		public DataTable SelectGroupByInto(
			string tableName, 
			DataTable sourceTable, 
			string fieldList, 
			string rowFilter, 
			string groupBy)
		{
			/*
			 * Selects data from one DataTable to another and performs various aggregate functions
			 * along the way. See InsertGroupByInto and ParseGroupByFieldList for supported aggregate functions.
			 */
			DataTable dt = CreateGroupByTable(tableName, sourceTable, fieldList);
			InsertGroupByInto(dt, sourceTable, fieldList, rowFilter, groupBy);
			return dt;
		}

		public DataTable SelectDistinct(
			string tableName, 
			DataTable sourceTable, 
			string fieldName)
		{
			DataTable dt = new DataTable(tableName);
			foreach (DataColumn col in sourceTable.Columns)
			{
				dt.Columns.Add(col.ColumnName, col.DataType);
			}

			List<object> lastValues = new List<object>();
			object currentValue;
			foreach (DataRow row in sourceTable.Rows)
			{
				currentValue = row[fieldName];
				if (!lastValues.Contains(currentValue))
				{
					lastValues.Add(currentValue);
					dt.Rows.Add(row.ItemArray);
				}
			}
			if (_ds != null)
			{
				_ds.Tables.Add(dt);
			}
			return dt;
		}
	}
}
