# encoding: utf-8

require File.expand_path('../lib/type_array/version', __FILE__)

Gem::Specification.new do |s|
  s.name = "type_array"
  s.version = TypeArray::VERSION
  s.summary = "Typed Arrays for Ruby"
  s.description = "TypeArray - Ruby implementation of the ECMAScript spec ( http://wiki.ecmascript.org/doku.php?id=strawman:typed_arrays )"
  s.authors = ["Lourens Naudé"]
  s.email = ["lourens@methodmissing.com"]
  s.homepage = "http://github.com/methodmissing/type_array"
  s.date = Time.now.utc.strftime('%Y-%m-%d')
  s.platform = Gem::Platform::RUBY
  s.extensions = "ext/type_array/extconf.rb"
  s.has_rdoc = true
  s.files = `git ls-files`.split("\n")
  s.test_files = `git ls-files test`.split("\n")
  s.rdoc_options = ["--charset=UTF-8"]
  s.require_paths = ["lib"]
  s.add_development_dependency('rake-compiler', '~> 0.8.0')
end