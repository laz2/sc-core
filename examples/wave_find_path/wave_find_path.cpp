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

/// @file wave_find_path.cpp
///
/// @brief Программа поиска одного из минимальных путей в неориентированном графе
///        с использованием sc-памяти.
///
/// @author Лазуркин Дмитрий Александрович
/// @author Уваров Константин Александрович
///

#include <assert.h>

#include <libsc.h>

#include <pm_keynodes.h>
#include <pm_utils.h>
#include <segment_utils.h>
#include <sc_generator.h>
#include <sc_tuple.h>
#include <sc_set.h>
#include <sc_relation.h>
#include <sc_list.h>

#include <iostream>

// Определение ключевых узлов в пространсте имен позволяет разграничить предметные области.
// В данном случае у нас только одна предметная область, но их может быть и больше.
namespace graph_theory
{
	// В массиве "uris" перечислены полные пути (URI) ключевых узлов.
	// Договоримся, что ключевые узлы теории графов будут
	// находиться в сегменте "/graph_theory/keynode",
	// а все узлы в качестве идентификаторов будут иметь 
	// свои английские названия.
	const char* uris[] = {
		"/graph_theory/keynode/graph structure",             // графовая структура
		"/graph_theory/keynode/vertex_",                     // вершина_
		"/graph_theory/keynode/connective_",                 // связка_

		"/graph_theory/keynode/directed graph structure",    // графовая структура с ориентированными связками
		"/graph_theory/keynode/directed connective_",        // ориентированная связка_
		
		"/graph_theory/keynode/undirected graph structure",  // графовая структура с неориентированными связками
		"/graph_theory/keynode/undirected connective_",      // неориентированная связка_

		"/graph_theory/keynode/hypergraph",                  // гиперграф
		"/graph_theory/keynode/hyperconnective_",            // гиперсвязка_
		"/graph_theory/keynode/hyperedge_",                  // гиперребро_
		"/graph_theory/keynode/hyperarc_",                   // гипердуга_

		"/graph_theory/keynode/pseudograph",                 // псевдограф
		"/graph_theory/keynode/binary connective_",          // бинарная связка_
		"/graph_theory/keynode/edge_",                       // ребро_
		"/graph_theory/keynode/arc_",                        // дуга_
		"/graph_theory/keynode/loop_",                       // петля_

		"/graph_theory/keynode/multigraph",                  // мультиграф

		"/graph_theory/keynode/graph",                       // граф

		"/graph_theory/keynode/undirected graph",            // неориентированный граф

		"/graph_theory/keynode/directed graph",              // ориентированный граф

		"/graph_theory/keynode/route*",                      // маршрут*

		"/graph_theory/keynode/trail*",                      // цепь*

		"/graph_theory/keynode/simple trail*"                // простая цепь*
	};

	// В данном массиве будут храниться sc-адреса созданных ключевых узлов.
	sc_addr keynodes[sizeof(uris) / sizeof(const char*)];

	// В этом sc-сегменте будут созданы ключевые узлы теории графов.
	sc_segment *segment;

	// С помощью ссылок C++ задан удобный способ обращения
	// к элементам массива ключевых узлов "keynodes".
	// Каждая из следующих констант ссылается на sc-адрес
	// ключевого узла с соответствующем именем.
	const sc_addr &graph_structure               = keynodes[0];
	const sc_addr &vertex_                       = keynodes[1];
	const sc_addr &connective_                   = keynodes[2];

	const sc_addr &directed_graph_structure      = keynodes[3];
	const sc_addr &directed_connective_          = keynodes[4];

	const sc_addr &undirected_graph_structure    = keynodes[5];
	const sc_addr &undirected_connective_        = keynodes[6];

	const sc_addr &hypergraph                    = keynodes[7];
	const sc_addr &hyperconnective_              = keynodes[8];
	const sc_addr &hyperedge_                    = keynodes[9];
	const sc_addr &hyperarc_                     = keynodes[10];

	const sc_addr &pseudograph                   = keynodes[11];
	const sc_addr &binary_connective_            = keynodes[12];
	const sc_addr &edge_                         = keynodes[13];
	const sc_addr &arc_                          = keynodes[14];
	const sc_addr &loop_                         = keynodes[15];

