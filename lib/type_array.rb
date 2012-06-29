# encoding: utf-8

# Prefer compiled Rubinius bytecode in .rbx/
ENV["RBXOPT"] = "-Xrbc.db"

require "type_array_ext"

require 'type_array/version' unless defined? TypeArray::VERSION
require 'type_array/io'

[ArrayBuffer, TypeArray, DataView].each do |c|
  c.extend TypeArray::IOReader
  c.send(:include, TypeArray::IOWriter)  
end