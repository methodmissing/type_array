# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestInt16Array < TypeArrayTestCase
  def test_size
    assert_equal 2, Int16Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(8)

    ary = Int16Array.new(buf, 2, 2);
    assert_equal 2, ary.length

    buf = ArrayBuffer.new(100)

    ary = Int16Array.new(buf, 20, 4)
    assert_equal 4, ary.length
    assert_equal 8, ary.byte_length
    assert_equal 20, ary.byte_offset
  end

  def test_index
    buf = ArrayBuffer.new(100)

    ary = Int16Array.new(buf)
    ary[0] = 12
    ary[2] = 5000
    ary[12] = -5
    assert_equal 12, ary[0]
    assert_equal 0, ary[1]
    assert_equal 0, ary[49]
    assert_equal(-5, ary[12])
    assert_equal 5000, ary[2]
  end
end