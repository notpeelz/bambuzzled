# bambuzzled

A replacement for Bambu Lab's closed-source networking plugin, so a
third-party slicer can drive a printer entirely offline, without
proprietary code phoning home.

Linux and Windows are supported.

## Background

In January 2025 Bambu Lab announced a firmware update with a new
"authorization control" system. Third-party slicers were to lose direct network
access, and printing would have to go through Bambu Connect, a proprietary
desktop app acting as a middleman between slicer and printer. Bambu framed it
as a security measure; a lot of owners saw lock-in instead. OrcaSlicer (itself
a fork of Bambu Studio) refused to support Bambu Connect and told users not to
upgrade. Bambu later walked some of it back with a "Developer Mode" under LAN
mode.

## Motivation

Even with the printer in LAN mode, OrcaSlicer reaches it through
`bambu_networking`, a closed-source binary shipped by Bambu.
The binary can break at any time, for instance when the C++ runtime
or one of its other dependencies changes, and there is nothing to do
but wait for a new build. It also makes it impossible to audit.

It's also a packaging problem; a distro cannot rebuild the blob. For example,
nixpkgs had to mark `bambu-studio` as `unfree`: it downloads and `dlopen`s a
proprietary networking library on first launch
([NixOS/nixpkgs#415821](https://github.com/NixOS/nixpkgs/issues/415821)). That
library also crashes or fails to load outright on NixOS
([#391622](https://github.com/NixOS/nixpkgs/issues/391622),
[#398019](https://github.com/NixOS/nixpkgs/issues/398019)), because a binary
built elsewhere cannot link against the distribution's own libraries.

## Layout

- `shared/` - sockets, TLS, logging and curl helpers
- `network-plugin/` - the `bambu_networking` replacement (MQTT, FTP, pairing)
- `video-plugin/` - the `BambuSource` replacement
- `forwarder/` - development-only shim that loads the official plugin and logs
every call; point `BBZL_FORWARD` at it to compare behavior or capture traffic

Build instructions are in [BUILDING.md](BUILDING.md).

## Status

This is one of my first larger C++ projects: it works but
it has bugs and missing functionality. Treat it as a proof of concept rather
than a drop-in replacement for anything important.
