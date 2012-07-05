# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestFloat32Array < TypeArrayTestCase
  def test_size
    assert_equal 4, Float32Array::BYTES_PER_ELEMENT
  end

  def test_constructor
    buf = ArrayBuffer.new(1024)

    assert_raises RangeError do
      Float32Array.new(buf, 3)
    end

    ary = Float32Array.new(128)
    assert_equal 128, ary.length
    assert_equal 512, ary.byte_length
    assert_equal 0, ary.byte_offset
  end

  def test_index
    buf = ArrayBuffer.new(100)

    ary = Float32Array.new(buf)
    ary[0] = 0.775
    ary[2] = 55.77
    ary[3] = -55.77
    assert_in_delta 0.775, ary[0], 0.01
    assert_equal 0, ary[1]
    assert_equal 0, ary[10]
    assert_in_delta 55.77, ary[2], 0.01
    assert_in_delta(-55.77, ary[3], 0.01)
  end
end