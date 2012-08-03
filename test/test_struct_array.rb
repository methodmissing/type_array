# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class OtherRecord < StructType
  self.int32   :a
  self.float64 :b
end

class TestStructArray < TypeArrayTestCase
  def test_size
    assert_equal 0, StructArray::BYTES_PER_ELEMENT
  end

  def test_constructor
    ary = StructArray.new(OtherRecord, 0, 10)

    assert_equal 10, ary.length
    assert_equal 120, ary.byte_length
  end

  def test_undef_operations
    ary = StructArray.new(OtherRecord, 0, 10)

    assert_raises(NoMethodError) do
      ary.mul(1,2)
    end
  end

  def struct_type
    ary = StructArray.new(OtherRecord, 0, 10)
    assert_equal OtherRecord, ary.struct_type
  end
end