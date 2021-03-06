MRuby::Gem::Specification.new("mruby-threaded_tcp_client") do |spec|
  spec.license = "MIT"
  spec.authors = "Daniel Inkpen"

  spec.cc.include_paths << "#{build.root}/src"
end
