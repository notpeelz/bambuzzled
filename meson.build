project(
  'bambuzzled',
  'cpp',
  version: '0.1',
  default_options: ['warning_level=3', 'cpp_std=c++20']
)

shlib = shared_library(
  'bambu_networking',
  'bambuzzled.cpp',
  install: false,
  cpp_args: [],
  gnu_symbol_visibility: 'hidden',
  dependencies: [
    dependency('boost', modules: ['filesystem', 'log']),
  ],
)

bambuzzled_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: shlib
)
