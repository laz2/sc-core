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

require 'set'
require 'stringio'

def ary_to_s(ary)
  ary.join ', '
end

# Contains sc-constraints compiler model and logic.
#
# Author:: Dmitry Lazurkin
module SCConstrCompiler
  PARAM_TYPES = Set[:sc_type, :sc_addr, :sc_addr_0, :sc_segment, :sc_boolean]

  @constraints = {}
  @filters = {}
  @functions = {}

  class << self
    attr_accessor :header
    attr_reader :constraints
    attr_reader :filters
    attr_reader :functions
    attr_accessor :footer
  end

  def self.valid_name?(name)
    name.is_a?(Symbol) || name.is_a?(String)
  end

  def self.def_assert_obj_name(type_short, type_long)
    define_singleton_method "assert_#{type_short}_name!" do |name|
      raise "'#{name}' is invalid #{type_long} name." unless valid_name? name
    end
  end

  def_assert_obj_name :var, 'variable'
  def_assert_obj_name :constr, 'constraint'
  def_assert_obj_name :func, 'function'
  def_assert_obj_name :filter, 'filter'

  class Constraint
    attr_reader :name
    attr_reader :params
    attr_reader :vars
    attr_reader :var2index
    attr_reader :code # array of instructions

    # Parameter is pair of type symbol and name symbol.
    class Param
      attr_reader :name
      attr_reader :type
      attr_reader :fixed

      def initialize(name, type, fixed)
        @name = name
        @type = type
        @fixed = fixed
      end

      def to_s
        @fixed ? "#{@type}! #@name" : "#{@type} #@name"
      end
    end

    class InputOutputInstr
      attr_reader :parent
      attr_reader :name
      attr_reader :input
      attr_reader :output

      # For input only instructions parameter output must be empty array.
      def initialize(parent, name, input, output)
        @parent = parent
        @name = name
        @input = input
        @output = output
      end

      def validate!
        @input.each { |n| @parent.assert_var! n }
        @output.each { |n| @parent.assert_var! n }
      end
    end

    class ConstrInstr < InputOutputInstr
      def to_s
        "constr #@name #{ary_to_s(@input)} => #{ary_to_s(@output)}"
      end
    end

    class FuncInstr < InputOutputInstr
      def to_s
        "func #@name #{ary_to_s(@input)} => #{ary_to_s(@output)}"
      end
    end

    class FilterInstr < InputOutputInstr
      def initialize(parent, name, input)
        super parent, name, input, []
      end

      def to_s
        "filter #@name #{ary_to_s(@input)}"
      end
    end

    class ReturnInstr < InputOutputInstr
      def initialize(parent, input)
        super parent, nil, input, []
      end

      def to_s
        "return #{ary_to_s @input}"
      end
    end

    def initialize(name, params)
      @name = name
      @params = params
      @vars = []
      @var2index = {}
      @code = []

      add_var :_
      @params.each { |p| add_var p.name }
    end

    def add_var(name)
      SCConstrCompiler.assert_var_name! name
      raise "Variable '#{name}' is already defined." if @vars.member? name

      @vars << name
      @var2index[name] = @vars.size - 1
    end

    def add_constr_instr(name, input, output)
      c = SCConstrCompiler.constr name

      raise "Constraint '#{c}' doesn't exist." unless c

      i = ConstrInstr.new self, name, input, output
      i.validate!
      @code << i
    end

    def add_func_instr(name, params)
      f = SCConstrCompiler.func name

      raise "Function '#{f}' doesn't exist." unless f
      raise "Size of parameters hash is #{params.size}. Must be 1." if params.size != 1

      pair = params.first
      i = FuncInstr.new self, name, alone_sym_to_ary(pair[0]), alone_sym_to_ary(pair[1])
      i.validate!

      @code << i
    end

    def add_filter_instr(name, params)
      f = SCConstrCompiler.filter name

      raise "Filter '#{f}' doesn't exist." unless f

      i = FilterInstr.new self, name, params
      i.validate!

      @code << i
    end

    def add_return_instr(vars)
      i = ReturnInstr.new self, vars
      i.validate!

      @code << i
    end

    def declr_only?
      @code.size == 0
    end

    def var?(name)
      @vars.member? name
    end

    def assert_var!(name)
      SCConstrCompiler.assert_var_name! name
      raise "Variable '#{name}' doesn't exist." unless var? name
    end

    def to_s
      s = StringIO.new
      s << "constraint #{@name}(#{ary_to_s(@params)})";

      unless @code.empty?
        s << "\n{\n"
        @code.each { |i| s << '    ' << i << "\n" }
        s << '}'
      end

      s.string
    end

    def inspect
      "constraint(#{name})"
    end
  end

  class Filter
    attr_reader :name

    def initialize(name)
      @name = name
    end

    def to_s
      "filter #@name"
    end

    def inspect
      "filter(#{name})"
    end
  end

  class Function
    attr_reader :name

    def initialize(name)
      @name = name
    end

    def to_s
      "func #@name"
    end

    def inspect
      "function(#{name})"
    end
  end

  def self.def_constr(name, *params, &body)
    assert_constr_name! name
    raise "Constraint '#{name}' already exists." if @constraints[name]

    constr = Constraint.new name, params
    @constraints[name] = constr

    constr
  end

  def self.def_filter(name)
    assert_filter_name! name
    raise "Filter '#{name}' already exists." if @filters[name]

    filter = Filter.new name
    @filters[name] = filter

    filter
  end

  def self.def_func(name)
    assert_func_name! name
    raise "Function '#{name}' already exists." if @functions[name]

    func = Function.new name
    @functions[name] = func

    func
  end

  def self.constr(name)
    @constraints[name]
  end

  def self.func(name)
    @functions[name]
  end

  def self.filter(name)
    @filters[name]
  end

  def self.dump
    @functions.each_value { |o| puts o }
    puts
    @filters.each_value { |o| puts o }
    puts
    @constraints.each_value { |o| puts o, "\n" }
  end
end


