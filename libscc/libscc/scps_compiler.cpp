/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

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

#include <libsc/sc_event_multi.h>

#include <SCPsLexer.h>
#include <SCPsParser.h>
#include "scps_compiler.h"

using namespace scc;

#if defined(_MSC_VER)
// disable forcing value to bool
#pragma warning(disable:4800)
#endif

#define SCC_DEBUG_ID(addr) ((addr->seg == wseg) ? SC_QID(addr) : SC_QURI(addr))

compiler *scc::cc;

pANTLR3_INPUT_STREAM antlr3AsciiStdinStreamNew();

static void
report_error(pANTLR3_BASE_RECOGNIZER recognizer)
{
	recognizer->state->errorRecovery = ANTLR3_FALSE;

	// Indicate this recognizer had an error while processing.
	//
	recognizer->state->errorCount++;

	// Call the error display routine
	//
	recognizer->displayRecognitionError(recognizer, recognizer->state->tokenNames);
	exit(5);
}

bool compiler::is_type_conversion(const lang_object_ref *from, const lang_object_ref *to,
								  sc_type arc_type, const file_position *arc_pos)
{
	object_type from_type = from->obj()->type();
	object_type to_type   = to->obj()->type();

	if (to_type == OBJ_SPEC_NODE)
		SCC_REPORT_ERROR_BF(arc_pos, "End of type conversion arc from '%1%' cannot be special node '%2%'.",
			from->name() % to->name())

	if (from_type == OBJ_SPEC_NODE) {
		if (!(to_type & OBJ_SC_ELEMENT))
			SCC_REPORT_ERROR_BF(arc_pos, "End of type conversion arc from '%1%' cannot be '%2%', because it isn't sc-element.",
				from->name() % to->name())

		if (arc_type != SC_ARC_MAIN)
			SCC_REPORT_ERROR_BF(arc_pos, "Type conversion arc from '%1%' to '%2%' can only be main sc-arc '->'.",
				from->name() % to->name())

		return true;
	} else {
		return false;
	}
}

void compiler::apply_type_conversion(const lang_object_ref *from, const lang_object_ref *to,
									 const file_position *arc_pos)
{
	special_node *spec_node = reinterpret_cast<special_node *>(from->obj());
	sc_addr el = to->obj()->sign();

	sc_type old_type = session()->get_type(el);
	sc_type old_subtype = old_type & spec_node->mask();

	if (spec_node->is_allowed(old_subtype))
		session()->change_type(el, (old_type & (~spec_node->mask()))|spec_node->new_type());
	else
		SCC_REPORT_ERROR_BF(arc_pos, "Cannot convert from '%1%' to '%2%'.",
			get_type_info(old_subtype) % get_type_info(spec_node->new_type()))
}

template<class T>
void compiler::create_special_node(const sc_string &name, sc_type new_type, sc_type mask)
{
	sc_addr sign = s->create_el(proc_seg, SC_N_CONST);

	special_node *special = new T(sign, new_type, mask);
	registry(special);

	scope->put(name.c_str(), special, true);
	s->set_idtf(sign, name);
}

compiler::compiler(sc_session *_s, sc_segment *_seg)
	: s(_s), wseg(_seg), gen(s, wseg), debug_mode_(false), expect_label(false)
{
	scope = names_scope::create();

	proc_seg = create_segment_full_path(s, "/proc/scc");

	create_special_node<syntactic_special_node>("@node",     SC_NODE,       SC_SYNTACTIC_MASK);
	create_special_node<syntactic_special_node>("@arc-main", SC_ARC_MAIN,   SC_SYNTACTIC_MASK|SC_CONSTANCY_MASK|SC_FUZZYNESS_MASK);
	create_special_node<syntactic_special_node>("@undf",     SC_UNDF,       SC_SYNTACTIC_MASK);
	create_special_node<syntactic_special_node>("@arc",      SC_ARC, SC_SYNTACTIC_MASK);

	create_special_node<fuz_special_node>("@fuz", SC_FUZ, SC_FUZZYNESS_MASK);
	create_special_node<fuz_special_node>("@pos", SC_POS, SC_FUZZYNESS_MASK);
	create_special_node<fuz_special_node>("@neg", SC_NEG, SC_FUZZYNESS_MASK);
}

