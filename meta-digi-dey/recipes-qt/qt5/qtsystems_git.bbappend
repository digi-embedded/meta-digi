# Copyright (C) 2016 Digi International

inherit bluetooth

PACKAGECONFIG ??= "${@bb.utils.contains('DISTRO_FEATURES', 'bluetooth', 'bluez', '', d)}"
PACKAGECONFIG[bluez] = "CONFIG+=OE_BLUEZ_ENABLED,,${BLUEZ}"

EXTRA_QMAKEVARS_PRE += "${EXTRA_OECONF}"
