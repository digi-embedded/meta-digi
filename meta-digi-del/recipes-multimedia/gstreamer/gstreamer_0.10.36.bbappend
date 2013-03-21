# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

EXTRA_OECONF += "\
		--disable-check \
		--disable-debug \
		--disable-failing-tests \
		--disable-rpath \
		--disable-shave \
		"
