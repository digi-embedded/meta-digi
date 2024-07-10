# Copyright (C) 2016-2024, Digi International Inc.

SUMMARY = "DEY trustfence packagegroup"

inherit packagegroup

RDEPENDS:${PN} = "\
	${@oe.utils.conditional('TRUSTFENCE_CONSOLE_DISABLE', '1', 'auto-serial-console', '', d)} \
	${@oe.utils.vartrue('TRUSTFENCE_FILE_BASED_ENCRYPT', 'e2fsprogs-tune2fs trustfence-fscrypt', '', d)} \
"
do_package[vardeps] += "TRUSTFENCE_CONSOLE_DISABLE TRUSTFENCE_FILE_BASED_ENCRYPT"
