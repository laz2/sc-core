#--
# This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
# For the latest info, see http://www.ostis.net
#
# Copyright (c) 2012 OSTIS
#
# OSTIS is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# OSTIS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
#++

require 'erb'
require 'set'
require 'stringio'

require './compiler'

module SCConstrCompiler
  module Generator
    TYPE_MAPPING = {
        :sc_type => 'SC_PD_TYPE',
        :sc_addr => 'SC_PD_ADDR',
        :sc_addr_0 => 'SC_PD_ADDR_0'
    }

    INSTR_MAPPING = {
        SCConstrCompiler::Constraint::ConstrInstr => 'SCD_CONSTR',
        SCConstrCompiler::Constraint::FuncInstr => 'SCD_FUNC',
        SCConstrCompiler::Constraint::FilterInstr => 'SCD_FILTER',
        SCConstrCompiler::Constraint::ReturnInstr => 'SCD_RETURN',
    }

    GENERATORS = Set['Structs']

    def self.process(gen)
      raise "Generator '#{gen}' isn't found." unless GENERATORS.member? gen.to_s

      helper = const_get(gen.to_s + 'Helper')

      append_features helper
      puts instance_methods

      template = gen.to_s + '_template.erb'
      File.open(template) do |f|
        t = ERB.new f.read, nil, '<>'
        t.result(binding)
      end
    end

    module StructsHelper
      def self.ary_to_s_with_params(constr, ary)
        s = StringIO.new

        ary.each_with_index do |el, i|
          s << el << ' /* ' << constr.vars[i + 1] << ' */'
          s << ', ' unless i + 1 == ary.size
        end

        s.string
      end

      def self.instr_params_to_s_with_vars(constr, params)
        s = StringIO.new

        params.each_with_index do |el, i|
          s << constr.var2index[el] - 1 << ' /* ' << el << ' */'
          s << ', ' unless i + 1 == params.size
        end

        s.string
      end
    end
  end
end
