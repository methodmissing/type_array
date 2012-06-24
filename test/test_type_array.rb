# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestTypeArray < TypeArrayTestCase
  def test_type
    assert_instance_of Class, TypeArray
  end

  def test_direct_constructor
    assert_raises TypeError do
      TypeArray.new(100)
    end
  end

  def test_unsupported_constructor
    assert_raises TypeError do
      Int32Array.new(:invalid)
    end
  end

  def test_length_constructor
    ary = Int32Array.new(100)

    assert_instance_of Int32Array, ary
    assert_instance_of ArrayBuffer, ary.buffer
    assert_equal 100, ary.length
    assert_equal 400, ary.byte_length
    assert_equal 0, ary.byte_offset
  end

  def test_array_buf_constructor
    buf = ArrayBuffer.new(100)

    ary = Int32Array.new(buf)
    assert_equal 25, ary.length
    assert_equal 100, ary.byte_length
    assert_equal 0, ary.byte_offset
    assert_equal buf, ary.buffer

    assert_raises TypeError do
      Int32Array.new(buf, :symbol)
    end

    assert_raises RangeError do
      Int32Array.new(buf, 13)
    end
  end

  def test_offset_length_constructor
    buf = ArrayBuffer.new(100)

    ary = Int32Array.new(buf, 20)
    assert_equal 20, ary.length
    assert_equal 80, ary.byte_length
    assert_equal 20, ary.byte_offset

    ary = Int32Array.new(buf, 0, 20)
    assert_equal 20, ary.length
    assert_equal 80, ary.byte_length
    assert_equal 0, ary.byte_offset

    ary = Int32Array.new(buf, 20, 20)
    assert_equal 20, ary.length
    assert_equal 80, ary.byte_length
    assert_equal 20, ary.byte_offset

    assert_raises RangeError do
      Int32Array.new(buf, 84, 20)
    end
  end
end