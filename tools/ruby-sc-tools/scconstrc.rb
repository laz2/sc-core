require 'set'
require 'stringio'

# Print comma separated array elements
def ary_to_s(ary)
  s = StringIO.new

  ary.each_with_index do |obj, i|
    s << obj.to_s
    s << ', ' if ary.size != i + 1
  end

  s.string
end

module SCConstraintCompiler
  PARAM_TYPES = [:sc_type, :sc_addr!, :sc_addr_0, :sc_segment, :sc_boolean]

  @constraints = {}
  @filters = {}
  @functions = {}

  def self.valid_name?(name)
    name.is_a?(Symbol) || name.is_a?(String)
  end

  def self.assert_valid_name!(name, type)
    raise "'#{name}' is invalid #{type} name." unless valid_name? name
  end

  def self.assert_var_name!(name)
    assert_valid_name! name, 'variable'
  end

  def self.assert_constr_name!(name)
    assert_valid_name! name, 'constraint'
  end

  def self.assert_func_name!(name)
    assert_valid_name! name, 'function'
  end

  def self.assert_filter_name!(name)
    assert_valid_name! name, 'filter'
  end

  class Constraint
    attr_reader :name
    attr_reader :params
    attr_reader :vars
    attr_reader :code # array of instructions
    attr_reader :return_with # array of variables

    # Parameter is pair of type symbol and name symbol.
    class Param
      attr_reader :name
      attr_reader :type

      def initialize(name, type)
        @name = name
        @type = type
      end

      def to_s
        "#@type #@name"
      end
    end

    class InputInstr
      attr_reader :name
      attr_reader :input

      def initialize(name, input, output)
        @name = name
        @input = input
      end
    end

    class InputOutputInstr < InputInstr
      attr_reader :output

      def initialize(name, input, output)
        super
        @output = output
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

    class FilterInstr < InputInstr
      def to_s
        "filter #@name #{ary_to_s(@input)}"
      end
    end

    def initialize(name, params)
      @name = name
      @params = params
      @vars = Set.new
      @code = []
      @return_with = []

      var :_
      var *@params.map { |p| p.name }
    end

    def var(*names)
      names.each do |name|
        SCConstraintCompiler.assert_var_name! name
        raise "Variable '#{name}' is already defined." if @vars.member? name

        instance_variable_set '@' + name.to_s, name
        @vars << name
      end
    end

    def constr(name, params)
      c = SCConstraintCompiler.constr name

      raise "Constraint '#{c}' doesn't exist." unless c
      raise "Size of parameters hash is #{params.size}. Must be 1." if params.size != 1

      pair = params.first
      i = ConstrInstr.new name, alone_sym_to_ary(pair[0]), alone_sym_to_ary(pair[1])
      @code << i
    end

    def func(name, params)
    end

    def filter(name, *params)
    end

    def return_with(*vars)
      vars.each do |name|
        assert_var! name
        @return_with << name
      end
    end

    def var?(name)
      @vars.member? name
    end

    def to_s
      s = StringIO.new
      s << "constraint #{@name}(#{ary_to_s(@params)})";

      unless @code.empty?
        s << "\n{\n"
        @code.each { |i| s << "\t" << i << "\n" }
        s << "\t" << 'return ' << ary_to_s(@return_with) << "\n"
        s << '}'
      end

      s.string
    end

    def inspect
      "constraint(#{name})"
    end

    private
    def assert_var!(name)
      SCConstraintCompiler.assert_var_name! name
      raise "Variable '#{name}' doesn't exist'" unless var? name
    end

    def alone_sym_to_ary(obj)
      if obj.is_a? Symbol
        [obj]
      else
        obj
      end
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

    if body
      constr.instance_eval &body
    else

    end

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

  def self.dump
    @functions.each_value { |o| puts o }
    puts
    @filters.each_value { |o| puts o }
    puts
    @constraints.each_value { |o| puts o, "\n" }
  end

  module DSL
    def def_constr(name, *params, &body)
      SCConstraintCompiler.def_constr name, *params, &body
    end

    def def_filter(name)
      SCConstraintCompiler.def_filter name
    end

    def def_func(name)
      SCConstraintCompiler.def_func name
    end

    # Define DSL-methods for creating parameters of each type.
    SCConstraintCompiler::PARAM_TYPES.each do |type|
      define_method type do |name|
        SCConstraintCompiler::Constraint::Param.new name, type
      end
    end
  end
end

include SCConstraintCompiler::DSL

def_filter :check_type
def_filter :equal2
def_filter :equal_with_d
def_filter :is_in_set
def_filter :differ
def_filter :not_in_set

def_func :get_beg
def_func :get_end
def_func :get_arc_const_pos
def_func :get_arc_neg_const
def_func :get_0

def_constr :all_input, sc_addr!(:e1)
def_constr :all_output, sc_addr!(:e1)

def_constr :arc_type_input, sc_type(:t1), sc_addr!(:e2) do
  var :r1
  constr :all_output, [:e2] => [:_, :r1]
  filter :check_type, :r1, :t1
  return_with :r1, :e2
end

def_constr :arc_type_output, sc_addr!(:e1), sc_type(:t2) do
  var :r2
  constr :all_output, [:e1] => [:_, :r2]
  filter :check_type, :r2, :t2
  return_with :e1, :r2
end

def_constr :std3_faa, sc_addr!(:e1), sc_type(:t2), sc_type(:t3) do
  var :e2, :e3
  constr :all_output, [:e1, :t2] => [:_, :e2]
  func :get_end, [:e2] => [:e3]
  filter :check_type, :e3, :t3
  return_with :e1, :e2, :e3
end

def_constr :std3_faf, sc_addr!(:e1), sc_type(:t2), sc_addr!(:e3) do
  var :e2, :ce1
  constr :all_input, [:e3] => [:_, :e2]
  func :get_end, [:e2] => [:ce1]
  filter :equal2, :e1, :ce1
  filter :check_type, :e2, :t2
  return_with :e1, :e2, :e3
end

SCConstraintCompiler.dump
