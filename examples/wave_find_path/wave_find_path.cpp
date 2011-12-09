
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

/**
 * @file wave_find_path.cpp
 * @brief Программа поиска минимального пути в графе с использованием sc-памяти.
 *
 * @author Constantine Uvarov
 * @author Dmitriy Lazurkin
 */

#include <libsc/libsc.h>

#include <libsc/sc_utils.h>
#include <libsc/segment_utils.h>
#include <libsc/sc_generator.h>
#include <libsc/sc_tuple.h>
#include <libsc/sc_set.h>
#include <libsc/sc_relation.h>
#include <libsc/sc_list.h>

#include <iostream>

namespace graph_theory
{
	const char* uris[] = {
		"/graph_theory/keynode/graph structure",
		"/graph_theory/keynode/vertex_",
		"/graph_theory/keynode/connective_",
		"/graph_theory/keynode/undirected graph",
		"/graph_theory/keynode/directed graph",
		"/graph_theory/keynode/route*",
		"/graph_theory/keynode/route correspondence*",
		"/graph_theory/keynode/trail*",
		"/graph_theory/keynode/simple trail*"
	};

	sc_addr keynodes[sizeof(uris) / sizeof(const char*)];

	const sc_addr& graph_structure = keynodes[0];
	const sc_addr& vertex_ = keynodes[1];
	const sc_addr& connective_ = keynodes[2];
	const sc_addr& undirected_graph = keynodes[3];
	const sc_addr& directed_graph = keynodes[4];
	const sc_addr& route = keynodes[5];
	const sc_addr& route_correspondence = keynodes[6];
	const sc_addr& trail = keynodes[7];
	const sc_addr& simple_trail = keynodes[8];
}

/**
 * @brief Генерация в sc-памяти неориентированного графа по строке @p str.
 * @param s сессия, по средствам которой будет происходить генерация.
 * @param seg сегмент, в котором будет происходить генерация.
 * @param str строка, описывающая граф. Например, "<{vertex_id1, vertex_id2,...},{{vertex_id1, vertex_id2},...}>".
 */
sc_addr gen_undirected_graph(sc_session* s, sc_segment* seg, sc_string str)
{
	enum {FIRST, RNODES, RNODE, RARCS, RARC} state = FIRST;
	sc_string buf;
	typedef std::set<sc_string> vertex_set;
	vertex_set vertexes;
	typedef std::set<std::pair<sc_string, sc_string> > edge_set;
	edge_set edges;
	std::pair<sc_string, sc_string> edge;
#define PUT_NODE() {vertexes.insert(buf); buf.erase();}
#define PUT_ARC() {edge.second = buf; buf.erase(); edges.insert(edge);}
#define PUT_ARC_BEG() {edge.first = buf; buf.erase();}
	for (size_t i = 1; i < str.size() - 1; i++) {
		switch (str[i]) {
			case '{':
				if (state == FIRST) {
					state = RNODE;
					break;
				}
				if(state == RNODES) {
					state = RARCS;
					break;
				}
				if(state == RARCS) {
					state = RARC;
					break;
				}
			break;
			case '}':
				if(state == RNODE) {
					PUT_NODE();
					state = RNODES;
					break;
				}
				if(state == RARC) {
					PUT_ARC();			
					state = RARCS;
					break;
				}
			break;
			case ',':
				if(state == RNODE) {
					PUT_NODE();
					break;
				}
				if(state == RARC) {
					PUT_ARC_BEG();
					break;
				}
			break;
			default: 
				buf += str[i];
		}
	}
#undef PUT_NODE
#undef PUT_ARC_BEG
#undef PUT_ARC
	std::map<sc_string, sc_addr> vertexes_map;
	sc_generator g(s, seg);
	GEN_PREPARE();
	
	// Генерация неориентированного графа в sc-памяти.
	g.element(graph_theory::undirected_graph);
	sc_addr graph;
	GEN_SET_VAR(graph, g, "") {

		// Генерация всех узлов графа в sc-памяти.
		for(vertex_set::iterator it = vertexes.begin(); it != vertexes.end(); ++it) {
			g.attr(graph_theory::vertex_);
			vertexes_map[*it] = g.element(it->c_str());
		}

		// Генерация всех дуг графа в sc-памяти.
		for(edge_set::iterator it = edges.begin(); it != edges.end(); ++it) {
			g.attr(graph_theory::connective_);
			GEN_SET(g, "") {
				g.element(vertexes_map[it->first]);
				g.element(vertexes_map[it->second]);
			}
		}
	}

	g.finish();

	return graph;
}

