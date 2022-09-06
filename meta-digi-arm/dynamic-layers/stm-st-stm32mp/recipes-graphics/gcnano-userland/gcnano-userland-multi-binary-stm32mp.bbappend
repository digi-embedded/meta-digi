# Copyright (C) 2022, Digi International Inc.

# Workaround for crank storyboard engine which is provided in binary format,
# and needs an explicit 'libEGL.so' runtime provides.
RPROVIDES:libegl-gcnano:prepend = "libEGL.so "
