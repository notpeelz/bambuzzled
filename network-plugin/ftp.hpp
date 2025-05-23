#pragma once

#include <string>
#include <optional>
#include <functional>
#include <filesystem>
#include <curl/system.h>

namespace bbzl::ftp {

using file_offset = curl_off_t;

using on_upload_progress_cb_fn = std::function<void(
  file_offset total,
  file_offset current
)>;

void upload_file(
  std::string const& url,
  std::string const& username,
  std::string const& password,
  std::filesystem::path file_path,
  std::optional<on_upload_progress_cb_fn> const& on_upload_progess_cb =
    std::nullopt
);

}
