bbzl_video_shlib = shared_library(
  'BambuSource',
  [
    'video.cpp',
  ],
  install: false,
  cpp_args: [],
  gnu_symbol_visibility: 'hidden',
)

bbzl_video_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: bbzl_video_shlib,
)
