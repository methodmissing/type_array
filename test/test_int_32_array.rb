# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestInt32Array < TypeArrayTestCase
  def test_size
    assert_equal 4, Int32Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(8)

    ary = Int32Array.new(buf)
    assert_equal 2, ary.length

    buf = ArrayBuffer.new(100)

    ary = Int32Array.new(buf)
    assert_equal 25, ary.length
    assert_equal 100, ary.byte_length

    ary = Int16Array.new(buf, 20, 4)
    assert_equal 4, ary.length
    assert_equal 8, ary.byte_length
    assert_equal 20, ary.byte_offset
  end

  def test_index
    buf = ArrayBuffer.new(100)

    ary = Int32Array.new(buf)
    ary[0] = 112
    ary[3] = 658
    assert_equal 112, ary[0]
    assert_equal 112, ary[0]
    assert_equal 658, ary[3]
    assert_equal 658, ary[3]
    assert_equal 0, ary[1]
  end
end