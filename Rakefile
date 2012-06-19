# encoding: utf-8

require 'rubygems' unless defined?(Gem)
require 'rake' unless defined?(Rake)

# Prefer compiled Rubinius bytecode in .rbx/
ENV["RBXOPT"] = "-Xrbc.db"

require 'rake/extensiontask'
require 'rake/testtask'

gemspec = eval(IO.read('type_array.gemspec'))

Gem::PackageTask.new(gemspec) do |pkg|
end

Rake::ExtensionTask.new('type_array', gemspec) do |ext|
  ext.name = 'type_array_ext'
  ext.ext_dir = 'ext/type_array'

  CLEAN.include 'lib/**/type_array_ext.*'
end

desc 'Run TypeArray tests'
Rake::TestTask.new(:test) do |t|
  t.test_files = Dir.glob("test/**/test_*.rb")
  t.verbose = true
  t.warning = true
end

task :test => :compile
task :default => :test