# encoding: utf-8

# Prefer compiled Rubinius bytecode in .rbx/
ENV["RBXOPT"] = "-Xrbc.db"

require "type_array_ext"

require 'type_array/version' unless defined? TypeArray::VERSION
require 'type_array/io'
require 'type_array/marshal'

[ArrayBuffer, TypeArray, DataView].each do |c|
  c.extend TypeArray::IOReader
  c.send(:include, TypeArray::IOWriter, TypeArray::Marshal)  
end