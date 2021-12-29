# Copyright 2019-2021 Digi International, Inc.

# The original recipe in meta-openembedded hardcodes a "master" branch, which
# doesn't exist in this repo, causing an error. Use the same SRC_URI, but
# using the "main" branch instead.
SRC_URI = "git://github.com/tesseract-ocr/tessdata.git;branch=main;protocol=https"