	const sc_addr &multigraph                    = keynodes[16];

	const sc_addr &graph                         = keynodes[17];

	const sc_addr &undirected_graph              = keynodes[18];
	
	const sc_addr &directed_graph                = keynodes[19];
	
	const sc_addr &route                         = keynodes[20];
	
	const sc_addr &trail                         = keynodes[21];
	
	const sc_addr &simple_trail                  = keynodes[22];
}

/// @brief Генерирует в sc-памяти неориентированный граф по строке @p str.
///
/// Входная строка @p str должна иметь следующую структуру:
/// "<{vertex_id1, vertex_id2,...},{{vertex_id1, vertex_id2},...}>"
/// Как видно из строки, граф задается классическим способом через множество вершин и множество ребер.
/// Например, "<{A,B,C,D,E,F,G},{{A,B},{C,A},{E,C},{C,D},{E,B},{E,F}}>".
///
/// @param s   sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param seg sc-сегмент, в котором будет сгенерирован неориентированный граф.
/// @param str строка, описывающая неориентированный граф.
///
/// @return сгенерированный неориентированный граф или 0, если входная строка имеет неправильную структуру.
///
sc_addr gen_undirected_graph(sc_session *s, sc_segment *seg, const sc_string &str);

/// @brief Выводит на консоль неориентированный граф @p graph.
///
/// @param s     sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param graph неориентированный граф для вывода на консоль.
///
void print_graph(sc_session* s, sc_addr graph);

/// @brief Выводит на консоль маршрута @p route.
///
/// @param s     sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param route связка отношения "маршрут*".
///
void print_route(sc_session* s, sc_addr route);

/// @brief Находит граф минимального пути между вершинами @p beg_vertex и @p end_vertex графа @p graph.
///
/// @param s          sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param seg        sc-сегмент, в котором будет сгенерирован найденный минимальный путь.
/// @param graph      граф, в котором будет производиться поиск минимального пути.
/// @param beg_vertex начальная вершина в графе @p graph для поиска минимального пути.
/// @param end_vertex конечная вершина в графе @p graph для поиска минимального пути.
///
/// @return граф-структура минимального пути или 0, если пути не существует.
///
sc_addr find_min_path(sc_session *s, sc_segment *seg, sc_addr graph, sc_addr beg_vertex, sc_addr end_vertex);

/// @brief Подготавливает запуск и запускает тестовый пример для алгоритма поиска минимального пути.
///
/// @param s          sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param number     порядковый номер тестового примера.
/// @param graph_str  строка, задающая тестовый граф для функции #gen_undirected_graph.
/// @param beg_name   имя начальной вершина в графе @p graph_str для поиска минимального пути.
/// @param end_name   имя конечной вершина в графе @p graph_str для поиска минимального пути.
///
/// @see gen_undirected_graph
/// @see find_min_path
///
void run_testcase(sc_session *s, int number, const sc_string &graph_str, const sc_string &beg_name, const sc_string &end_name);

/// @brief Создает ключевые узлы базы знаний по теории графов.
///
/// @note Ключевые узлы создаются в sc-сегменте с URI "/graph_theory/keynode".
///
/// @param s sc-сессия, при помощи которой будет производиться работа с sc-памятью.
///
void graph_keynodes_init(sc_session *s)
{
	// Сперва создадим для них сегмент "/graph_theory/keynode".
	//
	graph_theory::segment = create_segment_full_path(s, "/graph_theory/keynode");

	// Пробежимся по массиву полных URI ключевых узлов, для каждого URI создадим соответствующий ему узел.
	// Создаваемые узлы будем заносить в массив graph_theory::keynodes. 
	//
	for (int i = 0; i < sizeof(graph_theory::keynodes) / sizeof(sc_addr); ++i)
		graph_theory::keynodes[i] = create_el_by_full_uri(s, graph_theory::uris[i], SC_N_CONST);
}

