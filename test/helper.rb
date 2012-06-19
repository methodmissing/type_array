# encoding: utf-8

require 'test/unit'
require 'type_array'

class TypeArrayTestCase < Test::Unit::TestCase
  undef_method :default_test if method_defined? :default_test

  if ENV['STRESS_GC']
    def setup
      GC.stress = true
    end

    def teardown
      GC.stress = false
    end
  end
end