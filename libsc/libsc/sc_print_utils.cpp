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

#include "precompiled_p.h"

#ifdef WIN32
	#define snprintf _snprintf
#endif

/// Format for printing integer content.
#define SC_CINT_PRINTF_FORMAT "%ld"

/// Format for printing real content.
#define SC_CREAL_PRINTF_FORMAT "%g"

sc_retval print_content(const Content &cont)
{
	Cont c = cont;
	switch (cont.type()) {
	case TCSTRING:
		printf("%s", c.d.ptr);
		break;
	case TCINT:
		printf(SC_CINT_PRINTF_FORMAT, c.i);
		break;
	case TCREAL:
		printf(SC_CREAL_PRINTF_FORMAT, (double) c.r);
		break;
	case TCEMPTY:
		break;
	default:
		return RV_ERR_GEN;
	}
	return RV_OK;
}

struct type2visual
{
	sc_type t;
	const char *v;
};

sc_string get_type_info(sc_type type)
{
	sc_string result;
	result.reserve(25);

	static const type2visual types[] =  {
		{SC_NODE,          "node"},
		{SC_UNDF,          "undf"},
		{SC_ARC,           "arc"},
		{SC_EDGE,          "edge"},
		{SC_ARC_ACCESSORY, "arc-accessory"},
		{SC_LINK,          "link"},

		{SC_CONST,         "const"},
		{SC_VAR,           "var"},
		{SC_METAVAR,       "metavar"},

		{SC_POS,           "pos"},
		{SC_NEG,           "neg"},
		{SC_FUZ,           "fuz"},
		
		{SC_PERMANENT,     "permanent"},
		{SC_TEMPORARY,     "temporary"}
	};

	bool have_one = false;
	for (int i = 0; i < (sizeof(types) / sizeof(type2visual)); i++) {
		if (sc_isa(type, types[i].t)) {
			if (have_one) {
				result += "|";
			} else {
				have_one = true;
			}
			result += types[i].v;
		}
	}

	return result;
}

sc_string get_el_short_info(sc_session *s, sc_addr el)
{
	std::ostringstream out;

	if (el) {
		const sc_string &seg_uri = el->seg->get_full_uri();

		if (s->is_segment_opened(el->seg)) {
			out << get_type_info(s->get_type(el)) << ":" << seg_uri << "/" << s->get_idtf(el);
		} else {
			out << "<NIL (segment " << seg_uri << " isn't opened)>";
		}
	} else {
		out << "<NIL>";
	}

	return out.str();
}

static void _get_element_info(char *buf, size_t buflen, sc_addr a, sc_session *s)
{
	sc_string idtf;
	sc_type type;

	if (!a) {
__print_null:
		strncpy(buf,"(null)",buflen);
		return;
	}

	idtf = a->seg->get_full_uri() + "/";

	type = s->get_type(a);
	if (s->get_error()) {
		goto __print_null;
	}

	sc_string idtfStr = s->get_idtf(a);
	if (s->get_error()) {
		goto __print_null;
	}

	idtf += idtfStr;

	strncpy(buf, get_type_info(type).c_str(), buflen);
	strcat(buf,":");
	strncat(buf,idtf.c_str(),buflen-strlen(buf)-1);
}

static char* get_element_info(sc_addr a, sc_session *s)
{
	char buf[512];
	sc_type type;

	_get_element_info(buf,sizeof(buf),a,s);

	type = s->get_type(a);

	if (!s->get_error() && sc_isa(type, SC_CONNECTOR_MASK)) {
		size_t len = strlen(buf);
		snprintf(buf+len,sizeof(buf)," = <");
		len = strlen(buf);
		_get_element_info(buf+len,sizeof(buf)-len-1,s->get_beg(a),s);
		len = strlen(buf);
		strncpy(buf+len,",",sizeof(buf)-len-1);
		len = strlen(buf);
		_get_element_info(buf+len,sizeof(buf)-len-1,s->get_end(a),s);
		len = strlen(buf);
		strncpy(buf+len,">",sizeof(buf)-len-1);
	}

	return strdup(buf);
}

