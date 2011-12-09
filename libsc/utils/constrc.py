# -*- coding: utf-8 -*-
"""
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
"""

'''
@author: Dmitry Lazurkin
'''

from pyparsing import Word, alphanums, delimitedList, Literal, OneOrMore, Or,\
    Optional

#
# example:
# constraint my_constr (sc_addr! e1,sc_type! t1) {
#     var e2,e3;
#     constr all_input e1:,e2
#     filter check_type t1,e2
#     func get_beg e2:e3
#     return e1,e2,e3
# };
#

ident = Word( alphanums + '_' )

constr_name     = ident
constr_arg_type = Or( ["sc_addr", "sc_addr_0", "sc_type", "int", "sc_int", "sc_segment", "boolean", "sc_boolean"] ) \
                    + Optional( Word( '!' ) )
constr_arg_name = ident
constr_arg      = constr_arg_type + constr_arg_name
constr_arg_list = delimitedList( constr_arg )

var_declr       = "var" + delimitedList( ident )
filter_call     = "filter" + ident + delimitedList( ident )
constr_body     = OneOrMore( var_declr ^ filter_call )

constr_def = "constraint" + constr_name + '(' + constr_arg_list + ')' + '{' + constr_body + '}'
constr_def.setResultsName("name")

t = """
constraint my_constr (sc_addr! e1,sc_type! t1) {
    var e2,e3
    filter check_type t1,e2
}
"""

print t, "->", constr_def.parseString(t)

class Constraint(object):

    def __init__(self):
        self.input = []
        self.output = []
        self.vars = set()

class BaseOp(object):

    def __init__(self):
        pass

class SubconstrOp(BaseOp):

    def __init__(self):
        self.name = None
        self.input = []
        self.output = []

class FunctionOp(BaseOp):

    def __init__(self):
        self.name = None
        self.input = []
        self.output = []

class FilterOp(BaseOp):

    def __init__(self):
        self.name = None
        self.input = []
