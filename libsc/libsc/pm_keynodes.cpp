
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

const char *pm_idtfs[] = {
	"/proc/keynode/scp_process",
	"/proc/keynode/scp_operator",
	"/proc/keynode/scp_operator_type",
	"/proc/keynode/programSCP",
	"/proc/keynode/init",
	"/proc/keynode/initWithData",
	"/proc/keynode/prepared_scp_program",
	"/proc/keynode/const_segments_",
	"/proc/keynode/default_seg_",
	"/proc/keynode/opened_segments_",
	"/proc/keynode/queue",
	"/proc/keynode/queue_head_",
	"/proc/keynode/queue_next_",
	"/proc/keynode/queue_prev_",
	"/proc/keynode/queue_value_",
	"/proc/keynode/queue_Nil",
	"/proc/keynode/const_",
	"/proc/keynode/var_",
	"/proc/keynode/metavar_",
	"/proc/keynode/pos_",
	"/proc/keynode/neg_",
	"/proc/keynode/fuz_",
	"/proc/keynode/arc_",
	"/proc/keynode/node_",
	"/proc/keynode/undf_",
	"/proc/keynode/permanent_",
	"/proc/keynode/temporary_",
	"/proc/keynode/actual_",
	"/proc/keynode/phantom_",
	"/proc/keynode/typeattrs",
	"/proc/keynode/process_state_then",
	"/proc/keynode/process_state_else",
	"/proc/keynode/process_state_repeat",
	"/proc/keynode/process_state_sleep",
	"/proc/keynode/process_state_run",
	"/proc/keynode/process_state_dead",
	"/proc/keynode/process_state_error",
	"/proc/keynode/process_",
	"/proc/keynode/msg_dstmaj_",
	"/proc/keynode/msg_dstmin_",
	"/proc/keynode/msg_srcmaj_",
	"/proc/keynode/msg_srcmin_",
	"/proc/keynode/msg_data_",
	"/proc/keynode/mailbox_hook_",
	"/proc/keynode/mailbox",
	"/proc/keynode/mailbox_",
	"/proc/keynode/mail_incoming_",
	"/proc/keynode/mail_outgoing_",
	"/proc/keynode/system_mailbox",
	"/proc/keynode/mail_data_",
	"/proc/keynode/this_pm_major",
	"/proc/keynode/module",
	"/proc/keynode/connected_module",
	"/proc/keynode/module_",
	"/proc/keynode/processing_module",
	"/proc/keynode/terminal_module",
	"/proc/keynode/current_module",
	"/proc/keynode/owner_",
	"/proc/keynode/user",
	"/proc/keynode/command",
	"/proc/keynode/assign_",
	"/proc/keynode/fixed_",
	"/proc/keynode/f_",
	"/proc/keynode/active_",
	"/proc/keynode/autosegment_",
	"/proc/keynode/sons_garbage_",
	"/proc/keynode/program_",
	"/proc/keynode/then_",
	"/proc/keynode/else_",
	"/proc/keynode/goto_",
	"/proc/keynode/error_",
	"/proc/keynode/init_",
	"/proc/keynode/var_value_",
	"/proc/keynode/real_return_value",
	"/proc/keynode/list",
	"/proc/keynode/list_next_",
	"/proc/keynode/list_value_",
	"/proc/keynode/breakpoint",
	"/proc/keynode/ptrace_",
	"/proc/keynode/ptracer",
	"/proc/keynode/debugger",
	"/proc/keynode/breakpoint",
	"/proc/keynode/tracers_",
	"/proc/keynode/breakpoints_",
	"/proc/keynode/debug_copies_",
	"/proc/keynode/originals_",
	"/proc/keynode/segment_",
	"/proc/keynode/set_",
	"/proc/keynode/idtf_",
	"/proc/keynode/content_",
	"/proc/keynode/changeSet",
	"/proc/keynode/before_",
	"/proc/keynode/after_",
	"/proc/keynode/equivalents_",
	"/proc/keynode/merges_",
	"/proc/keynode/apply_diff",
	"/proc/keynode/messageBox",
	"/proc/keynode/type_",
	"/proc/keynode/secondary_text_",
	"/proc/keynode/primary_text_",
	"/proc/keynode/ui_confirm",
	"/proc/keynode/ui_information",
	"/proc/keynode/ui_error",
	"/proc/keynode/ui_ok",
	"/proc/keynode/ui_yes",
	"/proc/keynode/ui_no",
	"/proc/keynode/ev_allow",
	"/proc/keynode/ev_deny",
	"/proc/keynode/scheduling_class",
	"/proc/keynode/sched_class_kernel",
	"/proc/keynode/sched_class_system",
	"/proc/keynode/sched_class_normal",
	"/proc/keynode/__subsched_kernel",
	"/proc/keynode/__subsched_system",
	"/proc/keynode/__subsched_normal",
	"/proc/keynode/sleeping",
	"/proc/keynode/message",
	"/proc/keynode/lazy_data_message",
	"/proc/keynode/arrived_message",
	"/proc/keynode/wakeup_hook",
	"/proc/keynode/removeFromSheet",
	"/proc/keynode/father_",
	"/proc/keynode/child_death_hook_",
	"/proc/keynode/in_",
	"/proc/keynode/out_",
	"/proc/keynode/prm_",
	"/proc/keynode/prm_",
	"/proc/keynode/waiting_for_",
	"/proc/keynode/All_childs",
	"/proc/keynode/output_prm_binding_",
	"/proc/keynode/wait_gen_output_arc",
	"/proc/keynode/wait_gen_input_arc",
	"/proc/keynode/wait_input_arc",
	"/proc/keynode/wait_output_arc",
	"/proc/keynode/wait_erase_element",
	"/proc/keynode/wait_gen_output_arc_pre",
	"/proc/keynode/wait_gen_input_arc_pre",
	"/proc/keynode/wait_erase_element_pre",
	"/proc/keynode/wait_recieve_message",
	"/proc/keynode/send_",
	"/proc/keynode/send_named_",
	"/proc/keynode/default_shell",
	"/proc/keynode/shell",
	"/proc/keynode/class",
	"/proc/keynode/extend_",
	"/proc/keynode/object",
	"/proc/keynode/object_",
	"/proc/keynode/implementation_",
	"/proc/keynode/program_",
	"/proc/keynode/signature",
	"/proc/keynode/varValue_",
	"/proc/keynode/prm_",
	"/proc/keynode/prm_name_",
	"/proc/keynode/factory_mailbox_hook",
	"/proc/keynode/sco_factory_mailbox",
	"/proc/keynode/true",
	"/proc/keynode/false",
	"/proc/keynode/registered_user",
	"/proc/keynode/fs_names",
	"/proc/keynode/fs_name_",
	"/proc/keynode/file",
	"/proc/keynode/directory",
	"/proc/keynode/operator_file_",
	"/proc/keynode/operator_line_",
	"/proc/keynode/relayed_command",
	"/proc/keynode/waiting_command",
	"/proc/keynode/shell_",
	"/proc/agents/keynode/agent",
	"/proc/agents/keynode/command_",
	"/proc/agents/keynode/mailbox_",
	"/proc/agents/keynode/active_",
	"/proc/agents/keynode/directory_",
	"/proc/agents/keynode/home",
	"/proc/agents/keynode/internal",
	"/proc/agents/keynode/external",
	"/proc/agents/keynode/return_value",
	"/proc/agents/keynode/load_graph",
	"/proc/agents/keynode/login",
	"/proc/agents/keynode/subcribe",
	"/proc/agents/keynode/subcribe_ok",
	"/proc/agents/keynode/build_program",
	"/proc/agents/keynode/run_program",
	"/proc/agents/keynode/connect_tm",
	"/proc/agents/keynode/erase",
	"/proc/agents/keynode/change",
	"/proc/agents/keynode/create_user",
	"/proc/agents/keynode/get_element_by_idtf",
	"/proc/agents/keynode/command_failed",
	"/proc/agents/keynode/personal agent",
	"/proc/agents/keynode/person_",
	"/proc/agents/keynode/agent_",
	"/proc/agents/keynode/inbox_",
	"/proc/agents/keynode/login_agent",
	"/proc/agents/keynode/startup_program_",
	"/proc/agents/shell/keynode/sheets",
	"/proc/agents/shell/keynode/TMLSheet",
	"/proc/agents/shell/keynode/login_ok",
	"/proc/agents/shell/keynode/login_failed",
	"/proc/agents/shell/keynode/caption_",
	"/proc/agents/shell/keynode/menu",
	"/proc/agents/shell/keynode/mf_string",
	"/proc/agents/shell/keynode/mf_end",
	"/proc/agents/shell/keynode/mf_popup",
	"/proc/agents/shell/keynode/active_settings",
	"/proc/agents/shell/keynode/resource",
	"/proc/agents/shell/keynode/id_",
	"/proc/agents/shell/keynode/item_",
	"/proc/agents/shell/keynode/size_",
	"/proc/agents/shell/keynode/dlg_controls",
	"/proc/agents/shell/keynode/dlg_properties",
	"/proc/agents/shell/keynode/dlg_button",
	"/proc/agents/shell/keynode/dlg_static",
	"/proc/agents/shell/keynode/dlg_name",
	"/proc/agents/shell/keynode/dialog_create",
	"/proc/agents/shell/keynode/dialog_do",
	"/proc/agents/shell/keynode/sheet_create",
	"/proc/agents/shell/keynode/open_sheet",
	"/proc/agents/shell/keynode/output",
	"/proc/agents/shell/keynode/output_new_sheet",
	"/proc/agents/shell/keynode/module_",
	"/proc/agents/shell/keynode/have_inc_msg",
	"/proc/agents/shell/keynode/user_",
	"/proc/agents/shell/keynode/sheet_",
	"/proc/agents/shell/keynode/SCgSheet",
	"/proc/agents/shell/keynode/SCg3DSheet",
	"/proc/agents/shell/keynode/SCsoSheet",
	"/proc/agents/shell/keynode/SCsSheet",
	"/proc/agents/shell/keynode/SChSheet",
	"/proc/agents/shell/keynode/AnimationSheet",
	"/proc/agents/shell/keynode/TreeSheet",
	"/proc/agents/shell/keynode/AgentSheet",
	"/proc/agents/shell/keynode/GeometrySheet",
	"/proc/agents/shell/keynode/NLSheet",
	"/proc/im_keynode/login",
	"/proc/im_keynode/logout",
	"/proc/im_keynode/usr",
	"/proc/im_keynode/name_",
	"/proc/im_keynode/im_",
	"/proc/im_keynode/IM",
	"/proc/im_keynode/cws_",
	"/proc/im_keynode/pm_",
	"/proc/im_keynode/ui_events_",
	"/proc/im_keynode/ui_event",
	"/proc/im_keynode/ui_event_",
	"/proc/im_keynode/window_",
	"/proc/im_keynode/menu_",
	"/proc/im_keynode/toolbar_",
	"/proc/im_keynode/image_",
	"/proc/im_keynode/dialog_",
	"/proc/im_keynode/push_button_",
	"/proc/im_keynode/check_button_",
	"/proc/im_keynode/radio_button_",
	"/proc/im_keynode/group_",
	"/proc/im_keynode/static_",
	"/proc/im_keynode/editbox_",
	"/proc/im_keynode/ui_eventClass",
	"/proc/im_keynode/eventListener_",
	"/proc/im_keynode/selected_elements_on_sheet",
	"/proc/im_keynode/selected_element_list_on_sheet",
	"/proc/im_keynode/single_selected_element",
	"/proc/im_keynode/send_user",
	"/proc/im_keynode/sheet_content",
	"/proc/im_keynode/sheet",
	"/proc/im_keynode/listener_class",
	"/proc/im_keynode/listener_class_",
	"/proc/im_keynode/scp_process_listener",
	"/proc/im_keynode/sheettype",
	"/proc/im_keynode/vk_control",
	"/proc/im_keynode/ui_sheetEvTemplate",
	"/proc/im_keynode/ui_typeSheetEvTemplate_",
	"/home/shell/profile/shell_mailbox",
	"/info/rel",
	"/info/attr",
	"/info/conn_bin",
	"/info/set",
	"/info/tuple",
	"/info/subj",
	"/proc/mail/login_mailbox",
	"/proc/keynode/markup_",
	"/proc/keynode/root_",
	"/proc/keynode/default_",
	"/proc/keynode/style_",
	"/proc/keynode/vk_shift_",
	"/proc/scon_keynodes/synonymy",
	"/proc/scon_keynodes/main_",
	"/proc/scon_keynodes/title_",
	"/proc/scon_keynodes/def_",
	"/proc/scon_keynodes/nearSem",
	"/proc/scon_keynodes/setInclude",
	"/proc/scon_keynodes/sub_",
	"/proc/scon_keynodes/over_",
	"/proc/scon_keynodes/example_",
	"/proc/scon_keynodes/setPartition",
	"/proc/scon_keynodes/whole_",
	"/proc/scon_keynodes/parts_",
	"/proc/scon_keynodes/setUnion",
	"/proc/scon_keynodes/setIntersection",
	"/proc/scon_keynodes/result_",
	"/proc/scon_keynodes/sets_",
	"/proc/scon_keynodes/existSetIntersection",
	"/proc/scon_keynodes/existStrictSetIntersection",
	"/proc/scon_keynodes/noSetIntersection",
	"/proc/scon_keynodes/setEqual",
	"/proc/scon_keynodes/fctrSet",
	"/proc/scon_keynodes/signComment",
	"/proc/scon_keynodes/comment_",
	"/proc/scon_keynodes/keySign",
	"/proc/scon_keynodes/keySign_",
	"/proc/scon_keynodes/keySign2_",
	"/proc/scon_keynodes/keySign3_",
	"/proc/scon_keynodes/signDefinition",
	"/proc/scon_keynodes/signComparision",
	"/proc/scon_keynodes/signDefinitionSC",
	"/proc/scon_keynodes/idtfComment",
	"/proc/scon_keynodes/idtf_",
	"/proc/scon_keynodes/elemIdtfComment",
	"/proc/scon_keynodes/elem_",
	"/proc/scon_keynodes/relScheme",
	"/proc/scon_keynodes/attribute_",
	"/proc/scon_keynodes/relation_",
	"/proc/scon_keynodes/fieldDef",
	"/proc/scon_keynodes/rel_",
	"/proc/scon_keynodes/field_",
	"/proc/scon_keynodes/titleBibliographic_",
	"/proc/scon_keynodes/document_",
	"/proc/scon_keynodes/author",
	"/proc/scon_keynodes/author_",
	"/proc/scon_keynodes/docInclude_",
	"/proc/scon_keynodes/basicDocumentFragmentation",
	"/proc/scon_keynodes/basicSequence_",
	"/proc/scon_keynodes/documentFragmentsSet_",
	"/proc/scon_keynodes/additionalFragment_",
	"/proc/scon_keynodes/next_",
	"/proc/scon_keynodes/documentFragmentation_",
	"/proc/scon_keynodes/sequence_",
	"/proc/scon_keynodes/docSemComment",
	"/proc/scon_keynodes/docSyntComment",
	"/proc/scon_keynodes/docLocComment",
	"/proc/scon_keynodes/reference",
	"/proc/scon_keynodes/from_",
	"/proc/scon_keynodes/to_",
	"/proc/scon_keynodes/referenceLoc",
	"/proc/scon_keynodes/referenceBibl",
	"/proc/scon_keynodes/pages_",
	"/proc/scon_keynodes/semEqual",
	"/proc/scon_keynodes/SCs-language",
	"/proc/scon_keynodes/SCg-language",
	"/proc/scon_keynodes/RussianLanguage",
	"/proc/scon_keynodes/EnglishLanguage",
	"/proc/scon_keynodes/statProof",
	"/proc/scon_keynodes/statement_",
	"/proc/scon_keynodes/proof_",
	"/proc/scon_keynodes/consequence",
	"/proc/scon_keynodes/consequence_",
	"/proc/scon_keynodes/parts",
	"/proc/scon_keynodes/eBooks",
	"/proc/scon_keynodes/class_",
	"/proc/scon_keynodes/classNotions",
	"/proc/scon_keynodes/notion_",
	"/proc/scon_keynodes/eBook",
	"/proc/scon_keynodes/eBook_",
	"/proc/scon_keynodes/terms_",
	"/proc/scon_keynodes/tasks_",
	"/proc/scon_keynodes/examples",
	"/proc/scon_keynodes/комментарий",
	"/proc/scon_keynodes/определение",
	"/proc/scon_keynodes/рисунок",
	"/proc/scon_keynodes/утверждение",
	"/proc/scon_keynodes/фактографическое высказывание",
	"/proc/scon_keynodes/доказательство",
	"/proc/scon_keynodes/библиографическая ссылка",
	"/proc/scon_keynodes/раздел",
	"/proc/scon_keynodes/семантический электронный учебник",
	"/proc/scon_keynodes/классификация",
	"/proc/scon_keynodes/аннотация",
	"/proc/keynode/elem_",
	"/proc/keynode/mime_type",
	"/proc/keynode/mime_",
	"/proc/keynode/startup_",
	"/proc/keynode/Scheduler",
	"/proc/agents/shell/keynode/send",
	"/proc/keynode/num_attr",
	"/proc/keynode/set_num_attr",
	"/proc/keynode/segc_num_attr"
};

