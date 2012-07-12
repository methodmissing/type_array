# encoding: utf-8

$:.unshift File.expand_path('lib')
require 'type_array'
require 'benchmark'

buf = ArrayBuffer.new(24)

typed = Float64Array.new(buf)

typed[0] = 2.33
typed[1] = 2.33
typed[2] = 7.45
native = [2.33, 2.33, 7.45]


Benchmark.bmbm do |x|
  x.report("type array eql") do
    1_000_000.times {|i| typed.eql(0,1) }
  end

  x.report("type array mul") do
    1_000_000.times {|i| typed.mul(0,1) }
  end

  x.report("array eql") do
    1_000_000.times {|i| native[0] == native[1] }
  end

  x.report("array mul") do
    1_000_000.times {|i| native[0] * native[1] }
  end
end