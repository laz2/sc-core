require 'rexml/document'

module SCg
  class Sign
    attr_reader :idtf
    attr_reader :views

    def initialize(idtf)
      @idtf = idtf
      @views = []
    end

    def add_view(el)
      @views << el
    end
  end

  class Element
    attr_reader :sign
    attr_reader :parents
    attr_reader :children
    attr_reader :sys_id
    attr_reader :shape_color

    def initialize(sign)
      @sign = sign
      @parents = []
      @children = []
      @shape_color = 0

      sign.add_view self
    end

    def read_state(tag, refs)
      @sys_id = tag.attributes['id'].to_i
      @shape_color = tag.attributes['shapeColor']
      add_needed_ref refs, tag, 'parent'
    end

    def refs=(refs)
      parent = refs['parent']
      if parent
        @parents << parent
        parent.children << self
      end
    end

    def to_s
      "#{self.class.name}<#{@sign.idtf}, #{@sys_id}, #{self.to_s_impl}>"
    end

    private
    def add_needed_ref(refs, tag, name)
      id = tag.attributes[name].to_i
      refs[name] = id if id != 0
    end
  end

  class Node < Element
    attr_reader :x
    attr_reader :y
    attr_reader :constancy
    attr_reader :structure

    def read_state(tag, refs)
      super

      comps = tag.attributes['type'].split '/'
      @constancy = comps[1]
      @structure = comps[2]

      @x = tag.attributes['x'].to_f
      @y = tag.attributes['y'].to_f
    end

    protected
    def to_s_impl
      "#@type, x=#@x, y=#@y"
    end
  end

  class Bus < Element
    def read_state(tag, refs)
      super

      owner_id = tag.attributes['type'].to_i
      add_needed_ref refs, tag, 'owner' unless owner_id == 0
    end

    def refs=(refs)
      super

      @owner = refs['owner']
      @sign = @owner.sign
    end

    protected
    def to_s_impl
      "#@owner"
    end
  end

  class Contour < Element
  end

  class Pair < Element
    def read_state(tag, refs)
      super

      @type = tag.attributes['type']

      add_needed_ref refs, tag, 'id_b'
      add_needed_ref refs, tag, 'id_e'
    end

    def refs=(refs)
      super

      @begin = refs['id_b']
      @end = refs['id_e']
    end

    def to_s_impl
      "#@begin, #@end"
    end
  end

  class Root
    attr_reader :elements

    TAG_TO_CLASS = {
        'node' => Node,
        'bus' => Bus,
        'contour' => Contour,
        'arc' => Pair,
        'pair' => Pair
    }

    def initialize
      @idtf2ident = {}
      @elements = []
    end

    def load_gwf(input)
      el2needed_refs = {}
      id2el = {}

      doc = REXML::Document.new(input)
      doc.root.elements['staticSector'].elements.each do |tag|
        idtf = tag.attributes['idtf']
        sign = get_sign_for_idtf idtf

        el = TAG_TO_CLASS[tag.name].new sign

        # Read SCg-element state
        needed_refs = {}
        el.read_state tag, needed_refs

        el2needed_refs[el] = needed_refs
        id2el[el.sys_id] = el

        @elements << el
      end

      # Set needed references for SCg-elements
      el2needed_refs.each do |el, needed_refs|
        act_refs = {}
        needed_refs.each { |ref, sys_id| act_refs[ref] = id2el[sys_id] }
        el.refs = act_refs
      end
    end

    private
    def get_sign_for_idtf(idtf)
      if idtf.empty?
        sign = Sign.new ''
      else
        sign = @idtf2ident[idtf]
        unless sign
          sign = Sign.new idtf
          @idtf2ident[idtf] = sign
        end
      end

      sign
    end
  end
end