static
void compile_impl(pANTLR3_INPUT_STREAM input_stream) throw (lang_error)
{
	do_init(false, false);

	pSCPsLexer lexer = SCPsLexerNew(input_stream);
	lexer->pLexer->rec->reportError = report_error;
	pANTLR3_COMMON_TOKEN_STREAM token_stream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
		lexer->pLexer->rec->state->tokSource);

	pSCPsParser parser = SCPsParserNew(token_stream);
	parser->pParser->rec->reportError = report_error;

	parser->text(parser);

	parser->free(parser);              parser = 0;
	token_stream->free(token_stream);  token_stream = 0;
	lexer->free(lexer);                lexer = 0;
	input_stream->close(input_stream); input_stream = 0;
}

void compiler::compile() throw (lang_error)
{
	compile_impl(antlr3AsciiStdinStreamNew());
}

void compiler::compile(const fs::path &p) throw (lang_error)
{
	compile_impl(open_file(p));
}

compiler::~compiler()
{
}

sc_type compiler::get_type_from_idtf(const sc_string &idtf)
{
	SC_ASSERT(!idtf.empty());
	size_t start_idx = is_ref_idtf(idtf) ? 1 : 0;
	if (idtf[start_idx] == '_') {
		if ((idtf.length() - start_idx) > 1 && idtf[start_idx + 1] == '_') {
			return SC_METAVAR;
		} else {
			return SC_VAR;
		}
	} else {
		return SC_CONST;
	}
}

lang_object *compiler::term_idtf_full_uri(const char *idtf, const file_position *pos)
{
	SCC_ASSERT_BF(!expect_label, pos,
		"Expects name of label, but passed URI is bad name for label: %1%.", idtf)

	const sc_string dir = dirname(idtf);
	const sc_string base = basename(idtf);

	lang_object *obj = 0;

	sc_segment *seg = create_segment_full_path(s, dir);
	sc_addr addr = s->find_by_idtf(base, seg);
	if (addr == 0) {
		addr = s->create_el(seg, get_type_from_idtf(base)|SC_UNDF);
		s->set_idtf(addr, base.c_str());
		obj = create_sc_element(addr);
	} else {
		obj = get_obj(addr);
		if (!obj)
			obj = create_sc_element(addr);
	}

	return obj;
}

lang_object *compiler::term_idtf_short(const char *idtf, const file_position *pos, const char *&name)
{
	lang_object *obj = 0;

	names_scope::binding *bnd = scope->get_up(idtf);
	if (!bnd) {
		sc_type type = get_type_from_idtf(idtf)|SC_UNDF;

		sc_addr addr = s->create_el(wseg, type);

		if (scope->is_head() && !is_ref_idtf(idtf))
			s->set_idtf(addr, idtf);

		bnd = scope->put(idtf, create_sc_element(addr));
	}

	name = bnd->name();

	return bnd->obj();
}

lang_object_ref *compiler::term_idtf(const char *idtf, const file_position *pos)
{
	lang_object *obj  = 0;
	const char  *name = 0;

	if (idtf[0] == '/') {
		obj = term_idtf_full_uri(idtf, pos);
	} else {
		if (expect_label) {
			SCC_ASSERT(!prgs_stack.empty(), pos, "Expects label without program scope.");

			// TODO: scps labels
			//obj = prgs_stack.back()->get_exist_or_new_label(idtf);
		} else {
			obj = term_idtf_short(idtf, pos, name);
		}
	}

	return new_obj_ref(pos, obj, name);
}

lang_object_ref *compiler::term_simple(lang_object_ref *ref)
{
	gen.element(ref->obj()->sign());
	return ref;
}

lang_object_ref *compiler::term_from_content(const file_position *pos)
{
	sc_addr addr = gen.element("", SC_UNDF|SC_CONST);

	SCC_ASSERT(addr, pos, "Cannot create sign for term from content.");

	gen.finish();

	lang_object *obj  = create_sc_element(addr);
	return new_obj_ref(pos, obj, 0);
}

