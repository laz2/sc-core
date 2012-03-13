require './dsl'

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

#def_constr :arc_type_output, sc_addr!(:e1), sc_type(:t2) do
#  var :r2
#  constr :all_output, [:e1] => [:_, :r2]
#  filter :check_type, :r2, :t2
#  return_with :e1, :r2
#end
#
#def_constr :std3_faa, sc_addr!(:e1), sc_type(:t2), sc_type(:t3) do
#  var :e2, :e3
#  constr :all_output, [:e1, :t2] => [:_, :e2]
#  func :get_end, [:e2] => [:e3]
#  filter :check_type, :e3, :t3
#  return_with :e1, :e2, :e3
#end
#
#def_constr :std3_faf, sc_addr!(:e1), sc_type(:t2), sc_addr!(:e3) do
#  var :e2, :ce1
#  constr :all_input, [:e3] => [:_, :e2]
#  func :get_end, [:e2] => [:ce1]
#  filter :equal2, :e1, :ce1
#  filter :check_type, :e2, :t2
#  return_with :e1, :e2, :e3
#end


require 'erb'


File.open('sc_constraints_as_structs.erb') do |f|
  t = ERB.new f.read, nil, '<>'
  puts t.result(binding)
end

#SCConstraintCompiler.dump
