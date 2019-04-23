# Copyright (C) 2016-2019 Digi International.

# Split libraries into a different package
PACKAGES += "lib${PN}"

FILES_lib${PN} = "${libdir}/lib*.so.*"