void compiler::set_alias_as_idtf(sc_addr addr, const sc_string &alias, bool &has_idtf)
{
	if (!has_idtf && scope->is_head() && !is_ref_idtf(alias)) {
		sc_string idtf = s->get_idtf(addr);

		if (idtf.length() != 1 && idtf[0] == '@' && idtf[1] == '@')
			s->set_idtf(addr, alias);

		has_idtf = true;
	}
}

lang_object_ref *compiler::assign_aliases(lang_object_ref *ref, sc_aliases_list &aliases)
{
	if (aliases.empty()) {
		return ref;
	} else {
		lang_object *obj  = ref->obj();
		sc_addr      addr = obj->sign();

		if (debug_mode_) {
			std::cout << "[SCC] ";
			sc_aliases_list::const_iterator it = aliases.begin();
			for (; it != aliases.end(); ++it)
				std::cout << "\"" << (*it)->name() << "\" = ";
			std::cout << SCC_DEBUG_ID(addr) << std::endl;
		}

		if (obj->type() == OBJ_SPEC_NODE)
			SCC_REPORT_ERROR_BF(aliases.back()->pos(), "Cannot assign alias to special node '%1%'.",
				ref->name())

		bool has_idtf = false;
		const char *ref_name = 0;

		sc_aliases_list::const_reverse_iterator it = aliases.rbegin();
		for (; it != aliases.rend(); ++it) {
			const sc_string &alias = (*it)->name();

			names_scope::binding *bnd = scope->get_down(alias);
			if (bnd) {
				ref_name = bnd->name();

				if (bnd->obj() == obj) {
					SCC_REPORT_WARNING_BF((*it)->pos(), "Duplicated alias '%1%' for same sc-element.", bnd->name());
					continue;
				}

				// Another language object has this alias as name.
				//

				//if (is_external(bnd->obj())) {
					// If already binded object is external then rebind
					// alias to object from working segment.
					// 
					// Local object masks external object.
					//

// 					if (!is_external(obj)) {
// 						scope->rebind(bnd->name(), obj);
// 						set_alias_as_idtf(addr, alias, has_idtf);
// 					} else {
// 						SCC_REPORT_ERROR_BF((*it)->pos(), "Cannot merge external sc-elements '%1%' and '%2%'.",
// 							SC_URI(bnd->obj()->sign()) % SC_URI(addr))
// 					}
// 				} else {

				if (bnd->obj()->type() != OBJ_SC_ELEMENT || obj->type() != OBJ_SC_ELEMENT)
					SCC_REPORT_ERROR((*it)->pos(), "Yet compiler can merge only simple sc-elements.")

				if (sc_merge_elements(s, bnd->obj()->sign(), addr))
					SCC_REPORT_ERROR_BF((*it)->pos(), "Cannot merge '%1%' with '%2%'.",
						SC_ELTYPE(bnd->obj()->sign()) % SC_ELTYPE(addr))

				// TODO: Think more about merging, types and unique names
				scope->replace_down(obj, bnd->obj());

				deregistry(obj);
				delete obj;

				obj  = bnd->obj();
				addr = obj->sign();

				if (is_external(obj)) {
					// HACK: This is temporary hack for creating
					// link to external object in working segment.
					//
					sc_addr dummy_edge = s->create_el(wseg, SC_EDGE);
					s->set_end(dummy_edge, addr);
					s->erase_el(dummy_edge);
				}
// 				}
			} else {
				set_alias_as_idtf(addr, alias, has_idtf);
				ref_name = scope->put(alias, obj)->name();
			}
		}

		return new_obj_ref(aliases.front()->pos(), obj, ref_name);
	}
}

void compiler::assign_string_content(lang_object_ref *ref, const char *str)
{
	s->set_content(ref->obj()->sign(), Content::string(str));
}

void compiler::assign_integer_content(lang_object_ref *ref, const char *str,
									  const file_position *pos)
{
	std::istringstream in(str);
	cint number;
	in >> number;

	if (in.fail())
		SCC_REPORT_ERROR(pos, "Cannot parse integer content.")
	else
		s->set_content(ref->obj()->sign(), Content::integer(number));
}