int main(int argc, char **argv)
{
	// Инициализируем sc-память при помощи функции libsc_init().
	// Она вернет системную sc-сессию.
	//
	sc_session *system = libsc_init();

	// Кроме ключевых узлов нашей предментной области (теории графов)
	// есть еще системные ключевые узлы, которые необходимо также создать.
	// Для этого воспользуемся функцией "pm_keynodes_init". 
	//
	pm_keynodes_init(system);

	// Теперь пришла пора создать наши ключевые узлы.
	//
	graph_keynodes_init(system);

	// Создаем не системную сессию для работы.
	//
	sc_session* session = libsc_login();

	// Отрываем сегмент системных ключевых узлов для пользовательской сессии,
	// чтобы иметь к ним доступ. Метод sc_session::open_segment открывает sc-сегмент
	// по полному URI для данной sc-сессии.
	//
	session->open_segment("/proc/keynode");

	// Отрываем сегмент ключевых узлов теории графов для пользовательской сессии,
	// чтобы иметь к ним доступ. Метод get_full_uri класса sc_segment
	// возвращает полный путь к сегменту.
	//
	session->open_segment(graph_theory::segment->get_full_uri());

	// Запустим тестовые примеры алгоритма поиска одного из минимальных путей.
	//
	run_testcase(session, 1, "<{A,B,C},{{A,B},{B,C}}>", "A", "C");
	run_testcase(session, 2, "<{A,B,C,D,F},{{A,B},{A,C},{C,D},{B,D},{D,F}}>", "A", "F");
	run_testcase(session, 3, "<{A,B,C,D,F,K},{{A,B},{A,C},{C,D},{B,D},{D,F}}>", "A", "K");
	run_testcase(session, 4, "<{V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11},{{V1,V2},{V2,V3},"
							 "{V2,V4},{V3,V4},{V3,V5},{V4,V5},{V4,V6},{V5,V7},{V6,V7},"
							 "{V6,V8},{V6,V9},{V8,V9},{V8,V10},{V9,V10},{V9,V11},{V10,V11}}>", "V5", "V11");
	run_testcase(session, 5, "<{V1,V2,V3,V4,V5,V6,V7,V8,V9},{{V1,V2},{V2,V3},"
							 "{V2,V4},{V3,V4},{V3,V5},{V4,V5},{V6,V7},{V7,V9},"
							 "{V6,V8},{V6,V9},{V8,V9}}>", "V1", "V9");

	// Закроем пользовательскую sc-сессию.
	//
	session->close();

	// Деинициализируем sc-память.
	//
	libsc_deinit();

	return 0;
}

void run_testcase(sc_session *s, int number, const sc_string &graph_str, const sc_string &beg_name, const sc_string &end_name)
{
	std::cout << "[Testcase " << number << "]\n";

	// Для работы создаем временный сегмент /tmp/wave_find_path.
	//
	sc_segment* tmp_seg = create_unique_segment(s, "/tmp/wave_find_path");

	// Генерируем текстовый граф в sc-памяти.
	//
	sc_addr graph = gen_undirected_graph(s, tmp_seg, graph_str);

	// Распечатаем граф на консоль
	//
	std::cout << "Graph string: \n" << graph_str << '\n';

	std::cout << "Graph from memory: " << '\n';
	print_graph(s, graph);

	// Найдем начальную вершину для тестового поиска минимального пути.
	//
	sc_addr beg = s->find_by_idtf(beg_name, tmp_seg);
	assert(beg);

	// Найдем конечную вершину для тестового поиска миниального пути.
	//
	sc_addr end = s->find_by_idtf(end_name, tmp_seg);
	assert(end);

	std::cout << "Find minimal path from '" << beg_name << "' to '" << end_name << "'\n";

	// Найдем минимальный пути между начальной и конечной вершинами.
	//
	sc_addr result = find_min_path(s, tmp_seg, graph, beg, end);

	// Распечатаем найденный путь на консоль
	std::cout << "Path";

	if (result) {
		std::cout << ":\n";
		print_route(s, result);
	} else {
		std::cout << " isn't exist.\n";
	}

	// Удалим временный sc-сегмент.
	//
	s->unlink(tmp_seg->get_full_uri());

	std::cout << std::endl;
}

