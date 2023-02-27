# Copyright (C) 2022-2023, Digi International Inc.

#
# Workarounds for Crank storyboard engine
#

# Put the devel symlink in the normal package
FILES:libegl-gcnano += "${libdir}/libEGL${SOLIBSDEV}"
FILES:libgles1-gcnano += "${libdir}/libGLESv1_CM${SOLIBSDEV}"
FILES:libgles2-gcnano += "${libdir}/libGLESv2${SOLIBSDEV}"
FILES:libopenvg-gcnano += "${libdir}/libOpenVG${SOLIBSDEV}"

# Add explicit runtime provides for libEGL.so, libGLESv2.so, libGLESv1_CM.so and libOpenVG.so
RPROVIDES:libegl-gcnano:prepend = "libEGL.so "
RPROVIDES:libgles1-gcnano:prepend = "libGLESv1_CM.so "
RPROVIDES:libgles2-gcnano:prepend = "libGLESv2.so "
RPROVIDES:libopenvg-gcnano:prepend = "libOpenVG.so "
