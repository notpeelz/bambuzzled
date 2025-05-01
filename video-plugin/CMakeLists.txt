add_library(
  bambuzzled-video-plugin
  SHARED
  video.cpp
)
add_library(bbzl::video ALIAS bambuzzled-video-plugin)
set_target_properties(
  bambuzzled-video-plugin
  PROPERTIES
  OUTPUT_NAME BambuSource
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
target_link_libraries(
  bambuzzled-video-plugin
  PRIVATE
  bbzl::shared
)
