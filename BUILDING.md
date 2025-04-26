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

## Compiling Boost

You'll need to download, extract, then build Boost:
```pwsh
boostrap.bat toolset=msvc
b2.exe toolset=msvc link=static address-model=64
```

## Configuring and building

In order to build the project, you'll need to install `cmake`, `ninja`, `patch` and `openssl` via [scoop](https://scoop.sh/).

```pwsh
cmake -B build -S . `
  -G Ninja `
  -DCMAKE_PREFIX_PATH="C:/path/to/boost/stage/lib/cmake" `
  -DOPENSSL_ROOT_DIR="${env:UserProfile}/scoop/apps/openssl/current" `
  -DBBZL_VENDOR_NLOHMANN_JSON=ON `
  -DBBZL_VENDOR_MOSQUITTO=ON `
  -DBBZL_STATIC=ON
cmake --build build --config Release -j

Copy-Item -Path "./build/src/bambu_networking.dll" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
Copy-Item -Path "./build/src/bambu_networking.pdb" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
Copy-Item -Path "./build/video/BambuSource.dll" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
Copy-Item -Path "./build/video/BambuSource.pdb" -Destination "${env:AppData}/OrcaSlicer/plugins/." -Force
```
