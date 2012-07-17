# encoding: utf-8

require 'rubygems' unless defined?(Gem)
require 'rake' unless defined?(Rake)

# Prefer compiled Rubinius bytecode in .rbx/
ENV["RBXOPT"] = "-Xrbc.db"

ENV["RUBY_CC_VERSION"] = "1.8.7:1.9.2:1.9.3"
require 'rake/extensiontask'
require 'rake/testtask'

begin
require 'rdoc/task'
rescue LoadError # fallback to older 1.8.7 rubies
require 'rake/rdoctask'
end

gemspec = eval(IO.read('type_array.gemspec'))

Gem::PackageTask.new(gemspec) do |pkg|
end

Rake::ExtensionTask.new('type_array', gemspec) do |ext|
  ext.cross_compile = true
  ext.name = 'type_array_ext'
  ext.ext_dir = 'ext/type_array'

  ext.cross_compiling do |gem_spec|
    gem_spec.post_install_message = "You installed the binary version of this gem!"
  end

  CLEAN.include 'lib/**/type_array_ext.*'
end

desc 'Run TypeArray tests'
Rake::TestTask.new(:test) do |t|
  t.test_files = Dir.glob("test/**/test_*.rb")
  t.verbose = true
  t.warning = true
end

Rake::RDocTask.new do |rd|
  files = FileList["README.rdoc", "ext/type_array/*.c"]
  rd.title = "TypeArray - Ruby implementation of the ECMAScript spec"
  rd.main = "README.rdoc"
  rd.rdoc_dir = "doc"
  rd.options << "--promiscuous"
  rd.rdoc_files.include(files)
end

task :test => :compile
task :default => :test