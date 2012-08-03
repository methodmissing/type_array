# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class SomeRecord < StructType
  self.int32   :a
  self.float64 :b
end

class TestStructTpe< TypeArrayTestCase
  def test_type_map
    assert_equal :int32, SomeRecord.type_map[:a]
    assert_equal :float64, SomeRecord.type_map[:b]
  end

  def test_members
    r = SomeRecord.new
    assert_equal %w(a b), r.members.map(&:to_s)
  end

  def test_size
    assert_equal 0, StructType::BYTES_PER_ELEMENT
    assert_equal 12, SomeRecord::BYTES_PER_ELEMENT
  end
end
