# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestInt8Array < TypeArrayTestCase
  def test_size
    assert_equal 1, Int8Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(8)

    ary = Int8Array.new(buf)
    assert_equal 8, ary.length
    assert_equal 8, ary.byte_length
  end

  def test_index
    buf = ArrayBuffer.new(8)

    ary = Int8Array.new(buf)
    ary[0] = 3
    ary[2] = -3
    assert_equal 3, ary[0]
    assert_equal 0, ary[1]
    assert_equal 0, ary[7]
    assert_equal(-3, ary[2])
  end
end