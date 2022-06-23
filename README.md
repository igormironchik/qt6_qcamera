This project is just a tutorial how to work with `QCamera`, `QVideoSink` in Qt 6.

The magick with converting QVideoFrame to QImage is a subject to change
from Qt's version to version. But the main idea is done. In Qt 6.3.1
by default my camera on Linux produces frame in `QVideoFrameFormat::Format_XBGR8888`
format, that can't be converted to QImage without writing additional code.

The main reason of this repository is to show how to use `QVideoSink` to
handle each frame from the stream.
