require './scg'
require 'RMagick'
require 'rvg/rvg'

r = SCg::Root.new
File.open('test/nodes.gwf') do |f|
  r.load_gwf f
end


Magick::RVG::dpi = 72

def paint_struct(canvas, el)
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
      canvas.line -10, 0, 10, 0 # from top-center to bottom-center
    when 'asymmetry'
      canvas.line -10, 0, 10, 0 # from right-center to left-center
    when 'attribute'
      canvas.line -10, 0, 10, 0 # from top-center to bottom-center
      canvas.line 0, -10, 0, 10 # from right-center to left-center
    when 'relation'
      canvas.line -10, -10, 10, 10 # from top-left to bottom-right
      canvas.line -10, 10, 10, -10 # from bottom-left to top-right
    when 'group'
      canvas.line -10, -10, 10, 10 # from top-left to bottom-right
      canvas.line -10, 10, 10, -10 # from bottom-left to top-right
      canvas.line 0, -10, 0, 10 # from right-center to left-center
  end
end

rvg = Magick::RVG.new(500, 500).viewbox(0, 0, 500, 500) do |canvas|
  canvas.background_fill = 'white'

  r.elements.each do |el|
    canvas.g.translate(el.x, el.y) do |canvas|
      canvas.styles fill: 'white', stroke: el.shape_color, stroke_width: 4

      canvas.scale 0.3, 0.3 if el.structure == 'not_define'

      case el.constancy
        when 'const'
          o = canvas.circle 9

          canvas.g do |canvas|
            canvas.styles clip_path: Magick::RVG::ClipPath.new { |path| path.use o }
            paint_struct canvas, el
          end
        when 'var'
          canvas.g.scale(0.9) do |canvas|
            o = canvas.rect 18, 18, -9, -9
            canvas.g do |canvas|
              canvas.styles clip_path: Magick::RVG::ClipPath.new { |path| path.use o }
              paint_struct canvas, el
            end
          end
        when 'meta'
          canvas.g.rotate(45).scale(0.9) do |canvas|
            o = canvas.rect 16, 16, -8, -8

            canvas.g.rotate(-45).scale(0.9) do |canvas|
              canvas.styles clip_path: Magick::RVG::ClipPath.new { |path| path.use o }
              paint_struct canvas, el
            end
          end
      end
    end
  end
end

rvg.draw.write 'test/nodes.png'
