# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestDataView < TypeArrayTestCase
  def test_type
    assert_instance_of Class, DataView
  end

  def test_constructor
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_instance_of DataView, view
    assert_equal 100, view.byte_length
    assert_equal 0, view.byte_offset
    assert_equal buf, view.buffer
  end

  def test_get_set
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_int8(1)
    view.set_int8(1, 5)
    assert_equal 5, view.get_int8(1)
    view.set_int8(1, 9)
    assert_equal 9, view.get_int8(1)    

    view.set_int16(1, 11520)
    assert_equal 11520, view.get_int16(1)
    assert_equal 0, view.get_int8(1)

    view.set_int32(1, 200000000)
    assert_equal 200000000, view.get_int32(1)
    assert_equal(-15872, view.get_int16(1))
    assert_equal 49664, view.get_uint16(1)

    view.set_float32(1, 0.663)
    assert_equal 0.663, view.get_float32(1)
  end
end