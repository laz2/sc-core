require './sc_constrc'

module SCConstraintCompiler
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

      # Define DSL-methods for creating parameters of each type
      SCConstraintCompiler::PARAM_TYPES.each do |type|
        define_method type do |name|
          SCConstraintCompiler::Constraint::Param.new name, type, false
        end

        # For fixed parameters
        define_method type.to_s + '!' do |name|
          SCConstraintCompiler::Constraint::Param.new name, type, true
        end
      end
    end
end
