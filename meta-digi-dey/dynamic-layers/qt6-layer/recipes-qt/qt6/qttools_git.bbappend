# Copyright (C) 2025, Digi International Inc.

# The native and nativesdk versions of this recipe automatically pull in a
# clang dependency as soon as meta-clang is included in the bblayers. Change
# this so that the dependency only gets added when explicitly setting TOOLCHAIN
# to clang.
QTTOOLS_USE_CLANG = ""

# This check is copied as-is from meta-qt6
QTTOOLS_USE_CLANG:toolchain-clang = "${@ 'clang' if bb.utils.vercmp_string_op(d.getVar('LLVMVERSION') or '', '17', '>') else ''}"
