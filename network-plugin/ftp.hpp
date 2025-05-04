#pragma once

#include <string>
#include <filesystem>

namespace bbzl::ftp {

void upload_file(
  std::string const& url,
  std::string const& username,
  std::string const& password,
  std::filesystem::path file_path
);

}
