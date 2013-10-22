# Copyright (C) 2013 Digi International.

PRINC := "${@int(PRINC) + 1}"
PR_append = "+${DISTRO}"

EXTRA_OEMAKE_LINUX_2X = "\'CFLAGS=${CFLAGS} -Iutil/include -Iarch/${ARCH}/include -I${STAGING_KERNEL_DIR}/include\'"

EXTRA_OEMAKE += "${@base_conditional('IS_KERNEL_2X', '1' , '${EXTRA_OEMAKE_LINUX_2X}', '', d)}"