sc_addr gen_undirected_graph(sc_session *s, sc_segment *seg, const sc_string &str)
{
	assert(s);
	assert(seg);

	//
	// Разбор входной строки str, описывающей создаваемый граф.
	//
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

	//
	// Начинаем генерацию графа в sc-памяти
	//

	// Отображение, которое позволяет переходить быстро от имени вершины к sc-адресу,
	// который обозначает данную вершину графа в sc-памяти
	std::map<sc_string, sc_addr> vertexes_map;

	sc_addr graph = s->create_el(seg, SC_N_CONST); // Переменная будет хранить sc-адрес знака сгенерированного графа

	s->gen3_f_a_f(graph_theory::undirected_graph, 0, seg, SC_A_CONST|SC_POS, graph);

	// Генерация всех узлов графа в sc-памяти.
	// Пробегаем по всем вершинам графа из STL-множества vertexes
	// и добавляем их в генерируемый граф с атрибутом "vertex_".
	for(vertex_set::iterator it = vertexes.begin(); it != vertexes.end(); ++it) {
		sc_addr vertex = s->create_el(seg, SC_N_CONST);
		s->set_idtf(vertex, *it);

		s->gen5_f_a_f_a_f(graph, 0, seg, SC_A_CONST|SC_POS, vertex, 0, seg, SC_A_CONST|SC_POS, graph_theory::vertex_);

		vertexes_map[*it] = vertex;
	}

	// Генерация всех дуг графа в sc-памяти.
	// Пробегаем по всем ребрам графа из STL-множества edges
	// и добавляем их в генерируемый граф с атрибутом "edge_".
	for(edge_set::iterator it = edges.begin(); it != edges.end(); ++it) {
		sc_addr edge = s->create_el(seg, SC_N_CONST);

		s->gen5_f_a_f_a_f(graph, 0, seg, SC_A_CONST|SC_POS, edge, 0, seg, SC_A_CONST|SC_POS, graph_theory::edge_);

		s->gen3_f_a_f(edge, 0, seg, SC_A_CONST|SC_POS, vertexes_map[it->first]);
		s->gen3_f_a_f(edge, 0, seg, SC_A_CONST|SC_POS, vertexes_map[it->second]);
	}

	// Возвращаем из функции sc-адрес сгенерированного графа
	return graph;
}

void print_graph(sc_session *s, sc_addr graph)
{
	assert(s);
	assert(graph);

	std::set<sc_addr> vertexes_set;

	// Создаем итератор по всем ребрам.
	//
	sc_iterator* arcs_it = s->create_iterator(
		sc_constraint_new(
			CONSTR_5_f_a_a_a_f,
			graph,
			SC_A_CONST|SC_POS,
			0,
			SC_A_CONST|SC_POS,
			graph_theory::edge_
	), true);

	// Запускаем цикл по всем результатам поиска
	//
	sc_for_each (arcs_it) {
		// Третьим элементом найденной пяти элементной конструкции будет знак ребра графа
		// Индекция ведется с 0 как в массивах
		sc_addr arc = arcs_it->value(2);

		// Для каждого ребра нам необходимо вывести инцидентные ему вершины,
		// поэтому создаем итератор по sc-элементам, которые включает в себя sc-множество, обозначающее найденное ребро графа 
		sc_iterator* vertexes_it = s->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, arc, 0, 0), true);

		// Получаем инцидентные вершины
		// Так как у нас классический граф, то мы всегда знаем что множество ребра всегда двумощно.
		sc_addr e1 = vertexes_it->value(2);
		vertexes_it->next();
		sc_addr e2 = vertexes_it->value(2);

		// Запоминаем вершины, которые уже выводились
		vertexes_set.insert(e1);
		vertexes_set.insert(e2);

		// Выводим ребро на консоль
		std::cout << s->get_idtf(e1) << " -- " << s->get_idtf(e2) << '\n';

		// Освобождаем память, выделенную под итератор vertexes_it
		delete vertexes_it;
	}
	// sc_for_each автоматический освободит память, выделенную под arcs_it

	// Вывод всех вершин, которые не имеют инцидентных дуг.
	sc_iterator *vertexes_it = s->create_iterator(sc_constraint_new(CONSTR_5_f_a_a_a_f, graph, 0, 0, 0, graph_theory::vertex_), true);
	sc_for_each (vertexes_it) {
		sc_addr vertex = vertexes_it->value(2);

		// Проверим, входит ли sc-адерс вершин в множество vertexes_set
		if(vertexes_set.find(vertex) == vertexes_set.end())
			std::cout << s->get_idtf(vertex) << '\n';
	}
}

