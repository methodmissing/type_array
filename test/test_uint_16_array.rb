# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestUInt16Array < TypeArrayTestCase
  def test_size
    assert_equal 2, UInt16Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(20)

    assert_raises RangeError do
      UInt16Array.new(buf, 3)
    end

    ary = UInt16Array.new(buf, 4)
    assert_equal 8, ary.length
    assert_equal 16, ary.byte_length
  end

  def test_index
    buf = ArrayBuffer.new(100)

    ary = UInt16Array.new(buf)
    ary[0] = 9
    ary[2] = 19264
    ary[3] = -8
    assert_equal 9, ary[0]
    assert_equal 0, ary[1]
    assert_equal 0, ary[49]
    assert_equal 65528, ary[3]
    assert_equal 19264, ary[2]
  end
end