# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestUInt8Array < TypeArrayTestCase
  def test_size
    assert_equal 1, UInt8Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(8)

    assert_raises RangeError do
      UInt32Array.new(buf, 1)
    end

    ary = UInt8Array.new(buf, 2)
    assert_equal 6, ary.length
    assert_equal 6, ary.byte_length
  end

  def test_index
    buf = ArrayBuffer.new(8)

    ary = UInt8Array.new(buf)
    ary[0] = 5
    assert_equal 5, ary[0]
    assert_equal 0, ary[1]
    assert_equal 0, ary[7]
  end
end