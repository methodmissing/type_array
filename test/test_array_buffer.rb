# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestArrayBuffer < TypeArrayTestCase
  def test_type
    assert_instance_of Class, ArrayBuffer
  end

  def test_constructor
    buf = ArrayBuffer.new(100)
    assert_instance_of ArrayBuffer, buf

   assert_raises TypeError do
     ArrayBuffer.new(:symbol)
   end
  end

  def test_byte_length
    buf = ArrayBuffer.new(100)
    assert_equal 100, buf.byte_length
  end

  def test_slice
    buf = ArrayBuffer.new(100)
    copy = buf.slice(50)
    assert buf.object_id != copy.object_id
    assert_equal 50, copy.byte_length

    copy = copy.slice(25)
    assert_equal 25, copy.byte_length
  end
end