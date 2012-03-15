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
  PARAM_TYPES = Set[:sc_type, :sc_addr, :sc_addr_0, :sc_segment, :sc_bool, :sc_int]

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

  def self.raise_with_place!(msg, place)
    if place
      raise RuntimeError, msg, caller.unshift(place)
    else
      raise RuntimeError, msg
    end
  end

  def self.def_assert_obj_name(type_short, type_long)
    define_singleton_method "assert_#{type_short}_name!" do |name, place|
      raise_with_place! "'#{name}' is invalid #{type_long} name.", place unless valid_name? name
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
    attr_reader :place

    # Parameter is pair of type symbol and name symbol.
    class Param
      attr_reader :name
      attr_reader :type
      attr_reader :fixed
      attr_reader :place

      def initialize(name, type, fixed, place = nil)
        @name = name
        @type = type
        @fixed = fixed
        @place = place
      end

      def to_s
        @fixed ? "#{@type}! #@name" : "#{@type} #@name"
      end
    end

    class InputOutputInstr
      attr_reader :parent
      attr_reader :obj
      attr_reader :input
      attr_reader :output

      # For input only instructions parameter output must be empty array.
      def initialize(parent, obj, input, output, place = nil)
        @parent = parent
        @obj = obj
        @input = input
        @output = output
        @place = place
      end

      def validate!
        @input.each { |n| @parent.assert_var! n, @place }
        @output.each { |n| @parent.assert_var! n, @place }
      end
    end

    class ConstrInstr < InputOutputInstr
      def to_s
        "constr #{@obj.name} #{ary_to_s @input} => #{ary_to_s @output}"
      end
    end

    class FuncInstr < InputOutputInstr
      def to_s
        "func #{@obj.name} #{ary_to_s @input} => #{ary_to_s @output}"
      end
    end

    class FilterInstr < InputOutputInstr
      def initialize(parent, name, input, place = nil)
        super parent, name, input, [], place
      end

      def to_s
        "filter #{@obj.name} #{ary_to_s @input}"
      end
    end

    class ReturnInstr < InputOutputInstr
      def initialize(parent, input, place = nil)
        super parent, nil, input, [], place
      end

      def to_s
        "return #{ary_to_s @input}"
      end
    end

    def initialize(name, params, place = nil)
      @name = name
      @params = params
      @vars = []
      @var2index = {}
      @code = []
      @place = place

      add_var :_
      @params.each { |p| add_var p.name, p.place }
    end

    def add_var(name, place = nil)
      SCConstrCompiler.assert_var_name! name, place
      SCConstrCompiler.raise_with_place! "Variable '#{name}' is already defined.", place if @vars.member? name

      @vars << name
      @var2index[name] = @vars.size - 1
    end

    def add_constr_instr(name, input, output, place = nil)
      c = SCConstrCompiler.constr name

      SCConstrCompiler.raise_with_place! "Constraint '#{name}' doesn't exist.", place unless c

      i = ConstrInstr.new self, c, input, output, place
      i.validate!
      @code << i
    end

    def add_func_instr(name, input, output, place = nil)
      f = SCConstrCompiler.func name

      SCConstrCompiler.raise_with_place! "Function '#{name}' doesn't exist.", place unless f

      i = FuncInstr.new self, f, input, output, place
      i.validate!

      @code << i
    end

    def add_filter_instr(name, params, place = nil)
      f = SCConstrCompiler.filter name

      SCConstrCompiler.raise_with_place! "Filter '#{name}' doesn't exist.", place unless f

      i = FilterInstr.new self, f, params, place
      i.validate!

      @code << i
    end

    def add_return_instr(vars, place = nil)
      i = ReturnInstr.new self, vars, place
      i.validate!

      @code << i
    end

    def declr_only?
      @code.size == 0
    end

    def var?(name)
      @vars.member? name
    end

    def assert_var!(name, place)
      SCConstrCompiler.assert_var_name! name, place
      SCConstrCompiler.raise_with_place! "Variable '#{name}' doesn't exist.", place unless var? name
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
    attr_reader :place

    def initialize(name, place)
      @name = name
      @place = place
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
    attr_reader :place

    def initialize(name, place)
      @name = name
      @place = place
    end

    def to_s
      "func #@name"
    end

    def inspect
      "function(#{name})"
    end
  end

  def self.def_constr(name, params, place = nil)
    assert_constr_name! name, place
    SCConstrCompiler.raise_with_place! "Constraint '#{name}' already exists.", place if @constraints[name]

    constr = Constraint.new name, params, place
    @constraints[name] = constr

    constr
  end

  def self.def_filter(name, place = nil)
    assert_filter_name! name, place
    SCConstrCompiler.raise_with_place! "Filter '#{name}' already exists.", place if @filters[name]

    filter = Filter.new name, place
    @filters[name] = filter

    filter
  end

  def self.def_func(name, place = nil)
    assert_func_name! name, place
    SCConstrCompiler.raise_with_place! "Function '#{name}' already exists.", place if @functions[name]

    func = Function.new name, place
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


