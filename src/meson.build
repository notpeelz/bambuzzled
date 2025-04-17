bbzl_shlib = shared_library(
  'bambu_networking',
  'bambuzzled.cpp',
  install: false,
  cpp_args: [],
  gnu_symbol_visibility: 'hidden',
  dependencies: [
    bbzl_api_dep,
    dependency('libmosquitto'),
    dependency('openssl'),
    dependency('nlohmann_json'),
  ],
)

bbzl_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: bbzl_shlib
)