/// @brief Находит начало структуры маршурта @p route_struct.
///
/// @param s            sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param route_struct ориентированный граф структуры маршрута.
///
/// @return начальную вершину в структуре маршрута, если не найдена - 0.
///
sc_addr get_route_struct_begin(sc_session *s, sc_addr route_struct)
{
	// Начальной считается вершина, в которую нет входящих связок.
	// Переберем все вершины и проверим их на это свойство.
	//

	sc_iterator *it = s->create_iterator(
		sc_constraint_new(
			CONSTR_5_f_a_a_a_f,
			route_struct,
			SC_A_CONST|SC_POS,
			0,
			SC_A_CONST|SC_POS,
			graph_theory::vertex_
	), true);

	sc_for_each (it) {
		sc_addr vertex = it->value(2);

		// Ориентированный граф структуры маршрута можно рассматривать как
		// бинарное ориентированное отношение, которое связывает вершины.
		// Это позволяет нам использовать для проверки метод sc_rel::bin_ord_at_1.
		//
		if (!sc_rel::bin_ord_at_1(s, route_struct, vertex))
			return vertex;
	}

	return 0;
}

/// @brief Возвращает структуру маршрута для связки отношения маршрут*.
///
/// @note Структура маршрута - это компонент с атрибутом 1_.
///
/// @param s     sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param route связка отношения маршрут*.
///
/// @return ориентированный граф структуры маршрута.
///
inline sc_addr get_route_struct(sc_session *s, sc_addr route)
{
	return sc_tup::at(s, route, N1_);
}

/// @brief Возвращает отношение посещения для связки отношения маршрут*.
///
/// @note Отношение посещения - это компонент с атрибутом 3_.
///
/// @param s     sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param route связка отношения маршрут*.
///
/// @return бинарное отношение посещения.
///
inline sc_addr get_route_visit(sc_session *s, sc_addr route)
{
	return sc_tup::at(s, route, N3_);
}

/// @brief Выводит на консоль маршрут.
///
/// @param s     sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param route связка отношения маршрут*.
///
void print_route(sc_session *s, sc_addr route)
{
	sc_addr route_struct = get_route_struct(s, route);
	sc_addr route_visit  = get_route_visit(s, route);

	sc_addr cur_visit      = get_route_struct_begin(s, route_struct);
	sc_addr visited_vertex = sc_rel::bin_ord_at_2(s, route_visit, cur_visit);

	std::cout << s->get_idtf(visited_vertex);
	while (true) {
		cur_visit = sc_rel::bin_ord_at_2(s, route_struct, cur_visit);

		if (cur_visit) {
			visited_vertex = sc_rel::bin_ord_at_2(s, route_visit, cur_visit);
			std::cout << " -> " << s->get_idtf(visited_vertex);
		} else {
			break;
		}
	}

	std::cout << '\n';
}

/// @brief Возвращает вершину, которая инцидентена ребру @p edge и не является вершиной @p vertex.
///
/// @param s      sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param edge   ребро, которому инцидентна вершина @p vertex и должна быть инцидентна найденная вершина.
/// @param vertex одна из вершин, инцидентных ребру @p edge.
///
/// @return вершина, инцидентная ребру @p edge.
///
sc_addr get_other_vertex_incidence_edge(sc_session *s, sc_addr edge, sc_addr vertex)
{
	sc_iterator *it = s->create_iterator(
		sc_constraint_new(
			CONSTR_3_f_a_a,
			edge,
			SC_A_CONST|SC_POS,
			0
	), true);

	sc_addr other_vertex = it->value(2);
	if(other_vertex == vertex) {
		it->next();
		other_vertex = it->value(2);
	}
	delete it;

	return other_vertex;
}

