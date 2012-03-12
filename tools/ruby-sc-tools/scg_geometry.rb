#ENV['debug_prim'] = 'yes'
#ENV['debug_rvg'] = 'yes'
require './scg'
require 'RMagick'
require 'rvg/rvg'

module SCg
  module Geometry
    def self.draw(root)
      Magick::RVG::dpi = 72

      rvg = Magick::RVG.new(500, 500).viewbox(0, 0, 2000, 2000) do |canvas|
        canvas.background_fill = 'white'

        root.elements.each do |el|
          canvas.g { |canvas| draw_el canvas, el }
        end
      end

      rvg
    end

    def self.draw_el(canvas, el)
      klass_sym = el.class.name.split("::").last.to_sym

      raise "Not found SCg-geometry for #{el.class}" unless const_defined? klass_sym

      klass = const_get klass_sym
      klass.draw canvas, el
    end

    class Node
      def self.draw(canvas, el)
        canvas.translate(el.x, el.y)

        canvas.styles fill: 'white', stroke: el.shape_color, stroke_width: 4

        canvas.scale 0.3, 0.3 if el.structure == 'not_define'

        case el.constancy
          when 'const'
            o = canvas.circle 9

            canvas.g do |canvas|
              canvas.styles clip_path: Magick::RVG::ClipPath.new { |path| path.use o }
              draw_struct canvas, el
            end
          when 'var'
            canvas.g.scale(0.9) do |canvas|
              o = canvas.rect 18, 18, -9, -9
              canvas.g do |canvas|
                canvas.styles clip_path: Magick::RVG::ClipPath.new { |path| path.use o }
                draw_struct canvas, el
              end
            end
          when 'meta'
            canvas.g.rotate(45).scale(0.9) do |canvas|
              o = canvas.rect 16, 16, -8, -8

              canvas.g.rotate(-45) do |canvas|
                canvas.styles clip_path: Magick::RVG::ClipPath.new { |path| path.use o }
                draw_struct canvas, el
              end
            end
        end
      end

      def self.draw_struct(canvas, el)
        canvas.styles stroke_width: 2

        case el.structure
          when 'not_define', 'general_node', 'atom'
            # Leave blank
          when 'nopredmet'
            canvas.styles fill: 'black'
            r = (20 / 10) / 2
            canvas.circle r
          when 'predmet'
            y = -8
            while y < 8
              canvas.line -10, y, 10, y # from right-center to left-center
              y += 3
            end
          when 'symmetry'
            canvas.line 0, -10, 0, 10 # from top-center to bottom-center
          when 'asymmetry'
            canvas.line -10, 0, 10, 0 # from right-center to left-center
          when 'attribute'
            canvas.line 0, -10, 0, 10 # from top-center to bottom-center
            canvas.line -10, 0, 10, 0 # from right-center to left-center
          when 'relation'
            canvas.line -10, -10, 10, 10 # from top-left to bottom-right
            canvas.line -10, 10, 10, -10 # from bottom-left to top-right
          when 'group'
            canvas.line -10, -10, 10, 10 # from top-left to bottom-right
            canvas.line -10, 10, 10, -10 # from bottom-left to top-right
            canvas.line -10, 0, 10, 0 # from right-center to left-center
        end
      end
    end

    class Pair
      def self.draw(canvas, el)
        canvas.line el.begin.x, el.begin.y, el.end.x, el.end.y
      end
    end
  end
end

r = SCg::Root.new
File.open('test/nodes_test2.gwf') do |f|
  r.load_gwf f
end

rvg = SCg::Geometry.draw r
rvg.draw.write 'test/test.png'
