# encoding: utf-8

$:.unshift File.expand_path('lib')
require 'type_array'
require 'benchmark'

typed = Float32Array.new(1_000_000)
native = Array.new(1_000_000)


Benchmark.bmbm do |x|
  x.report("typed array []=") do
    1_000_000.times {|i| typed[i] = 0.1234567890123456; typed[i] }
  end

  x.report("array []=") do
    1_000_000.times {|i| native[i] = 0.1234567890123456; typed[i] }
  end
end