/// @brief Добавляет посещение вершины @p vertex в маршрут @p route.
///
/// @note Все sc-элементы генерируются в sc-сегменте связки отношения маршрут* @p route.
///
/// @param s      sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param route  связка отношения маршрут*.
/// @param vertex вершина, для которой нужно добавить посещение в маршрут.
///
/// @return посещение вершины (вершина в ориентированном графе структуры маршрута).
///
sc_addr add_vertex_visit_to_route(sc_session *s, sc_addr route, sc_addr vertex)
{
	sc_addr route_struct = get_route_struct(s, route);
	sc_addr route_visit  = get_route_visit(s, route);

	sc_addr vertex_visit = sc_rel::bin_ord_at_1(s, route_visit, vertex);
	if (vertex_visit == 0) {
		vertex_visit = s->create_el(route->seg, SC_N_CONST);
		sc_tup::add(s, route->seg, route_struct, graph_theory::vertex_, vertex_visit);
		sc_rel::add_ord_tuple(s, route_visit, vertex_visit, vertex);
	}

	return vertex_visit;
}

/// @brief Добавляет посещение ребра @p edge и инцидентные ей вершины в маршрут @p route.
///
/// @note Все sc-элементы генерируются в sc-сегменте связки отношения маршрут* @p route.
///
/// @param s           sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param route       связка отношения маршрут*.
/// @param edge        ребро, посещение которого надо добавить в маршрут.
/// @param from_vertex вершина, из которой выходит при посещении.
/// @param to_vertex   вершина, в которую входим при посещении.
///
/// @return посещение ребра (дуга в ориентированном графе структуры маршрута).
///
sc_addr add_edge_visit_to_route(sc_session *s, sc_addr route, sc_addr edge, sc_addr from_vertex, sc_addr to_vertex)
{
	sc_addr route_struct = get_route_struct(s, route);
	sc_addr route_visit  = get_route_visit(s, route);

	// Добавим посещение вершины @p from_vertex в маршрут.
	sc_addr from_vertex_visit = add_vertex_visit_to_route(s, route, from_vertex);
	
	// Ориентированная связка, которая обозначает в структуре маршрута посещение ребра графа, на котором задан мартшрут.
	//
	sc_addr edge_visit = s->create_el(route->seg, SC_N_CONST);

	// Добавим @p edge_visit как дугу в структуру маршрута.
	//
	sc_tup::add(s, route_struct, graph_theory::arc_, edge_visit);

	// Укажем, что @p edge_visit является посещением @p edge.
	//
	sc_rel::add_ord_tuple(s, route_visit, edge_visit, edge);

	// Укажем, что дуга @p edge_visit выходит из @p from_vertex_visit.
	//
	sc_tup::add(s, edge_visit, N1_, from_vertex_visit);

	// Найдем посещение для вершины @p to_vertex.
	//
	sc_addr to_vertex_visit = sc_rel::bin_ord_at_1(s, route_visit, to_vertex);

	// Укажем, что дуга @p edge_visit входит в @p to_vertex_visit.
	//
	sc_tup::add(s, edge_visit, N2_, to_vertex_visit);

	return edge_visit;
}

/// @brief Ищет для @p vertex любую смежную вершину из волны @p prev_wave в графе @p graph.
///
/// @param s         sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param graph     обрабатываемый неориентированный граф.
/// @param vertex    вершина, для которой ищется смежная вершина из @p prev_wave.
/// @param prev_wave предыдущая волна, из которой ищется смежная вершина.
///
/// @return ребро инцидентное @p vertex и найденной вершине.
///
sc_addr find_any_edge(sc_session *s, sc_addr graph, sc_addr vertex, sc_addr prev_wave)
{
	sc_addr edge = 0;

	sc_iterator *it = s->create_iterator(
		sc_constraint_new(
			CONSTR_3l2_f_a_a_a_f,
			graph,
			SC_A_CONST|SC_POS,
			SC_N_CONST,
			SC_A_CONST|SC_POS,
			vertex
	), true);

	sc_for_each (it) {
		edge = it->value(2);
		sc_addr other_vertex = get_other_vertex_incidence_edge(s, edge, vertex);
		
		if(sc_set::is_in(s, other_vertex, prev_wave))
			break;
	}

	return edge;
}

