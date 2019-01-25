/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 二维动态数组
\*************************************************************************/
#ifndef __VAYO_TABLE_H__
#define __VAYO_TABLE_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

template<typename T>
class Table
{
public:
	Table()
		: _rows(0), _cols(0)
	{
	}

	Table(int m, int n)
		: _rows(m), _cols(n), _matrix(m*n)
	{
	}

	Table(int m, int n, const T& value)
		: _rows(m), _cols(n), _matrix(m*n, value)
	{
	}

	// For non-const objects
	T& operator()(int i, int j)
	{
		return _matrix[i*_cols + j];
	}

	// For const objects
	const T& operator()(int i, int j)const
	{
		return _matrix[i*_cols + j];
	}

	// Add typename to let compiler know type and not static variable.
	typedef typename std::vector<T>::iterator iter;
	typedef typename std::vector<T>::const_iterator citer;

	// For non-const objects
	iter begin() { return _matrix.begin(); }
	iter end() { return _matrix.end(); }

	// For const objects
	citer begin() const { return _matrix.begin(); }
	citer end() const { return _matrix.end(); }

	int numRows() const { return _rows; }
	int numCols() const { return _cols; }

	void resize(int m, int n)
	{
		_rows = m;
		_cols = n;
		_matrix.resize(m*n);
	}

	void resize(int m, int n, const T& value)
	{
		_rows = m;
		_cols = n;
		_matrix.resize(m*n, value);
	}

private:
	int _rows;
	int _cols;
	std::vector<T> _matrix;
};

NS_VAYO_END

#endif // __VAYO_TABLE_H__
