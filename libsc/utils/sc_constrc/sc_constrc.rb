require 'set'
require 'stringio'

def ary_to_s(ary)
  ary.join ', '
end

module SCConstraintCompiler
  PARAM_TYPES = Set[:sc_type, :sc_addr, :sc_addr_0, :sc_segment, :sc_boolean]

  @constraints = {}
  @filters = {}
  @functions = {}

  def self.constraints
    @constraints
  end

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
    attr_reader :var2index
    attr_reader :code # array of instructions
    attr_reader :return_with # array of variables

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
      attr_reader :name
      attr_reader :input
      attr_reader :output

      def initialize(name, input, output)
        @name = name
        @input = input
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

    class FilterInstr < InputOutputInstr
      def to_s
        "filter #@name #{ary_to_s(@input)}"
      end
    end

    def initialize(name, params)
      @name = name
      @params = params
      @vars = []
      @var2index = {}
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
        @var2index[name] = @vars.size - 1
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
      f = SCConstraintCompiler.func name

      raise "Function '#{f}' doesn't exist." unless f
      raise "Size of parameters hash is #{params.size}. Must be 1." if params.size != 1

      pair = params.first
      i = FuncInstr.new name, alone_sym_to_ary(pair[0]), alone_sym_to_ary(pair[1])
      @code << i
    end

    def filter(name, *params)
      f = SCConstraintCompiler.filter name

      raise "Filter '#{f}' doesn't exist." unless f

      pair = params.first
      i = FilterInstr.new name, params, []
      @code << i
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


