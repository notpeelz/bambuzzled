add_library(
  bambuzzled-video
  SHARED
  video.cpp
)
set_target_properties(
  bambuzzled-video
  PROPERTIES
  OUTPUT_NAME BambuSource
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
target_link_libraries(
  bambuzzled-video
  PRIVATE
  bambuzzled-shared
)