/**
 * @brief Находит начало структуры маршурта @p route_struct.
 */
sc_addr get_route_struct_begin(sc_session* s, sc_addr route_struct)
{
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_5_f_a_a_a_f, route_struct, 0, 0, 0, graph_theory::vertex_), true);
	sc_for_each (it) {
		sc_addr vertex = it->value(2);
		if (sc_rel::bin_ord_at_1(s, route_struct, vertex) == SCADDR_NIL)
			return vertex;
	}

	return SCADDR_NIL;
}

/**
 * @brief Функция вывода на консоль неориентированного графа @p graph.
 */
void print_graph(sc_session* s, sc_addr graph) 
{
	addr_set vertexes_set;
	
	// Вывод всех дуг графа.
	sc_iterator* arcs_it = s->create_iterator(sc_constraint_new(CONSTR_5_f_a_a_a_f, graph, 0, 0, 0, graph_theory::connective_), true);
	sc_for_each (arcs_it) {
		sc_addr arc = arcs_it->value(2);
		sc_iterator* vertexes_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, arc, 0, 0), true);
		
		sc_addr e1 = vertexes_it->value(2);
		vertexes_it->next();
		sc_addr e2 = vertexes_it->value(2);
		
		vertexes_set.insert(e1);
		vertexes_set.insert(e2);
		
		printf("%s -- %s\n", s->get_idtf(e1).c_str(), s->get_idtf(e2).c_str());
		
		delete vertexes_it;
	}
	
	// Вывод всех вершин, которые не имеют инцидентных дуг.
	sc_iterator *vertexes_it = s->create_iterator(sc_constraint_new(CONSTR_5_f_a_a_a_f, graph, 0, 0, 0, graph_theory::vertex_), true);
	sc_for_each (vertexes_it) {
		sc_addr vertex = vertexes_it->value(2);
		if(vertexes_set.find(vertex) == vertexes_set.end())
			printf("%s\n", s->get_idtf(vertex).c_str());
	}
}

/**
* @brief Функция вывода на консоль маршрута @p route (связка отношения). 
*/
void print_route(sc_session* s, sc_addr route) 
{
	sc_addr route_struct = sc_tup::at(s, route, N2_);
	sc_addr route_visit = sc_rel::bin_ord_at_2(s, graph_theory::route_correspondence, route);

	sc_addr cur_visit = get_route_struct_begin(s, route_struct);
	sc_addr visited_vertex = sc_rel::bin_ord_at_2(s, route_visit, cur_visit);
	printf("%s", s->get_idtf(visited_vertex).c_str());
	while (true) {
		cur_visit = sc_rel::bin_ord_at_2(s, route_struct, cur_visit);
		if (cur_visit) {
			visited_vertex = sc_rel::bin_ord_at_2(s, route_visit, cur_visit);
			printf(" -> %s", s->get_idtf(visited_vertex).c_str());
		} else {
			break;
		}
	}

	printf("\n");
}

/**
 * @brief Возвращает вершину, которая инцидентена ребру @p edge и не является вершиной @p vertex.  
 */
sc_addr get_other_vertex_incidence_edge(sc_session* s, sc_addr edge, sc_addr vertex)
{
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, edge, SC_ARC_MAIN, 0), true);
	sc_addr n = it->value(2);
	if(n == vertex) {
		it->next();
		n = it->value(2);
	}
	delete it;
	return n;
}

/**
 * @brief Добавляет посещение вершины @p vertex в маршрут @p route.
 * @return Добавленное посещение вершины.
 */
sc_addr add_vertex_visit_to_route(sc_session* s, sc_segment* seg, sc_addr route, sc_addr vertex)
{
	sc_addr route_struct = sc_tup::at(s, route, N2_);
	sc_addr route_visit = sc_rel::bin_ord_at_2(s, graph_theory::route_correspondence, route);

	sc_addr vertex_visit = sc_rel::bin_ord_at_1(s, route_visit, vertex);
	if (vertex_visit == SCADDR_NIL) {
		vertex_visit = s->create_el(seg, SC_N_CONST);
		sc_tup::add(s, seg, route_struct, graph_theory::vertex_, vertex_visit);
		sc_rel::add_ord_tuple(s, route_visit, vertex_visit, vertex);
	}

	return vertex_visit;
}

