SOURCES = sse4_1.cpp
CONFIG -= qt dylib release debug_and_release
CONFIG += debug console
!defined(QMAKE_CFLAGS_SSE4_1, "var"):error("This compiler does not support SSE4.1")
else:QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_SSE4_1
