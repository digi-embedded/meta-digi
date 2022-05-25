# Copyright (C) 2017 Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRCREV = "5ec928ddf62b0ad936efacf2b2d8fb87cca112ac"
PV = "1.5+git${SRCPV}"

SRC_URI = " \
    git://git.eclipse.org/gitroot/tcf/org.eclipse.tcf.agent;branch=1.5_oxygen_bugfix \
    file://0001-Makefile.inc-fix-ranlib.patch;striplevel=2 \
    file://0002-tcf-agent-obey-LDFLAGS.patch;striplevel=2 \
    file://0003-canonicalize_file_name-is-specific-to-glibc.patch;striplevel=2 \
    file://tcf-agent.init \
    file://tcf-agent.service \
"

# tcf-agent falls back to '/bin/sh' if 'bash' is not available, so don't
# depend on bash at runtime.
RDEPENDS:${PN}:remove = "bash"
