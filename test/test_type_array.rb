# encoding: utf-8

require File.join(File.dirname(__FILE__), 'helper')

class TestTypeArray < TypeArrayTestCase
  def test_type
    assert_instance_of Class, TypeArray
  end

  def test_direct_constructor
    assert_raises TypeError do
      TypeArray.new(100)
    end
  end

  def test_unsupported_constructor
    assert_raises TypeError do
      Int32Array.new(:invalid)
    end
  end

  def test_length_constructor
    ary = Int32Array.new(100)

    assert_instance_of Int32Array, ary
    assert_instance_of ArrayBuffer, ary.buffer
    assert_equal 100, ary.length
    assert_equal 400, ary.byte_length
    assert_equal 0, ary.byte_offset
  end

  def test_string_constructor
    str = "01234567"
    ary = Int32Array.new(str)

    assert_equal 8, ary.byte_length
    assert_equal 2, ary.length
    assert_equal str, ary.to_s
  end

  def test_array_buf_constructor
    buf = ArrayBuffer.new(100)

    ary = Int32Array.new(buf)
    assert_equal 25, ary.length
    assert_equal 100, ary.byte_length
    assert_equal 0, ary.byte_offset
    assert_equal buf, ary.buffer

    assert_raises TypeError do
      Int32Array.new(buf, :symbol)
    end

    assert_raises RangeError do
      Int32Array.new(buf, 13)
    end
  end

  def test_offset_length_constructor
    buf = ArrayBuffer.new(100)

    ary = Int32Array.new(buf, 20)
    assert_equal 20, ary.length
    assert_equal 80, ary.byte_length
    assert_equal 20, ary.byte_offset

    ary = Int32Array.new(buf, 0, 20)
    assert_equal 20, ary.length
    assert_equal 80, ary.byte_length
    assert_equal 0, ary.byte_offset

    ary = Int32Array.new(buf, 20, 20)
    assert_equal 20, ary.length
    assert_equal 80, ary.byte_length
    assert_equal 20, ary.byte_offset

    assert_raises RangeError do
      Int32Array.new(buf, 84, 20)
    end
  end

  def test_type_array_constructor
    buf = ArrayBuffer.new(100)

    ary = Int16Array.new(buf)
    ary[0] = 100
    ary[1] = 200
    ary[2] = 300

    ary2 = Int8Array.new(ary)
    assert_equal 100, ary2[0]
    assert_equal -56, ary2[1]
    assert_equal 44, ary2[2]
  end

  def test_std_io
    buf = ArrayBuffer.new(16)

    f = File.open(file_name, File::CREAT | File::RDWR)

    ary = Int32Array.new(buf)
    ary[0] = 78
    ary[1] = 43
    ary[2] = 54
    ary[3] = 12

    ary.write(f)

    f.flush
    assert_equal 16, File.size(file_name)

    f.rewind

    ary = Int32Array.read(f, 16)
    assert_equal 16, ary.byte_length

    assert_equal 78, ary[0]
    assert_equal 43, ary[1]
    assert_equal 54, ary[2]
    assert_equal 12, ary[3]

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

    ary = Int32Array.new(buf)
    ary[0] = 78
    ary[1] = 43
    ary[2] = 54
    ary[3] = 12

    ary.write(client)

    ary = Int32Array.read(s, 16)
    assert_equal 16, ary.byte_length

    assert_equal 78, ary[0]
    assert_equal 43, ary[1]
    assert_equal 54, ary[2]
    assert_equal 12, ary[3]

    s.close
  ensure
    s.close rescue nil
  end
end