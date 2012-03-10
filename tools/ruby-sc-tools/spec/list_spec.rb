require './list'

describe List do
  it "is empty when first created" do
    l = List.new
    l.should be_empty
    l.size.should eq(0)
    l.first.should be_nil
    l.last.should be_nil
  end

  describe "#<<" do
    before(:each) do
      @l = List.new
    end

    it "pushes one element" do
      @l << 'a'
      @l.size.should eq(1)
      @l.first.should eq(@l.last)
      @l.first.should eq('a')
    end

    it "pushes some elements" do
      @l << 'a' << 'b' << 'c'
      @l.size.should eq(3)
      @l.first.should eq('a')
      @l.last.should eq('c')
    end
  end

  describe "#push" do
    before(:each) do
      @l = List.new
    end

    it "pushes one element" do
      @l.push 'a'
      @l.size.should eq(1)
      @l.first.should eq(@l.last)
      @l.first.should eq('a')
    end

    it "pushes some elements" do
      @l.push 'a', 'b', 'c'
      @l.size.should eq(3)
      @l.first.should eq('a')
      @l.last.should eq('c')
    end
  end

  describe "#pop" do
    before(:each) do
      @l = List.new
      @l << 'a' << 'b' << 'c' << 'd'
    end

    it "pops one element" do
      el = @l.pop
      el.should eq('d')
      @l.size.should eq(3)
      @l.last.should eq('c')
    end

    it "pops all elements by one" do
      @l.pop.should eq('d')
      @l.pop.should eq('c')
      @l.pop.should eq('b')
      @l.pop.should eq('a')
      @l.pop.should be_nil
      @l.should be_empty
    end

    it "pops one element as array" do
      @l.pop(1).should eq(['d'])
      @l.size.should eq(3)
    end

    it "pops some elements as array" do
      @l.pop(2).should eq(['c', 'd'])
      @l.size.should eq(2)
    end

    it "pops exact all elements as array" do
      @l.pop(4).should eq(['a', 'b', 'c', 'd'])
      @l.should be_empty
    end

    it "pops grater then all elements as array" do
      @l.pop(5).should eq(['a', 'b', 'c', 'd'])
      @l.should be_empty
    end
  end

  it "iterate each element" do
    l = List.new
    l << 'a' << 'b' << 'c'
    it = l.each
    it.next.should eq('a')
    it.next.should eq('b')
    it.next.should eq('c')
    lambda { it.next }.should raise_error
  end

  it "reverse iterate each element" do
    l = List.new
    l << 'a' << 'b' << 'c'
    it = l.reverse_each
    it.next.should eq('c')
    it.next.should eq('b')
    it.next.should eq('a')
    lambda { it.next }.should raise_error
  end

  describe "#unshift" do
    before(:each) do
      @l = List.new
    end

    it "unshifts one element" do
      @l.unshift 'a'
      @l.size.should eq(1)
      @l.first.should eq(@l.last)
      @l.first.should eq('a')
    end

    it "unshifts some elements" do
      @l.unshift 'a', 'b', 'c'
      @l.size.should eq(3)
      @l.first.should eq('a')
      @l.last.should eq('c')
    end
  end

  describe "#shift" do
    before(:each) do
      @l = List.new
      @l << 'a' << 'b' << 'c' << 'd'
    end

    it "shifts one element" do
      el = @l.shift
      el.should eq('a')
      @l.size.should eq(3)
      @l.first.should eq('b')
    end

    it "shifts all elements by one" do
      @l.shift.should eq('a')
      @l.shift.should eq('b')
      @l.shift.should eq('c')
      @l.shift.should eq('d')
      @l.shift.should be_nil
      @l.should be_empty
    end

    it "shifts one element as array" do
      @l.shift(1).should eq(['a'])
      @l.size.should eq(3)
    end

    it "shifts some elements as array" do
      @l.shift(2).should eq(['a', 'b'])
      @l.size.should eq(2)
    end

    it "shifts exact all elements as array" do
      @l.shift(4).should eq(['a', 'b', 'c', 'd'])
      @l.should be_empty
    end

    it "shifts greater then all elements as array" do
      @l.shift(5).should eq(['a', 'b', 'c', 'd'])
      @l.should be_empty
    end
  end
end
