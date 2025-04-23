project(
  'bambuzzled',
  'cpp',
  version: '0.1',
  default_options: [
    'warning_level=2',
    'cpp_std=c++20',
    'default_library=static'
  ]
)

if host_machine.system() == 'windows'
  add_project_arguments('-D_CRT_SECURE_NO_WARNINGS', language : 'c')
  add_project_arguments('-D_CRT_SECURE_NO_WARNINGS', language : 'cpp')
  add_project_arguments('/Z7', language: ['c', 'cpp']) # Debug info in object files
  add_project_link_arguments('/DEBUG', language: ['c', 'cpp']) # Generate PDB
endif

subdir('shared')
subdir('test-client')
subdir('forwarder')
subdir('video')
subdir('src')
