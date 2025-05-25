#pragma once

#include <string>
#include <istream>
#include <optional>
#include <functional>
#include <curl/system.h>
#include <bambuzzled/optional_ref.hpp>

namespace bbzl::ftp {

using file_offset = curl_off_t;

using on_upload_progress_cb_fn = std::function<void(
  file_offset total,
  file_offset current
)>;

[[nodiscard]]
bool upload_file(
  std::string const& url,
  std::string const& username,
  std::string const& password,
  std::istream& is,
  std::optional<on_upload_progress_cb_fn> const& on_upload_progess_cb =
    std::nullopt,
  bbzl::utils::optional_ref<std::string> err = std::nullopt
);

}
