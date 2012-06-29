require 'socket'

module TypeArray::IOReader
  def read(io, length)
    buf = nil
    case io
    when BasicSocket
      buf = io.recv(length)
    when IO
      buf = ""
      io.read(length, buf)
    end
    new(buf)
  end
end

module TypeArray::IOWriter
  def write(io)
    case io
    when BasicSocket
      io.send(to_s, 0)
    when IO
      io.write(to_s)
    end
  end
end