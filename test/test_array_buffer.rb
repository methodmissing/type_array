# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestArrayBuffer < TypeArrayTestCase
  def test_type
    assert_instance_of Class, ArrayBuffer
  end

  def test_length_constructor
    buf = ArrayBuffer.new(100)
    assert_instance_of ArrayBuffer, buf

    assert_raises TypeError do
      ArrayBuffer.new(:symbol)
    end

    assert_raises RangeError do
      ArrayBuffer.new(0)
    end
  end

  def test_string_constructor
    str = "01234567"
    buf = ArrayBuffer.new(str)
    assert_equal 8, buf.byte_length

    assert_equal str, buf.to_s
  end

  def test_byte_length
    buf = ArrayBuffer.new(100)
    assert_equal 100, buf.byte_length
  end

  def test_slice
    buf = ArrayBuffer.new(100)

    assert_raises TypeError do
      buf.slice(:invalid)
    end

    assert_raises TypeError do
      buf.slice(50, :invalid)
    end

    assert_raises RangeError do
      buf.slice(101)
    end

    copy = buf.slice(50)
    assert buf.object_id != copy.object_id
    assert_equal 50, copy.byte_length

    copy = copy.slice(25)
    assert_equal 25, copy.byte_length

    copy = buf.slice(-30, -10)
    assert_equal 20, copy.byte_length
  end

  def test_to_s
    buf = ArrayBuffer.new(16)
    str = buf.to_s
    assert_equal "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000", str
    assert str.frozen?
  end

  def test_io
    buf = ArrayBuffer.new(16)

    f = File.open(file_name, File::CREAT | File::RDWR)

    view = DataView.new(buf)
    view.set_int8(0, 5)
    view.set_int8(1, 12)
    view.set_int16(2, 34)
    view.set_float64(3, 8323.32)

    view.write(f)

    f.flush
    assert_equal 16, File.size(file_name)

    f.rewind

    buf = ArrayBuffer.read(f, 16)

    view = DataView.new(buf)
    assert_equal 16, view.byte_length

    assert_equal 5, view.get_int8(0)
    assert_equal 12, view.get_int8(1)
    assert_equal 34, view.get_int16(2)
    assert_equal 8323.32, view.get_float64(3)

    f.close
  ensure
    File.unlink(file_name) rescue nil
  end

  def test_socket_io
    buf = ArrayBuffer.new(16)

    server = TCPServer.new("127.0.0.1", 0)
    port = server.addr[1]
    client = TCPSocket.new("127.0.0.1", port)
    s = server.accept

    view = DataView.new(buf)
    view.set_int8(0, 5)
    view.set_int8(1, 12)
    view.set_int16(2, 34)
    view.set_float64(3, 8323.32)

    view.write(client)

    buf = ArrayBuffer.read(s, 16)

    view = DataView.new(buf)
    assert_equal 16, view.byte_length

    assert_equal 5, view.get_int8(0)
    assert_equal 12, view.get_int8(1)
    assert_equal 34, view.get_int16(2)
    assert_equal 8323.32, view.get_float64(3)

    s.close
  ensure
    s.close rescue nil
  end
end