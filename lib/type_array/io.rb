module TypeArray::IOReader
  def read(io, length)
    buf = ""
    io.read(length, buf)
    new(buf)
  end
end

module TypeArray::IOWriter
  def write(io)
    io.write(to_s)
  end
end