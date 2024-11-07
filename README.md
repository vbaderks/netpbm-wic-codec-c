# Netpbm Windows Imaging Component Codec (C)

This Windows Imaging Component (WIC) codec makes it possible to decode .pgm and .ppm files with Windows applications that can leverage WIC codecs.
It is a proof of concept WIC codec implementation in C based on the C++ implementation [Netpbm Windows Imaging Component Codec](https://github.com/team-charls/netpbm-wic-codec).

For a complete overview about WIC and Netpbm and for a supported implementation with installer, please see the C++ project.

## Build Instructions

1. Install Visual Studio 2022 17.12 or later
1. Clone this repository
1. Open a console and go the folder with the cloned repository
1. Execute msbuild /p:configuration=Release

### Installation

1. Open a command prompt with elevated rights
1. Navigate to folder with the netpbm_wic_codec_c.dll
1. Execute:

```shell
regsvr32 netpbm_wic_codec_c.dll
```

### Uninstall

1. Open a command prompt with elevated rights
1. Navigate to folder with the netpbm_wic_codec_c.dll
1. Execute:

```shell
regsvr32 -u netpbm_wic_codec_c.dll
```
