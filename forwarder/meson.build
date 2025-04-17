bbzl_forwarder = shared_library(
  'bambu_networking',
  'forwarder.cpp',
  install: false,
  cpp_args: [],
  gnu_symbol_visibility: 'hidden',
  dependencies: [
    bbzl_api_dep,
  ],
)

bbzl_forwarder_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: bbzl_forwarder,
)