/// @brief Удаляет все волны из списка и список волн.
///
/// @param s               sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param waves_list_head sc-адрес головного элемента удаляемого списка волн.
///
void erase_waves_list(sc_session *s, sc_addr waves_list_head)
{
	sc_addr waves_list_curr = waves_list_head;
	while (waves_list_curr) {
		// Удаляем волну.
		//
		sc_addr curr_wave = sc_list::get_value(s, waves_list_curr);
		s->erase_el(curr_wave);

		// Удаляем текущий элемент списка.
		//
		sc_addr waves_list_next = sc_list::get_next(s, waves_list_curr);
		s->erase_el(waves_list_curr);

		waves_list_curr = waves_list_next;
	}
}

/// @brief Создает следующую волну из непроверенных вершин,
///        смежных с вершинами из волны @p wave неориентированного графа @p graph.
///
/// @param s                    sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param seg                  sc-сегмент, в котором происходит генерация новой волны.
/// @param graph                обрабатываемый неориентированный граф.
/// @param wave                 текущая волна.
/// @param not_checked_vertexes множество непроверенных вершин.
///
/// @return созданная новая волна.
///
sc_addr create_wave(sc_session *s, sc_segment *seg, sc_addr graph, sc_addr wave, sc_addr not_checked_vertexes)
{
	sc_addr new_wave = s->create_el(seg, SC_N_CONST);

	// Перебор всех вершин из волны wave.
	//
	sc_iterator *it_vertex = s->create_iterator(
		sc_constraint_new(
			CONSTR_3_f_a_a,
			wave,
			SC_A_CONST|SC_POS,
			0
		), true);

	sc_for_each (it_vertex) {
		sc_addr vertex = it_vertex->value(2);

		// Перебор всех ребер, которые инцидентны vertex.
		//
		sc_iterator *it_edge = s->create_iterator(
			sc_constraint_new(
				CONSTR_3l2_f_a_a_a_f,
				graph,
				SC_A_CONST|SC_POS,
				0,
				SC_A_CONST|SC_POS,
				vertex
			), true);

		sc_for_each (it_edge) {
			sc_addr edge = it_edge->value(2);
			sc_addr other_vertex = get_other_vertex_incidence_edge(s, edge, vertex);

			if (sc_set::exclude_from(s, other_vertex, not_checked_vertexes))
				sc_set::include_in(s, other_vertex, new_wave);
		}
	}

	return new_wave;
}

