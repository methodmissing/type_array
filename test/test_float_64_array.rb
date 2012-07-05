# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestFloat64Array < TypeArrayTestCase
  def test_size
    assert_equal 8, Float64Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(1024)

    assert_raises RangeError do
      Float64Array.new(buf, 3)
    end

    ary = Float64Array.new(128)
    assert_equal 128, ary.length
    assert_equal 1024, ary.byte_length
    assert_equal 0, ary.byte_offset
  end

  def test_index
    buf = ArrayBuffer.new(100)

    ary = Float64Array.new(buf)
    ary[0] = 77.643
    assert_equal 77.643, ary[0]
    ary[0] = 55.333
    ary[3] = 323233.77
    ary[4] = -323233.77
    assert_equal 55.333, ary[0]
    assert_equal 0, ary[1]
    assert_equal 0, ary[10]
    assert_equal 323233.77, ary[3]
    assert_equal(-323233.77, ary[4])
  end
end