sc_addr pm_keynodes[sizeof(pm_idtfs) / sizeof(const char*)];

sc_segment *pm_keynodes_segment;
sc_addr pm_numattr[NUMATTR_CNT];
sc_addr pm_num_segc_attrs[NUMATTR_CNT];
sc_addr pm_num_set_attrs[NUMATTR_CNT];

sc_addr create_keynode(sc_session *s,const char *idtf)
{
	sc_segment *seg;
	sc_string dn = dirname(idtf);
	assert(idtf);
	seg = s->open_segment(dn);
	if (!seg)
		seg = create_segment_full_path(s,dirname(idtf));
	sc_addr rv;
	if (!(rv = s->find_by_idtf(basename(sc_string(idtf)),seg))) {
		rv = s->create_el(seg,SC_N_CONST);
		if (basename(sc_string(idtf)).size())
			if (s->set_idtf(rv,basename(idtf)))
				SC_ABORT();
	}
	return rv;
}

void create_keynodes(sc_session *s, int keynodes_count, const char *keynodes_uris[], sc_addr keynodes[])
{
	for (int i = 0; i < keynodes_count; i++)
		keynodes[i] = create_keynode(s, keynodes_uris[i]);
}

void	pm_keynodes_init(sc_session *system)
{
#define CHECK_ERROR() \
	do {\
		if (system->get_error()) \
			SC_ABORT();\
	} while (0)
	sc_uint i;
	static char idtf_buffer[256];
	sc_segment *seg = system->create_segment("/proc/keynode");
	pm_keynodes_segment = seg;

	for (i = 0; i < sizeof(pm_keynodes) / sizeof(sc_addr); i++)
		pm_keynodes[i] = create_keynode(system,pm_idtfs[i]);

	for (i = 0; i < NUMATTR_CNT; i++) {
		sprintf(idtf_buffer,"/proc/keynode/%u_",i);
		pm_numattr[i] = create_keynode(system, idtf_buffer);
		system->set_content(pm_numattr[i], Content::integer(i));
		system->gen3_f_a_f(NUM_ATTR, 0, seg, SC_A_CONST|SC_POS, pm_numattr[i]);
	}

	for (i = 0; i < NUMATTR_CNT; i++) {
		sprintf(idtf_buffer,"/proc/keynode/segc_%u_",i);
		pm_num_segc_attrs[i] = create_keynode(system, idtf_buffer);
		system->set_content(pm_num_segc_attrs[i], Content::integer(i));
		system->gen3_f_a_f(SEGC_NUM_ATTR, 0, seg, SC_A_CONST|SC_POS, pm_num_segc_attrs[i]);
	}

	for (i = 0; i < NUMATTR_CNT; i++) {
		sprintf(idtf_buffer,"/proc/keynode/set%u_",i);
		pm_num_set_attrs[i] = create_keynode(system, idtf_buffer);
		system->set_content(pm_num_set_attrs[i], Content::integer(i));
		system->gen3_f_a_f(SET_NUM_ATTR, 0, seg, SC_A_CONST|SC_POS, pm_num_set_attrs[i]);
	}

	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,CONST_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,VAR_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,METAVAR_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,POS_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,NEG_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,FUZ_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,ARC_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,NODE_);
	CHECK_ERROR();
	system->gen3_f_a_f(TYPEATTRS,0,seg,
			SC_A_CONST|SC_POS,UNDF_);
	CHECK_ERROR();
#undef CHECK_ERROR
}
