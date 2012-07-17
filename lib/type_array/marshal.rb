# encoding: utf-8

module TypeArray::Marshal
  def self.included(klass)
    class << klass
      def _load(str); new(str) end
    end
  end

  def _dump(obj)
    to_s
  end
end