void print_element_info(sc_addr a,sc_session *s)
{
	if (s == 0) {
		s = system_session;
	}
	char *info = get_element_info(a,s);
	printf("%s",info);
	free(info);
}

static void print_short_element_info(sc_addr a, sc_session *s)
{
	char buf[256];
	_get_element_info(buf,sizeof(buf),a,s);
	printf("%s",buf);
}

int print_el_variant;
int print_el_max_power;

sc_retval print_el(sc_session *session, sc_addr el)
{
	int max_elements = 0x7ffffff;
	printf("printEl: ");
	print_element_info(el, session);

	printf("\nOutput arcs:\n");
	if (print_el_max_power && session->get_out_qnt(el) > print_el_max_power) {
		if (print_el_variant) {
			max_elements = 1;
			// printf("Too many arcs to print\n");
		} else {
			max_elements = print_el_max_power+1;
		}
	}
	sc_iterator *iter = session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, el, 0, 0), true);
	for (;!iter->is_over() && --max_elements;iter->next()) {
		printf("\t");
		print_short_element_info(iter->value(1), session);
		printf(" >- ");
		print_short_element_info(iter->value(2), session);
		printf("\n");
	}
	if (!iter->is_over()) {
		printf("Skipping the rest\n");
	}
	delete iter;

	printf("\nInput arcs:\n");
	if (print_el_max_power && session->get_in_qnt(el) > print_el_max_power) {
		if (print_el_variant) {
			max_elements = 1;
			// printf("Too many arcs to print\n");
		} else {
			max_elements = print_el_max_power+1;
		}
	}
	iter = session->create_iterator(sc_constraint_new(CONSTR_3_a_a_f, 0, 0, el), true);
	for (;!iter->is_over() && --max_elements;iter->next()) {
		printf("\t");
		print_short_element_info(iter->value(1), session);
		printf(" -< ");
		print_short_element_info(iter->value(0), session);
		printf("\n");
	}
	if (!iter->is_over()) {
		printf("Skipping the rest\n");
	}

	delete iter;

	printf("\n");

	fflush(stdout);

	return RV_OK;
}

void print_el(sc_addr el)
{
	print_el(system_session, el);
}

#define SIZEOF_VISUAL_MAP(m) (sizeof(m) / sizeof(type2visual))

template<size_t size>
static const char* get_visual(const type2visual visual_map[], sc_type t)
{
	for (size_t i = 0; i < size; ++i) {
		if (t & visual_map[i].t)
			return visual_map[i].v;
	}

	SC_ASSERT(false && "Not found visual representation.");
	return 0;
}

static const type2visual arc_end_visual_map[] = {
	{SC_CONST,   ">"},
	{SC_VAR,     ">>"},
	{SC_METAVAR, ">>>"}
};

static const type2visual arc_start_visual_map[] =  {
	{SC_POS, "-"},
	{SC_NEG, "!"},
	{SC_FUZ, "~"}
};

static const type2visual open_set_visual_map[] = {
	{SC_CONST,   "{"},
	{SC_VAR,     "{."},
	{SC_METAVAR, "{.."}
};

static const type2visual close_set_visual_map[] = {
	{SC_CONST,   "}"},
	{SC_VAR,     ".}"},
	{SC_METAVAR, "..}"}
};

static const type2visual attr_visual_map[] = {
	{SC_CONST,   ":"},
	{SC_VAR,     "::"},
	{SC_METAVAR, ":::"}
};

void print_struct(sc_session *session, sc_addr el)
{
	printf("printStruct: ");
	print_element_info(el, system_session);

	printf("\nOutput arcs:\n");
	sc_iterator *iter = session->create_iterator(sc_constraint_new(CONSTR_3_f_a_a, el, 0, 0), true);
	for (;!iter->is_over(); iter->next()) {
		if (session->get_in_qnt(iter->value(1)) > 0) {
			printf("\tAttributes:\n");
			sc_iterator *it2 = session->create_iterator(sc_constraint_new(CONSTR_3_a_a_f, 0, 0, iter->value(1)), true);
			for (;!it2->is_over();it2->next()) {
				printf("\t");
				print_short_element_info(it2->value(1),session);
				printf(" -< ");
				print_short_element_info(it2->value(0),session);
				printf("\n");
			}
			delete it2;
		}
		printf("\tElement:\n\t");
		print_short_element_info(iter->value(1), session);
		printf(" >- ");
		print_short_element_info(iter->value(2), session);
		printf("\n");
	}
	delete iter;
	printf("\n");
}

