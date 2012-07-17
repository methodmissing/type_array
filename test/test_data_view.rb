# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestDataView < TypeArrayTestCase
  def test_type
    assert_instance_of Class, DataView
  end

  def test_buffer_constructor
    buf = ArrayBuffer.new(100)

    assert_raises TypeError do
      DataView.new(:invalid)
    end

    assert_raises RangeError do
      DataView.new(buf, 101)
    end

   assert_raises RangeError do
     DataView.new(buf, 0, 101)
   end

    assert_raises RangeError do
      DataView.new(buf, 4, 100)
    end

    view = DataView.new(buf)
    assert_instance_of DataView, view
    assert_equal 100, view.byte_length
    assert_equal 0, view.byte_offset
    assert_equal buf, view.buffer
  end

  def test_string_constructor
    str = "01234567"
    view = DataView.new(str)
    assert_equal 8, view.byte_length
    assert_equal str, view.to_s
  end

  def test_offset_constructor
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf, 10)
    assert_equal 90, view.byte_length
    assert_equal 10, view.byte_offset
  end

  def test_byte_length_constructor
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf, 0, 50)
    assert_equal 50, view.byte_length
    assert_equal 0, view.byte_offset

    view = DataView.new(buf, 15, 50)
    assert_equal 35, view.byte_length
    assert_equal 15, view.byte_offset
  end

  def test_to_s
    buf = ArrayBuffer.new(8)

    view = DataView.new(buf)
    str = view.to_s
    assert_equal "\x00\x00\x00\x00\x00\x00\x00\x00", str
    assert str.frozen?
  end

  def test_marshal
    buf = ArrayBuffer.new(4)

    view = DataView.new(buf)
    view.set_int8(1, 5)
    view.set_int16(3, 7)

    serialized = "\x04\bu:\rDataView\t\x00\x05\x00\a"
    assert_equal serialized, Marshal.dump(view)

    view = Marshal.load(serialized)
    assert_instance_of DataView, view
    assert_equal 5, view.get_int8(1)
    assert_equal 7, view.get_int8(3)
  end

  def test_int8
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_int8(1)
    view.set_int8(1, 5)
    assert_equal 5, view.get_int8(1)
  end

  def test_uint8
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_uint8(1)
    view.set_uint8(1, 5)
    assert_equal 5, view.get_uint8(1)
  end

  def test_int16
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_int16(1)
    view.set_int16(1, 5000)
    assert_equal 5000, view.get_int16(1)
  end

  def test_uint16
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_uint16(1)
    view.set_uint16(1, 19264)
    assert_equal 19264, view.get_uint16(1)
  end

  def test_int32
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_int32(1)
    view.set_int32(1, 50000000)
    assert_equal 50000000, view.get_int32(1)
  end

  def test_uint32
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_uint32(1)
    view.set_uint32(1, 80000000)
    assert_equal 80000000, view.get_uint32(1)
  end

  def test_float32
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_float32(1)
    view.set_float32(1, 55.77)
    assert_in_delta 55.77, view.get_float32(1), 0.01
  end

  def test_float64
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    assert_equal 0, view.get_float64(1)
    view.set_float64(1, 323233.77)
    assert_equal 323233.77, view.get_float64(1)
  end

  def test_combo
    buf = ArrayBuffer.new(100)

    view = DataView.new(buf)
    view.set_int8(0, 5)
    view.set_int8(1, 12)
    view.set_int16(2, 34)
    view.set_float64(4, 8323.32)

    assert_equal 5, view.get_int8(0)
    assert_equal 12, view.get_int8(1)
    assert_equal 34, view.get_int16(2)
    assert_equal 8323.32, view.get_float64(4)
  end

  def test_std_io
    buf = ArrayBuffer.new(16)

    f = File.open(file_name, File::CREAT | File::RDWR)

    view = DataView.new(buf)
    view.set_int8(0, 5)
    view.set_int8(1, 12)
    view.set_int16(2, 34)
    view.set_float64(4, 8323.32)

    view.write(f)

    f.flush
    assert_equal 16, File.size(file_name)

    f.rewind

    view = DataView.read(f, 16)
    assert_equal 16, view.byte_length

    assert_equal 5, view.get_int8(0)
    assert_equal 12, view.get_int8(1)
    assert_equal 34, view.get_int16(2)
    assert_equal 8323.32, view.get_float64(4)

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
    view.set_float64(4, 8323.32)

    view.write(client)

    view = DataView.read(s, 16)
    assert_equal 16, view.byte_length

    assert_equal 5, view.get_int8(0)
    assert_equal 12, view.get_int8(1)
    assert_equal 34, view.get_int16(2)
    assert_equal 8323.32, view.get_float64(4)

    s.close
  ensure
    s.close rescue nil
  end
end