/// @brief Находит один из минимальных путей в графе @p graph
///        от вершины @p beg_vertex до вершины @p end_vertex.
///
/// Волновой алгоритм поиска одного из минимальных путей:
/// 1. Добавить все вершины графа (кроме начальной вершины пути) в множество непроверенных вершин.
///
/// 2. Создать начальную волну и добавить в нее начальную вершину пути.
///
/// 3. Начальная волна - новая волна. Новой волной будем называть последнюю созданную волну.
///
/// 4. Сформировать следующую волну для новой волны. В нее попадет та вершина,
///    которая является смежной вершине из новой волны и присутствует во множестве непроверенных вершин.
///    Если вершина попала в формируемую волну, то ее надо исключить из множества непроверенных вершин.
///    Созданную волну установим как следующую для новой волны, и после этого созданную волну будем считать новой волной.
///
/// 5. Если новая волна пуста, то значит между вершинами не существует пути.
///    Завершить алгоритм.
///
/// 6. Если в текущей волне есть конечная вершина, то перейти к пункту 7, иначе к пункту 4.
///
/// 7. Сформировать один из минимальных путей, проходя в обратном порядке по списку волн.
///
/// @param s          sc-сессия, при помощи которой будет производиться работа с sc-памятью.
/// @param seg        sc-сегмент, в котором происходит работа алгоритма.
/// @param graph      неориентированный граф, в котором будет находится минимальный путь.
/// @param beg_vertex начальная вершина пути.
/// @param end_vertex конечная вершина пути.
///
/// @return связка отношения "простая цепь*" или 0, если минимальный путь не найден.
///
sc_addr find_min_path(sc_session* s, sc_segment* seg, sc_addr graph, sc_addr beg_vertex, sc_addr end_vertex)
{
	// 1. Добавить все вершины графа (кроме начальной вершины пути) в множество непроверенных вершин.
	//
	sc_addr not_checked_vertexes = s->create_el(seg, SC_N_CONST); // множество непроверенных вершин

	// Итератор по вершинам графа
	//
	sc_iterator *it = s->create_iterator(
		sc_constraint_new(
			CONSTR_3_f_a_a, 
			graph,
			SC_A_CONST|SC_POS,
			0,
			SC_A_CONST|SC_POS,
			graph_theory::vertex_
		), true);

	sc_for_each (it) {
		sc_addr vertex = it->value(2);

		// Не добавляем вершину начала пути в множество непросмотренных вершин.
		//
		if (vertex != beg_vertex)
			sc_set::include_in(s, it->value(2), not_checked_vertexes);
	}

	// 2. Создать начальную волну и добавить в нее начальную вершину пути.
	// 3. Начальная волна - новая волна.
	//
	sc_addr new_wave = s->create_el(seg, SC_N_CONST);
	sc_set::include_in(s, beg_vertex, new_wave);

	// Создаем начало списка волн.
	sc_addr waves_list_head = sc_list::create(s, seg, new_wave);
	sc_addr waves_list_tail = waves_list_head;

	do {
		// 4. Сформировать следующую волну для новой волны.
		//    Установить созданную волну как следующую для новой волны.
		//    Созданная волна - новая волна.
		//
		new_wave = create_wave(s, seg, graph, new_wave, not_checked_vertexes);

		sc_addr waves_list_curr = sc_list::create(s, seg, new_wave);
		sc_list::set_next(s, waves_list_tail, waves_list_curr);

		waves_list_tail = waves_list_curr;

		// 5. Если новая волна пуста, то значит между вершинами не существует пути.
		//
		if (sc_set::is_empty(s, new_wave)) {
			// Очищаем память и завершаем алгоритм.
			//
			erase_waves_list(s, waves_list_head);
			s->erase_el(not_checked_vertexes);
			return 0;
		}

		// 6. Если в текущей волне есть конечная вершина, то перейти к пункту 7, иначе к пункту 4.
		//
	} while (!sc_set::is_in(s, end_vertex, new_wave));

	// Подчистим память...
	//
	s->erase_el(not_checked_vertexes);

	// 7. Сформировать один из минимальных путей, проходя в обратном порядке по списку волн.

	// Сформируем связку отношения "простая цепь*"
	//
	sc_addr route        = s->create_el(seg, SC_N_CONST); // связка отношения "простая цепь*"
	sc_addr route_struct = s->create_el(seg, SC_N_CONST); // ориентированный граф структуры маршрута
	sc_set::include_in(s, route_struct, graph_theory::directed_graph);
	sc_addr route_visit  = s->create_el(seg, SC_N_CONST); // бинарное отношение посещения

	// Добавим все компоненты в связку отношения "простая цепь*".
	//
	sc_tup::add(s, route, N1_, route_struct);
	sc_tup::add(s, route, N2_, graph);
	sc_tup::add(s, route, N3_, route_visit);

	sc_set::include_in(s, route, graph_theory::simple_trail);

	// Добавим в простую цепь посещение начальной вершины.
	//
	sc_addr beg_vertex_visit = add_vertex_visit_to_route(s, route, beg_vertex);

	// Добавим в простую цепь посещение конечной вершины.
	//
	sc_addr end_vertex_visit = add_vertex_visit_to_route(s, route, end_vertex);

	// Пройдем в обратном направлении по списку волн
	// и сформируем структуру маршрута.
	//

	sc_addr curr_vertex = end_vertex;

	// Строим из списка волн маршрут, проходя по этому списку в обратном направлении.
	sc_list::reverse_iterator list_it(s, waves_list_tail), list_end;
	for (++list_it; list_it != list_end; ++list_it) {
		sc_addr curr_wave = *list_it;

		sc_addr edge = find_any_edge(s, graph, curr_vertex, curr_wave);

		// Получаем предыдущую вершину в пути.
		//
		sc_addr prev_vertex = get_other_vertex_incidence_edge(s, edge, curr_vertex); 

		// Добавляем посещение ребра @p edge в путь.
		//
		add_edge_visit_to_route(s, route, edge, prev_vertex, curr_vertex);
		curr_vertex = prev_vertex;
	}

	// Подчистим память...
	//
	erase_waves_list(s, waves_list_head);

	return route;
}
