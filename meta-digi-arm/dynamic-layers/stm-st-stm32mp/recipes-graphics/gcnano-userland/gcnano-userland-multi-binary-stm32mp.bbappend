# Copyright (C) 2022-2023, Digi International Inc.

#
# Workarounds for Crank storyboard engine
#

# Put the devel symlink in the normal package
FILES:libegl-gcnano += "${libdir}/libEGL${SOLIBSDEV}"
FILES:libgles2-gcnano += "${libdir}/libGLESv2${SOLIBSDEV}"

# Add explicit runtime provides for libEGL.so and libGLESv2.so
RPROVIDES:libegl-gcnano:prepend = "libEGL.so "
RPROVIDES:libgles2-gcnano:prepend = "libGLESv2.so "
