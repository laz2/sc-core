class List
  include Enumerable

  class Element

    def initialize(prev_el, next_el, value)
      @prev_el = prev_el
      @next_el = next_el
      @value = value
    end

    attr_accessor :prev_el
    attr_accessor :next_el
    attr_accessor :value
  end

  def initialize()
    @first = nil
    @last = nil
    @size = 0
  end

  def << obj
    el = Element.new(@last, nil, obj)

    if @last
      @last.next_el = el
    else
      @first = el
    end
    @last = el

    @size += 1

    self
  end

  def push(obj, *rest)
    self << obj
    rest.each { |o| self << o }

    self
  end

  def pop(*rest)
    if rest.empty?
      el = pop_el
      el.value if el
    else
      rv = Array.new
      [rest[0], @size].min.times { rv.unshift pop_el.value }
      rv
    end
  end

  def unshift(obj, *rest)
    rest.reverse_each { |obj| unshift_el obj }
    unshift_el obj

    self
  end

  def shift(*rest)
    if rest.empty?
      el = shift_el
      el.value if el
    else
      rv = Array.new
      [rest[0], @size].min.times { rv << shift_el.value }
      rv
    end
  end

  def each
    if block_given?
      cur = @first
      while cur
        yield cur.value
        cur = cur.next_el
      end
    else
      Enumerator.new self, :each
    end
  end

  def reverse_each
    if block_given?
      cur = @last
      while cur
        yield cur.value
        cur = cur.prev_el
      end
    else
      Enumerator.new self, :reverse_each
    end
  end

  def empty?
    @size == 0
  end

  def first
    @first.value if @first
  end

  def last
    @last.value if @last
  end

  attr_reader :size
  alias :length :size

  private
  def pop_el
    if @last
      el = @last

      if (@last = el.prev_el)
        @last.next_el = nil
      else
        @first = nil
      end

      @size -= 1

      el
    end
  end

  def unshift_el(obj)
    el = Element.new(nil, @first, obj)

    if empty?
      @last = el
    else
      @first.prev_el = el
    end
    @first = el

    @size += 1

    self
  end

  def shift_el
    if @first
      el = @first

      if (@first = el.next_el)
        @first.prev_el = nil
      else
        @last = nil
      end

      @size -= 1

      el
    end
  end
end