void compiler::assign_double_content(lang_object_ref *ref, const char *str,
									 const file_position *pos)
{
	std::istringstream in(str);
	creal number;
	in >> number;

	if (in.fail())
		SCC_REPORT_ERROR(pos, "Cannot parse real content.")
	else
		s->set_content(ref->obj()->sign(), Content::real(number));
}

void compiler::assign_file_content(lang_object_ref *ref, const char *str,
								   const file_position *pos)
{
	fs::path p(str);

	if (!fs::exists(p))
		SCC_REPORT_ERROR_BF(pos, "Content file path %1% doesn't exist.", p)

	if (!fs::is_regular_file(p))
		SCC_REPORT_ERROR_BF(pos, "Content file path %1% doesn't exist.", p)

	std::ifstream in(p.string().c_str(), std::ifstream::in|std::ifstream::binary);

	size_t size = static_cast<size_t>(fs::file_size(p));
	boost::scoped_array<char> data(new char[size]);

	in.read(data.get(), size);

	s->set_content(ref->obj()->sign(), Content::data(size, data.get()));
}

void compiler::change_type(sc_addr addr, sc_type t)
{
	s->change_type(addr, t);
}

sc_addr compiler::gen_connector(sc_ident *from, sc_type arc_type, const file_position *arc_pos, sc_ident *to)
{
	sc_addr arc = 0;

	if (sc_isa(arc_type, SC_ARC) && !new_sc_aplha_) {
		// Generate binary ordered tuple for general sc-arc.
		// It's old sc-alphabet.
		//

		if (!to->attrs()->empty())
			SCC_REPORT_ERROR(arc_pos, "In old sc-alphabet general sc-arc cannot have attributes.")

		sc_type const_type = SC_CONSTANCY_MASK & arc_type;

		gen.attr_arc_type = gen.arc_type = SC_ARC_ACCESSORY|SC_POS|const_type;

		arc = gen.enter_set("", SC_NODE|const_type);
		{
			gen.attr(N1_);
			gen.element(from->ident()->obj()->sign());
			gen.attr(N2_);
			gen.element(to->ident()->obj()->sign());
		}
		gen.leave_set();
	} else {
		// FIXME: ugly working with sc-generator
		arc = gen.element("", arc_type);
		s->set_beg(arc, from->ident()->obj()->sign());
		s->set_end(arc, to->ident()->obj()->sign());
		gen.finish();

		const sc_attributes *attrs = to->attrs();

		gen.arc_type = SC_ARC_ACCESSORY|SC_POS|SC_CONST;
		foreach (lang_object *attr, attrs->consts())
			gen.element(attr->sign());
		gen.arc();
		gen.element(arc);
		gen.finish();

		gen.arc_type = SC_ARC_ACCESSORY|SC_POS|SC_VAR;
		foreach (lang_object *attr, attrs->vars())
			gen.element(attr->sign());
		gen.arc();
		gen.element(arc);
		gen.finish();

		gen.arc_type = SC_ARC_ACCESSORY|SC_POS|SC_METAVAR;
		foreach (lang_object *attr, attrs->metavars())
			gen.element(attr->sign());
		gen.arc();
		gen.element(arc);
		gen.finish();
	}

	gen.finish();

	return arc;
}

lang_object_ref *compiler::connector(sc_ident *from, sc_type arc_type,
									 const file_position *arc_pos, sc_ident *to)
{
	SCC_ASSERT(!expect_label, arc_pos, "Expects name of label.");

	lang_object_ref *from_ref = from->ident();
	lang_object_ref *to_ref   = to->ident();

	if (debug_mode_)
		std::cout << "[SCC] Connector from " << SCC_DEBUG_ID(from_ref->obj()->sign()) << " to "
			<< SCC_DEBUG_ID(to_ref->obj()->sign()) << std::endl;

	if (is_type_conversion(from_ref, to_ref, arc_type, arc_pos))
		SCC_REPORT_ERROR(arc_pos, "Cannot take reference from type conversion arc.")

	sc_addr arc = gen_connector(from, arc_type, arc_pos, to);
	return new_obj_ref(arc_pos, create_sc_element(arc), 0);
}