/**
 * @brief Добавляет посещение связки @p connective и инцидентные ей вершины в маршурт @p route.
 * @return добавленное посещение связки.
 */
sc_addr add_connective_visit_to_route(sc_session* s, sc_segment* seg, sc_addr route, sc_addr connective, sc_addr from_vertex, sc_addr to_vertex)
{
	sc_addr route_struct = sc_tup::at(s, route, N2_);
	sc_addr route_visit = sc_rel::bin_ord_at_2(s, graph_theory::route_correspondence, route);

	sc_addr edge_visit; // Ориентированная связка, которая обозначает в структуре маршрута посещение ребра графа, 
						// на котором задан мартшрут.


	// Добавим посещение вершины @p from_vertex в маршрут.
	sc_addr from_vertex_visit = add_vertex_visit_to_route(s, seg, route, from_vertex);

	GEN_PREPARE();
	sc_generator g(s, seg);

	GEN_SET(g, route_struct) {
		g.attr(graph_theory::connective_);
		GEN_SET_VAR(edge_visit, g, "") {
			sc_rel::add_ord_tuple(s, route_visit, edge_visit, connective);

			g.attr(N1_);
			g.element(from_vertex_visit);

			g.attr(N2_);
			sc_addr to_vertex_visit = sc_rel::bin_ord_at_1(s, route_visit, to_vertex);
			g.element(to_vertex_visit);
		}
	}

	g.finish();

	return edge_visit;
}

/**
 * @brief Ищет для @p vertex любую смежную вершину из волны @p prev_wave.
 */
sc_addr find_any_edge(sc_session *s, sc_addr graph, sc_addr vertex, sc_addr prev_wave)
{
	sc_addr edge;
	sc_iterator *it = s->create_iterator(sc_constraint_new(CONSTR_3l2_f_a_a_a_f, 
		graph, SC_A_CONST|SC_POS, SC_N_CONST, SC_A_CONST|SC_POS, vertex), true);
	sc_for_each (it) {
		edge = it->value(2);
		sc_addr other_vertex = get_other_vertex_incidence_edge(s, edge, vertex);
		
		if(sc_set::is_in(s, other_vertex, prev_wave))
			break;
	}
	return edge;
}

/**
 * @brief Создает следующую волну для волны @p wave, графа @p graph 
 * и множества непросмотренных вершин @p not_checked_vertexes.
 */
sc_addr create_wave(sc_session *s, sc_segment *seg, sc_addr graph, sc_addr wave, sc_addr not_checked_vertexes)
{
	sc_addr new_wave = s->create_el(seg, SC_N_CONST);
	sc_iterator *it_vertex = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, wave, SC_A_CONST|SC_POS, 0), true);
	sc_for_each (it_vertex) {
		sc_addr vertex = it_vertex->value(2);
		
		sc_iterator *it_edge = s->create_iterator(sc_constraint_new(CONSTR_3l2_f_a_a_a_f,
			graph, SC_A_CONST|SC_POS, 0, SC_A_CONST|SC_POS, vertex), true);
		sc_for_each (it_edge) {
			sc_addr edge = it_edge->value(2);
			sc_addr other_vertex = get_other_vertex_incidence_edge(s, edge, vertex);

			if (sc_set::exclude_from(s, other_vertex, not_checked_vertexes))
				sc_set::include_in(s, other_vertex, new_wave);
		}
	}

	return new_wave;
}

#include <libsc/sc_print_utils.h>

/**
 * @brief Находит граф минимального пути между вершинами @p beg_vertex и @p end_vertex графа @p graph.
 * @return знак графа минимального пути или 0, если пути не существует.
 */
