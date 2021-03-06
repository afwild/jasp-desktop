
#include "dataset.h"

/*
 * DataSet is implemented as a set of columns
 */

DataSet::DataSet() :
	_columns()
{
	_rowCount = 0;
	_columnCount = 0;
}

DataSet::~DataSet()
{
}

Columns &DataSet::columns()
{
	return _columns;
}

void DataSet::setRowCount(int rowCount)
{
	_columns.setRowCount(rowCount);
	_rowCount = rowCount;
}

void DataSet::setColumnCount(int columnCount)
{
	_columns.setColumnCount(columnCount);
	_columnCount = columnCount;
}

int DataSet::rowCount()
{
	return _rowCount;
}

int DataSet::columnCount()
{
	return _columnCount;
}