void compiler::scsentence(const sc_idents_list &from_list, sc_type arc_type,
						  const file_position *arc_pos, const sc_idents_list &to_list)
{
	SCC_ASSERT(!expect_label, arc_pos, "Expects name of label.");

	foreach (sc_ident *from, from_list) {
		if (from->ident()->obj()->type() == OBJ_SPEC_NODE) {
			foreach (sc_ident *to, to_list) {
				const lang_object_ref *from_ref = from->ident();
				const lang_object_ref *to_ref   = to->ident();

				// Begin of type conversion arc is special node then
				// next call return true or break program with error.
				is_type_conversion(from_ref, to_ref, arc_type, arc_pos);

				if (!to->attrs()->empty())
					SCC_REPORT_ERROR_BF(arc_pos, "Type conversion arc from '%1%' to '%2%' must be without attributes.",
						from_ref->name() % to_ref->name())

				apply_type_conversion(from_ref, to_ref, arc_pos);
			}
		} else {
			foreach (sc_ident *to, to_list)
				gen_connector(from, arc_type, arc_pos, to);
		}
	}
}

void compiler::scattribute(lang_object_ref *ref, sc_type t, const file_position *pos)
{
	SCC_ASSERT(!expect_label, pos, "Expects name of label.");

	gen.attr_arc_type = t|SC_ARC_ACCESSORY|SC_POS;
	gen.attr(ref->obj()->sign());
}

lang_object_ref *compiler::enter_term_set(sc_type t, const file_position *pos)
{
	return new_obj_ref(pos, create_sc_element(gen.enter_set("", t)), 0);
}

void compiler::leave_term_set()
{
	gen.leave_set();
}

lang_object_ref *compiler::enter_term_sys(sc_type t, const file_position *pos)
{
	return new_obj_ref(pos, create_sc_element(gen.enter_system("", t)), 0);
}

void compiler::leave_term_sys()
{
	gen.leave_system();
}

lang_object_ref *compiler::enter_term_ord_tuple(sc_type t, const file_position *pos)
{
	SCC_REPORT_ERROR(pos, "SCC doesn't support ordered tuples in this version.");
	return 0;
}

void compiler::leave_term_ord_tuple()
{

}

void compiler::add_include_dir(const fs::path &p) throw (lang_error)
{
	fs::path abs_path = fs::absolute(p);

	if (!fs::exists(abs_path))
		SCPS_THROW_ERROR_BF("Includes path %1% doesn't exist.", abs_path)

	if (!fs::is_directory(abs_path))
		SCPS_THROW_ERROR_BF("Includes path %1% isn't directory path.", abs_path)

	if (std::find(include_dirs.begin(), include_dirs.end(), abs_path) == include_dirs.end())
		include_dirs.push_back(abs_path);
}

fs::path compiler::find_include_file(const fs::path &p) throw (lang_error)
{
	if (p.is_absolute()) {
		if (!fs::exists(p))
			SCPS_THROW_ERROR_BF("Included path %1% doesn't exist.", p)

		if (!fs::is_regular_file(p))
			SCPS_THROW_ERROR_BF("Included path %1% isn't regular file path.", p)

		return p;
	} else {
		include_dirs_list::const_iterator it = include_dirs.begin();
		for (; it != include_dirs.end(); ++it) {
			fs::path include_path = *it;
			include_path /= p;
			if (fs::exists(include_path) && fs::is_regular_file(include_path))
				return include_path;
		}
	}

	SCPS_THROW_ERROR_BF("Included file for path %1% doesn't found.", p)
}

void compiler::report_error(const sc_string &msg, const file_position *pos)
{
	std::cerr << pos->stream() << " (" << pos->line();

	if (pos->begin() != -1)
		std::cerr << ":" << pos->begin();

	std::cerr << ") : error : " << msg << std::endl;

	exit(5);
}

void compiler::report_warning(const sc_string &msg, const file_position *pos)
{
	std::cerr << pos->stream() << " (" << pos->line();

	if (pos->begin() != -1)
		std::cerr << ":" << pos->begin();

	std::cerr << ") : warning : " << msg << std::endl;
}
