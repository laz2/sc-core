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
 * @file nsm_keynodes.h
 * @brief Определение URI ключевых узлов наивгационно-поисковой машины и макросы для доступа к их адресам.
 *
 * Generated from tools/nsm_keynodes.h.in by m4 at  *
 */

#ifndef __NSM_KEYNODES_H__
#define __NSM_KEYNODES_H__

#include <libpm/pm.h>
#include <libsc/sc_types.h>

#define nsm__extern extern
extern const char *nsm_idtfs[];

nsm__extern LIBPM_API
sc_addr	nsm_keynodes[71];

#define	NSM_SHEET_	(nsm_keynodes[0])
#define	NSM_CONFIRM_	(nsm_keynodes[1])
#define	NSM_MAILBOX_HOOK	(nsm_keynodes[2])
#define	NSM_POSSIBLE_COMMANDS	(nsm_keynodes[3])
#define	NSM_DENIED_	(nsm_keynodes[4])
#define	NSM_STORE_IN_ONE_	(nsm_keynodes[5])
#define	NSM_SEARCH_ONLY_ONE_	(nsm_keynodes[6])
#define	NSM_COMMAND	(nsm_keynodes[7])
#define	NSM_COMMAND_	(nsm_keynodes[8])
#define	NSM_COMMAND_PATTERN_	(nsm_keynodes[9])
#define	NSM_COMMAND_ELEM_	(nsm_keynodes[10])
#define	NSM_SHOW_CONTENT_	(nsm_keynodes[11])
#define	NSM_COMMAND_COMMENT_	(nsm_keynodes[12])
#define	NSM_COMMAND_LAYOUT_	(nsm_keynodes[13])
#define	NSM_COMMAND_SHORTNAME_	(nsm_keynodes[14])
#define	NSM_RETURN_ONLY_	(nsm_keynodes[15])
#define	NSM_SEARCHED_	(nsm_keynodes[16])
#define	NSM_NOT_RETURNED_ELEMS_	(nsm_keynodes[17])
#define	NSM_RETURNED_ELEMS_	(nsm_keynodes[18])
#define	NSM_RECURSIVED_ELEMS_	(nsm_keynodes[19])
#define	NSM_GOALS	(nsm_keynodes[20])
#define	NSM_ACTIVE_	(nsm_keynodes[21])
#define	NSM_RESULT	(nsm_keynodes[22])
#define	NSM_CONFIRMED_	(nsm_keynodes[23])
#define	NSM_ADD_UI_INFO_	(nsm_keynodes[24])
#define	NSM_SEARCH_	(nsm_keynodes[25])
#define	NSM_GENERATE_	(nsm_keynodes[26])
#define	NSM_GENERATED_	(nsm_keynodes[27])
#define	NSM_DENY_	(nsm_keynodes[28])
#define	NSM_SEARCH_IN_SEGMENT_	(nsm_keynodes[29])
#define	NSM_CACHE_SHEET	(nsm_keynodes[30])
#define	NSM_REPLY_ON_MSG	(nsm_keynodes[31])
#define	NSM_GEN_MSG	(nsm_keynodes[32])
#define	NSM_SYNONYMS	(nsm_keynodes[33])
#define	NSM_SEARCH_ALL_INPUT_ARCS	(nsm_keynodes[34])
#define	NSM_SEARCH_ALL_INPUT_ATTR	(nsm_keynodes[35])
#define	NSM_SEARCH_ALL_KORT	(nsm_keynodes[36])
#define	NSM_SEARCH_ALL_OUTPUT_ARCS	(nsm_keynodes[37])
#define	NSM_SEARCH_ALL_OUTPUT_ATTR	(nsm_keynodes[38])
#define	NSM_SEARCH_ALL_REL	(nsm_keynodes[39])
#define	NSM_SEARCH_CLASSIFICATION_SCHEME	(nsm_keynodes[40])
#define	NSM_SEARCH_PROGRAM_HEADER	(nsm_keynodes[41])
#define	NSM_USE_MAIN_SYNONIM_	(nsm_keynodes[42])
#define	NSM_SCLIST_PARENT_TOPIC	(nsm_keynodes[43])
#define	NSM_SCLIST_TOPIC_PARTS	(nsm_keynodes[44])
#define	NSM_SCLIST_NEXT_TOPIC	(nsm_keynodes[45])
#define	NSM_SCLIST_PREV_TOPIC	(nsm_keynodes[46])
#define	NSM_SCLIST_TOPIC_TITLE	(nsm_keynodes[47])
#define	NSM_SCLIST_SYNONYMY	(nsm_keynodes[48])
#define	NSM_SCLIST_MAIN_SYNONYM	(nsm_keynodes[49])
#define	NSM_SCLIST_OVER_SET	(nsm_keynodes[50])
#define	NSM_SCLIST_SUB_SET	(nsm_keynodes[51])
#define	NSM_SCLIST_EXAMPLES	(nsm_keynodes[52])
#define	NSM_SCLIST_DEF	(nsm_keynodes[53])
#define	NSM_SCLIST_COMMENT	(nsm_keynodes[54])
#define	NSM_SCLIST_PODRAZDEL	(nsm_keynodes[55])
#define	NSM_SCLIST_DOC_COMMENT	(nsm_keynodes[56])
#define	NSM_SCLIST_REFERENCEbIBL	(nsm_keynodes[57])
#define	NSM_SCLIST_REFERENCE_DOC	(nsm_keynodes[58])
#define	NSM_SCLIST_DOC_TITLE	(nsm_keynodes[59])
#define	NSM_SCLIST_SETUNIOUN	(nsm_keynodes[60])
#define	NSM_SCLIST_SETINTERSECTION	(nsm_keynodes[61])
#define	NSM_SCLIST_EXAMPLES_DOC_COMMENT	(nsm_keynodes[62])
#define	NSM_SCLIST_ALL_SYNONYM	(nsm_keynodes[63])
#define	NSM_MAIN_KEYSIGN_DOCUMENT	(nsm_keynodes[64])
#define	NSM_MAIN_KEYSIGN2_KEYSIGN3	(nsm_keynodes[65])
#define	NSM_MAIN_KEYSIGN2	(nsm_keynodes[66])
#define	NSM_MAIN_KEYSIGN	(nsm_keynodes[67])
#define	NSM_MAILBOX	(nsm_keynodes[68])
#define	NSM_ISOM_GENERATE_	(nsm_keynodes[69])
#define	NSM_PARAMETERS_ (nsm_keynodes[70])

#endif //__NSM_KEYNODES_H__
