# Linux

## Standard build

```sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j
```

## Statically-linked build

### Compiling OpenSSL

```
git clone --recursive https://github.com/openssl/openssl.git /path/to/openssl
cd /path/to/openssl
perl Configure \
  no-docs \
  no-unit-test \
  --prefix="$PWD/install" \
  --libdir=lib \
  --openssldir="$PWD/install" \
  -static \
  -fPIC
make install
```

### Configuring and building

```
cmake -B build -S . \
  -DOPENSSL_ROOT_DIR="/path/to/openssl/install" \
  -DBBZL_WITH_NLOHMANN_JSON=VENDOR \
  -DBBZL_WITH_MOSQUITTO=VENDOR \
  -DBBZL_WITH_CURL=VENDOR \
  -DBBZL_STATIC_DEPS=ON
cmake --build build --config Release -j
```

# Windows

## VS dev shell

You'll need a VS dev shell configured for x64.
For VSCode, you may add this to your `settings.json`:

```json
"terminal.integrated.profiles.windows": {
  "VS Developer PowerShell": {
    "source": "PowerShell",
    "icon": "terminal-powershell",
    "args": [
      "-NoExit",
      "-Command",
      "& { $originalLocation = Get-Location; $dllPath = \"${env:ProgramFiles}\\Microsoft Visual Studio\\2022\\Community\\Common7\\Tools\\Microsoft.VisualStudio.DevShell.dll\"; $vsPath = \"${env:ProgramFiles}\\Microsoft Visual Studio\\2022\\Community\\\"; if (Test-Path $dllPath) { Import-Module $dllPath; Enter-VsDevShell -VsInstallPath $vsPath -DevCmdArguments '-no_logo -arch=x64 -host_arch=x64'; Set-Location $originalLocation; Write-Host \"VS Developer PowerShell environment loaded.\" -ForegroundColor Green } else { Write-Host \"Visual Studio DevShell DLL not found at: $dllPath\" -ForegroundColor Yellow } }"
    ]
  }
}
```

## Configuring and building

In order to build the project, you'll need to install `cmake`, `ninja`, `patch` and `openssl` via [scoop](https://scoop.sh/).

```pwsh
cmake -B build -S . `
  -G Ninja `
  -DCMAKE_BUILD_TYPE=RelWithDebInfo `
  -DOPENSSL_ROOT_DIR="${env:UserProfile}/scoop/apps/openssl/current" `
  -DBBZL_WITH_NLOHMANN_JSON=VENDOR `
  -DBBZL_WITH_MOSQUITTO=VENDOR `
  -DBBZL_WITH_CURL=VENDOR `
  -DBBZL_STATIC_DEPS=ON
cmake --build build -j

Copy-Item -Path "./build/network-plugin/bambu_networking.dll" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
Copy-Item -Path "./build/network-plugin/bambu_networking.pdb" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
Copy-Item -Path "./build/video-plugin/BambuSource.dll" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
Copy-Item -Path "./build/video-plugin/BambuSource.pdb" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
```

# Useful tricks for development

## OpenSSL certificates

If you have trouble with certificates, you can use the `SSL_CERT_DIR` env var to tell OpenSSL where to find the BBL CA.
On Linux: `SSL_CERT_DIR="/etc/ssl/certs:$PWD/certs"`
On Windows: `SSL_CERT_DIR="$PWD/certs"`
