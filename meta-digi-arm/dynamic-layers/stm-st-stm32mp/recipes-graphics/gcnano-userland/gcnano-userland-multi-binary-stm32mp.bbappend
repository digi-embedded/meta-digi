# Copyright (C) 2022-2025, Digi International Inc.

# Put the devel symlink in the normal package
FILES:libgles1-gcnano += "${libdir}/libGLESv1_CM${SOLIBSDEV}"
FILES:libopenvg-gcnano += "${libdir}/libOpenVG${SOLIBSDEV}"

# Add explicit runtime provides for libGLESv1_CM.so and libOpenVG.so
RPROVIDES:libgles1-gcnano:prepend = "libGLESv1_CM.so "
RPROVIDES:libopenvg-gcnano:prepend = "libOpenVG.so "