void print_struct(sc_addr el)
{
	print_struct(system_session, el);
}

void scs_printer::open_set(std::ostream &os, sc_type type, size_t indent)
{
	indentation(os, indent);
	os << get_visual<SIZEOF_VISUAL_MAP(open_set_visual_map)>(
		open_set_visual_map,
		type);
}

void scs_printer::close_set(std::ostream &os, sc_type type, size_t indent)
{
	indentation(os, indent);
	os << get_visual<SIZEOF_VISUAL_MAP(close_set_visual_map)>(
		close_set_visual_map,
		type);
}

sc_string scs_printer::arc_visual(sc_type t)
{
	std::ostringstream os;
	
	// Search visual representation for positiveness (arc start)
	os << get_visual<SIZEOF_VISUAL_MAP(arc_start_visual_map)>(arc_start_visual_map, t)
		// Search visual representation for constancy (arc end)
		<< get_visual<SIZEOF_VISUAL_MAP(arc_end_visual_map)>(arc_end_visual_map, t);

	return os.str();
}

const char* scs_printer::attr_visual(sc_type t)
{
	return get_visual<SIZEOF_VISUAL_MAP(attr_visual_map)>(attr_visual_map, t);
}

const sc_string& scs_printer::get_short_scs_idtf(sc_session *s, addr2idtf_map &idtf_map, sc_addr addr, bool &is_new)
{
	addr2idtf_map::const_iterator it = idtf_map.find(addr);
	if (it != idtf_map.end()) {
		is_new = false;
		return it->second;
	} else {
		is_new = true;

		std::ostringstream ostr;

		sc_string idtf = s->get_idtf(addr);
		if (is_system_id(idtf)) {
			ostr << "$";

			sc_type t = s->get_type(addr);

			if (t & SC_VAR) {
				ostr << "_";
			} else if (t & SC_METAVAR) {
				ostr << "__";
			}

			if (sc_isa(t, SC_CONNECTOR_MASK)) {
				ostr << "a";
			} else if (sc_isa(t, SC_NODE)) {
				ostr << "n";
			} else if (sc_isa(t, SC_UNDF)) {
				ostr << "u";
			} else {
				ostr << "unknow";
			}

			ostr << idtf_map.size() + 1;
		} else {
			ostr << idtf;
		}

		return idtf_map.insert(addr2idtf_map::value_type(addr, ostr.str())).first->second;
	}
}

void scs_printer::set_members(std::ostream &os, sc_session *s, sc_addr addr, addr2idtf_map &idtf_map, size_t depth, size_t indent)
{
	SC_ASSERT(s);

	if (depth) {
		if (addr && !addr->is_dead()) {
			sc_type const_type = s->get_type(addr) & SC_CONSTANCY_MASK;

			sc_iterator *it = s->create_iterator(sc_constraint_new(
				CONSTR_3_f_a_a, addr, SC_ARC_ACCESSORY|const_type|SC_POS, 0), true);
			for (SC_ITERATOR_PTR_DELETER(it); !it->is_over();) {
				//
				// Print only attributes which are keynodes
				//
				sc_iterator *a_it = s->create_iterator(sc_constraint_new(
					CONSTR_3_a_a_f, 0, SC_POS, it->value(1)), true);

				bool has_attrs = false;

				sc_for_each (a_it) {
					sc_addr attr = a_it->value(0);
					sc_string idtf = s->get_idtf(attr);

					if (!is_system_id(idtf) && boost::ends_with(idtf, "_")) {
						if (!has_attrs)
							indentation(os, indent);

						has_attrs = true;
						element(os, s, attr, idtf_map, 1, 0);
						os << attr_visual(s->get_type(a_it->value(1))) << " ";
					}
				}

				if (has_attrs)
					os << std::endl;

				sc_addr member = it->value(2);
				element(os, s, member, idtf_map, depth, indent + (has_attrs ? 1 : 0));

				it->next();

				if (!it->is_over())
					os << ",";
				os << std::endl;
			}
		} else {
			indentation(os, indent);
			os << "NIL";
		}
	}
}

