require 'pathname'

module SCRepo
  class SourceEntry
    def initialize(parent, src_path)
      @parent   = parent
      @src_path = src_path
    end

    attr_reader :parent
    attr_reader :src_path
    attr_reader :bin_path
  end

  class SourceDirectory < SourceEntry
    def initialize(parent, src_path)
      super

      @children = {}
      @mtime    = Time.new 0
    end

    def update
      cur_mtime = @src_path.mtime
      if cur_mtime > @mtime
        @mtime = cur_mtime

        cur_children = {}
        @src_path.each_child do |path|
          next if path.basename.to_s[0] == '.'

          child = @children.delete path.basename
          unless child
            if path.file?
              child = SourceFile.new self, path
            elsif path.directory?
              child = SourceDirectory.new self, path
            end
          end

          child.update

          cur_children[path.basename] = child
        end

        # TODO
        @children.each

        @children = cur_children
      end
    end
  end

  class SourceFile < SourceEntry
    def update
    end
  end

  class SCsSourceFile < SourceFile
  end

  class M4SCPSourceFile < SourceFile
  end

  class SCgSourceFile < SourceFile
  end
end

dir = SCRepo::SourceDirectory.new nil, Pathname.new('.').realdirpath
dir.update

require 'yaml'
puts YAML::dump(dir)
