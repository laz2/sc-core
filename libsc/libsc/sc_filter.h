/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef __LIBSC_SC_FILTER_H_INCLUDED__
#define __LIBSC_SC_FILTER_H_INCLUDED__

/*!
 * @addtogroup libsc
 * @{
 */

/*!
 * @file sc_filter.h
 * @brief Filters for #sc_iterator.
 **/

#include "libsc.h"

class sc_single_filter
{
public:
	virtual bool check_value(sc_addr val) = 0;
	virtual ~sc_single_filter() {}
};

/**
 * Шаблон класса фильтра, который фильтрует результат на основе 
 * отдельного фильтра для каждого элемента найденной структуры.
 **/
template <int width>
class sc_iterator_complex_filter : public sc_iterator
{
private:

	/**
	 * Проверяет результат на соответствие фильтрам.
	 * @return true результат прошел проверку.
	 *         false результат не прошел проверку.
	 */
	bool check_tuple()
	{
		for (int i = 0; i < width; i++) {
			sc_addr val = source->value(i);

			if (!val) {
				return false;
			}

			if (filters[i] && !filters[i]->check_value(val)) {
				return false;
			}
		}

		return true;
	}

public:
	
	/**
	 * @param *_source итератор.
	 * @param _sink флаг удаления итератора.
	 * @param _sink_filters флаг удаления фильтров.
	 */
	sc_iterator_complex_filter(sc_iterator *_source, bool _sink, bool _sink_filters = false)
	{
		source = _source;
		sink = _sink;
		sink_filters = _sink_filters;
		memset(filters, 0, sizeof(void *)*width);
	}

	~sc_iterator_complex_filter()
	{
		if (sink) {
			delete source;
		}
		
		if (sink_filters) {
			for (int i = 0; i < width; i++) {
				delete filters[i];
			}
		}
	}

	sc_retval next()
	{
		while (!source->is_over()) {
			source->next();

			if (check_tuple()) {
				return RV_OK;
			}
		}

		return RV_ELSE_GEN;
	}

	sc_addr value(sc_uint num) 
	{
		return source->value(num);
	}

	bool is_over() 
	{
		return source->is_over();
	}

	/**
	 * Устанавливает фильтр для элемента структуры с номером pos.
	 * @param pos номер результата, для которого устанавливается фильтр.
	 * @param *filter фильтр для установки.
	 */
	void set_filter(int pos, sc_single_filter *filter)
	{
		assert(pos >= 0 || pos < width);

		filters[pos] = filter;

		if (filter && !source->is_over() && !filter->check_value(source->value(pos))) {
			next();
		}
	}

private:
	sc_single_filter *filters[width];
	sc_iterator *source;
	bool sink, sink_filters;
};

class sc_iterator_filter: public sc_iterator 
{
public:

	sc_iterator_filter(sc_iterator *_iter, bool _sink)
	{
		iter = _iter;
		sink = _sink;
	}

	~sc_iterator_filter()
	{
		if (sink) {
			delete iter;
		}
	}

	void begin()
	{
		if (!check_tuple()) {
			next();
		}
	}

	sc_retval next()
	{
		do {
			iter->next();
		} while(!check_tuple() && !iter->is_over());
		return iter->is_over() ? RV_ELSE_GEN : RV_OK;
	}

	bool is_over()
	{
		return iter->is_over();
	}

	sc_addr value(sc_uint num)
	{
		return iter->value(num);
	}

protected:

	sc_iterator *iter;
	bool sink;
	virtual bool check_tuple() = 0;

};

template <int width>
class sc_match_filter : public sc_iterator_filter
{
public:

	sc_match_filter(sc_iterator *iter, bool sink) : sc_iterator_filter(iter, sink)
	{
		memset(match, 0, width * width * sizeof(bool));
	}

	~sc_match_filter() {}

	void require_match(int i, int j)
	{
		match[i][j] = match[j][i] = true;
	}

protected:

	bool match[width][width]; // true means match required
	sc_addr vals[width]; // used in check_tuple internally
	// far from optimal, but quite general

	bool check_tuple()
	{
		for (int i = 0; i < width; i++) {
			if (!(vals[i] = iter->value(i))) {
				return false;
			}
		}

		for (int i = 0; i < width - 1; i++) {
			for (int j = i + 1; j < width; j++) {
				if (match[i][j] && (vals[i] != vals[j])) {
					return false;
				}
			}
		}

		return true;
	}

};

/*!@}*/

#endif // __LIBSC_SC_FILTER_H_INCLUDED__
