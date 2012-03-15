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

require './compiler'

module SCConstrCompiler
  # Contains DSL-methods top scope of sc-constraint generator
  module DSL
    def header(text)
      SCConstrCompiler.header = text
    end

    def def_constr(name, *params, &body)
      constr = SCConstrCompiler.def_constr name, *params
      constr_dsl = ConstraintDSL.new constr
      constr_dsl.instance_eval &body if block_given?
    end

    def def_filter(name)
      SCConstrCompiler.def_filter name
    end

    def def_func(name)
      SCConstrCompiler.def_func name
    end

    def footer(text)
      SCConstrCompiler.footer = text
    end

    # Define DSL-methods for creating parameters of each type
    SCConstrCompiler::PARAM_TYPES.each do |type|
      define_method type do |name|
        SCConstrCompiler::Constraint::Param.new name, type, false
      end

      # For fixed parameters
      define_method type.to_s + '!' do |name|
        SCConstrCompiler::Constraint::Param.new name, type, true
      end
    end

    # This class contains DSL-methods inside block for SCConstrCompiler::DSL::def_constr
    class ConstraintDSL
      def initialize(constr)
        @__constr__ = constr
        constr.vars.map { |v| def_dsl_var v }
      end

      def var(*names)
        names.each do |n|
          @__constr__.add_var n
          def_dsl_var n
        end
      end

      def constr(name, params)
        raise "Size of parameters hash is #{params.size}. Must be 1." if params.size != 1

        pair = params.first
        @__constr__.add_constr_instr name, alone_sym_to_ary(pair[0]), alone_sym_to_ary(pair[1])
      end

      def func(name, params)
        raise "Size of parameters hash is #{params.size}. Must be 1." if params.size != 1

        pair = params.first
        @__constr__.add_func_instr name, alone_sym_to_ary(pair[0]), alone_sym_to_ary(pair[1])
      end

      def filter(name, *params)
        @__constr__.add_filter_instr name, params
      end

      def return_with(*vars)
        @__constr__.add_return_instr vars
      end

      private

      def def_dsl_var(name)
        self.class.send(:define_method, name) { name }
      end

      def alone_sym_to_ary(obj)
        obj.is_a?(Symbol) ? [obj] : obj
      end
    end
  end
end
