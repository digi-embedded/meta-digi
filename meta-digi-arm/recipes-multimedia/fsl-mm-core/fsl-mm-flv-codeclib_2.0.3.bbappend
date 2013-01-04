PR_append = "+del.r0"

FILES_${PN} += "${libdir}/lib*"

do_install_append() {
	cp -r ${S}/release/lib/* ${D}${libdir}
}