void scs_printer::arc(std::ostream &os, sc_session *s, sc_addr addr, addr2idtf_map &idtf_map, size_t depth, size_t indent)
{
	SC_ASSERT(s);

	if (depth) {
		indentation(os, indent);

		if (addr && !addr->is_dead()) {
			sc_type type = s->get_type(addr);
			sc_addr beg = s->get_beg(addr);
			sc_addr end = s->get_end(addr);

			element(os, s, beg, idtf_map, depth);

			os << " "
				// Search visual representation for positiveness (arc start)
				<< get_visual<SIZEOF_VISUAL_MAP(arc_start_visual_map)>(arc_start_visual_map, type)
				// Search visual representation for constancy (arc end)
				<< get_visual<SIZEOF_VISUAL_MAP(arc_end_visual_map)>(arc_end_visual_map, type)
				<< " ";

			element(os, s, end, idtf_map, depth);
		} else {
			os << "NIL";
		}
	}
}

void scs_printer::content(std::ostream &os, const Content &cont)
{
	Cont c = cont;
	switch (cont.type()) {
	case TCSTRING:
		os << " =c= /\"" << c.d.ptr << "\"/";
		break;
	case TCINT:
		os << " =n= /\"" << c.i << "\"/";
		break;
	case TCREAL:
		os << " =n= /\"" << (double) c.r << "\"/";
		break;
	}
}

/// Print element in SCs format.
void scs_printer::element(std::ostream &os, sc_session *s, sc_addr addr, addr2idtf_map &idtf_map, size_t depth, size_t indent)
{
	SC_ASSERT(s);

	if (depth) {
		indentation(os, indent);

		if (addr && !addr->is_dead()) {
			sc_type type = s->get_type(addr);

			bool is_new = false;

			const sc_string &short_idtf = get_short_scs_idtf(s, idtf_map, addr, is_new);
			os << short_idtf;

			--depth;

			if (is_new) {
				os << " = " << "\"" << get_full_uri(s, addr) << "\"";

				const Content *cont = s->get_content_const(addr);
				content(os, *cont);

				if (sc_isa(type, SC_CONNECTOR_MASK)) {
					os << " = (";

					if (depth) {
						os << std::endl;
						arc(os, s, addr, idtf_map, depth, indent + 1);
						os << std::endl;
						indentation(os, indent);
					} else {
						os << "...";
					}

					os << ")";
				} else if (!sc_set::is_empty(s, addr)) {
					if (depth) {
						os << " = ";
						open_set(os, type, 0);
						os << std::endl;
						set_members(os, s, addr, idtf_map, depth, indent + 1);
						close_set(os, type, indent);
					}
				}
			}
		} else {
			os << "NIL";
		}
	}
}

#ifdef WIN32
print_f::print_f(const char* format, ...)
{
	size_t size = 50;
	buf = new char[size];

	va_list args;
	va_start(args, format);
	int rc = vsnprintf(buf, 0, format, args);
	va_end(args);

	while (rc < 0) {
		delete []buf;
		size *= 2;
		buf = new char[size];
		va_start(args, format);
		rc = vsnprintf(buf, size, format, args);
		va_end(args);
	}
}
#else
print_f::print_f(const char* format, ...)
{
	va_list args;

	buf = new char[1];
	buf[0] = 0;

	va_start(args, format);
	int rc = vsnprintf(buf, 0, format, args);
	va_end(args);

	if (rc >= 0) {
		delete []buf;
		buf = new char[rc + 1];
		va_start(args, format);
		rc = vsnprintf(buf, rc + 1, format, args);
		va_end(args);
	}
}
#endif
