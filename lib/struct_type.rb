class StructType < Struct
  BYTES_PER_ELEMENT = 0

  class << self
    attr_reader :type_map
  end

  { :int8 => Int8Array,
    :uint8 => UInt8Array,
    :int16 => Int16Array,
    :uint16 => UInt16Array,
    :int32 => Int32Array,
    :uint32 => UInt32Array,
    :float32 => Float32Array,
    :float64 => Float64Array }.each do |type, klass|
    class_eval <<-doc
      def self.#{type}(member)
        (@type_map ||= {})[member.to_sym] = :#{type}
        const_set(:BYTES_PER_ELEMENT, const_get(:BYTES_PER_ELEMENT) + #{klass.const_get(:BYTES_PER_ELEMENT)})
      end
    doc
  end

  def self.new
    Struct.new(*([self.name].concat(type_map.keys)))
  end

  def type_map
    self.class.type_map
  end
end