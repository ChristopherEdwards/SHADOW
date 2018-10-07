# RogueMP3 Arduino Library Changelog
************************************

Latest Release: v2.0.0
Latest Beta: v2.0.1-bxxx

V2.0.0
------
- Examples overhaul, and added sample MP3 files.
- Changed _fwLevel criteria to check if rMP3. NOTE that _fwLevel is not used, yet.
- Changed LastErrorCode to lastErrorCode, to match standards.
- Updated keywords.txt - needed some serious fixing.
- Added isPlaying().
- Introduction of String and Constant (i.e. Flash) types.
- Updated playFile, getTrackLength.
- Changed return value on some methods to return 0 (or < 0) when things go wrong, and > 0 when things are good.
- Added getVolumeLeftRight, and setVolumeLeftRight.
- Tidy, formatting, etc.
- Removed pointless function aliases, such as _commWrite() and _commFlush().
- Rename methods and members to standard.
- Added non-blocking read for sync() method.
- License file updated to MIT license.


V1.0.0 (Initial Version)
------
- Transfer from Google Code

