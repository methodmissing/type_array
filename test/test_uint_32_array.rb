# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestUInt32Array < TypeArrayTestCase
  def test_size
    assert_equal 4, UInt32Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(20)

    assert_raises RangeError do
      UInt32Array.new(buf, 2)
    end

    ary = UInt32Array.new(buf, 4)
    assert_equal 4, ary.length
    assert_equal 16, ary.byte_length
  end

  def test_index
    buf = ArrayBuffer.new(100)

    ary = UInt32Array.new(buf)
    ary[0] = 77
    assert_equal 77, ary[0]
    assert_equal 77, ary[0]
    assert_equal 0, ary[1]
    assert_equal 0, ary[1]
    assert_equal 0, ary[24]
  end
end