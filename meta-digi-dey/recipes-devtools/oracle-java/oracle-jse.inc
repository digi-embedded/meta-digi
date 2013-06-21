LICENSE_FLAGS = "oracle_java"
LICENSE = "Oracle_Binary_Code_License_Agreement"
LIC_FILES_CHKSUM = "\
	file://${WORKDIR}/${JDK_JRE}${PV}_${PV_UPDATE}/LICENSE;md5=98f46ab6481d87c4d77e0e91a6dbc15f \
	file://${WORKDIR}/${JDK_JRE}${PV}_${PV_UPDATE}/COPYRIGHT;md5=f5f3c0856f2ca27413b55b6ca50c897e \
	file://${WORKDIR}/${JDK_JRE}${PV}_${PV_UPDATE}/THIRDPARTYLICENSEREADME.txt;md5=c339b34e3da6673d2c5950d0f8808f8c \
	"

# get the java update version in the resulting package
PR =. "u${PV_UPDATE}"
S = "${WORKDIR}"

do_install () {
	install -d -m 0755			${D}/usr/${JDK_JRE}${PV}_${PV_UPDATE}
	cp -a ${S}/${JDK_JRE}${PV}_${PV_UPDATE}	${D}/usr/
	ln -sf ${JDK_JRE}${PV}_${PV_UPDATE}	${D}/usr/java
}

# All the files are provided in a binaray package, and keeping all the
# files in a single package causes packaging QA errors and warnings.
# Avoid these packaging failure by skiping all the QA checks
INSANE_SKIP_${PN} = "${ERROR_QA} ${WARN_QA}"

FILES_${PN} = "/usr/"

RPROVIDES += "java2-runtime"
