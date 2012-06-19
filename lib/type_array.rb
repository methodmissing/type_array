# encoding: utf-8

# Prefer compiled Rubinius bytecode in .rbx/
ENV["RBXOPT"] = "-Xrbc.db"

require "type_array_ext"

require 'type_array/version' unless defined? TypeArray::VERSION