sc_addr find_min_path(sc_session* s, sc_segment* seg, sc_addr graph, sc_addr beg_vertex, sc_addr end_vertex)
{
	// Создаем множество всех вершин графа, оно будет множеством всех непроверенных вершин.
	sc_addr not_checked_vertexes = s->create_el(seg, SC_N_CONST);
	sc_iterator* it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, 
		graph, SC_A_CONST|SC_POS, 0, SC_A_CONST|SC_POS, graph_theory::vertex_), true);
	sc_for_each (it) {
		sc_addr vertex = it->value(2);
		// Не добавляем вершину начала пути в множество непросмотренных вершин.
		if (vertex != beg_vertex)
			sc_set::include_in(s, it->value(2), not_checked_vertexes);
	}

	// Создаем первую волну и включаем в нее только начальную вершину beg_vertex пути .
	sc_addr new_wave = s->create_el(seg, SC_N_CONST);
	sc_set::include_in(s, beg_vertex, new_wave);

	// Создаем начало списока волн.
	sc_addr waves_list_head = sc_list::create(s, seg, new_wave);
	sc_addr waves_list_tail = waves_list_head;

	do {
		// Находим новую волну для текущей волны.
		new_wave = create_wave(s, seg, graph, new_wave, not_checked_vertexes);

		// Добавляем новую волну в список волн.
		sc_addr waves_list_curr = sc_list::create(s, seg, new_wave);
		sc_list::set_next(s, waves_list_tail, waves_list_curr);

		waves_list_tail = waves_list_curr;
	} while (!sc_set::is_in(s, end_vertex, new_wave));

	// Теперь займемся построением пути (простой цепи).

	sc_addr route_struct = s->create_el(seg, SC_N_CONST);
	sc_addr route_conn = sc_rel::add_ord_tuple(s, graph_theory::simple_trail, graph, route_struct);
	
	sc_addr route_visit = s->create_el(seg, SC_N_CONST);
	sc_rel::add_ord_tuple(s, graph_theory::route_correspondence, route_conn, route_visit);
	
	// Добавим в путь посещение начальной вершины.
	sc_addr beg_vertex_visit = add_vertex_visit_to_route(s, seg, route_conn, beg_vertex);

	// Добавим в путь посещение конечной вершины. 
	sc_addr end_vertex_visit = add_vertex_visit_to_route(s, seg, route_conn, end_vertex);

	sc_addr curr_vertex = end_vertex;

	// Строим из списка волн маршрут, проходя по этому списку в обратном направлении.
	sc_list::reverse_iterator list_it(s, waves_list_tail), list_end;
	for (++list_it; list_it != list_end; ++list_it) {
		sc_addr curr_wave = *list_it;

		sc_addr edge = find_any_edge(s, graph, curr_vertex, curr_wave);
		
		// Получаем предыдущую вершину в пути.
		sc_addr prev_vertex = get_other_vertex_incidence_edge(s, edge, curr_vertex); 

		// Добавляем посещение ребра @p edge в путь.
		add_connective_visit_to_route(s, seg, route_conn, edge, prev_vertex, curr_vertex);
		curr_vertex = prev_vertex;
	}

	sc_list::erase(s, waves_list_head);

	return route_conn;
}

int main(int argn, char** argv) 
{
	// Инициализируем libsc.
	sc_session* system = libsc_init();
	// Инициализируем стандартные ключевые узлы.
	pm_keynodes_init(system);

	 // Создаем ключевые узлы.
	sc_segment *graph_keynodes_seg = create_segment_full_path(system, "/graph_theory/keynode");
	for (int i = 0; i < sizeof(graph_theory::keynodes) / sizeof(sc_addr); i++) {
		graph_theory::keynodes[i] = create_el_by_full_uri(system, graph_theory::uris[i], SC_N_CONST);
	}

	// Создаем не системную сессию для работы.
	sc_session* session = libsc_login();

	// Отрываем сегмент ключевых узлов, чтобы иметь к ним доступ.
	session->open_segment("/proc/keynode");

	 // Отрываем сегмент ключевых узлов программы, чтобы иметь к ним доступ.
	session->open_segment(graph_keynodes_seg->get_full_uri());

	// Для работы создаем временный сегмент /tmp/wave_find_path. 
	sc_segment* tmp_seg = create_unique_segment(session, "/tmp/wave_find_path");
	
	sc_addr graph = gen_undirected_graph(session, tmp_seg, "<{A,B,C,D,E,F,G},{{A,B},{C,A},{E,C},{C,D},{E,B},{E,F}}>");
	printf("Input graph: \n");
	print_graph(session, graph);

	sc_addr beg = session->find_by_idtf("A", tmp_seg);
	sc_addr end = session->find_by_idtf("F", tmp_seg);
	sc_addr result = find_min_path(session, tmp_seg, graph, beg, end);
	printf("Path: \n");
	print_route(session, result);
	
	session->unlink(tmp_seg->get_full_uri());
	session->close();

	return 0;
}
