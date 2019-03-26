# Copyright (C) 2018, 2019, Digi International Inc.

require greengrass.inc

#
# The package includes different licenses:
#
# [Apache-2.0]
#   ggc/core/LICENSE/attributions/github_aws_aws_sdk_go_License.txt
#   ggc/core/LICENSE/attributions/github_coreos_go_systemd_License.txt
#   ggc/core/LICENSE/attributions/github_docker_docker_License.txt
#   ggc/core/LICENSE/attributions/github_docker_go_units_License.txt
#   ggc/core/LICENSE/attributions/github_go_ini_ini_License.txt
#   ggc/core/LICENSE/attributions/github_jmespath_go_jmespath_License.txt
#   ggc/core/LICENSE/attributions/github_opencontainers_runc_License.txt
#   ggc/core/LICENSE/attributions/github_opencontainers_runtime_spec_License.txt
#   ggc/core/LICENSE/attributions/github_pquerna_ffjson_License.txt
#   ggc/core/LICENSE/attributions/github_vishvananda_netlink_License.txt
# [BSD-2-Clause]
#   ggc/core/LICENSE/attributions/github_godbus_dbus_License.txt
#   ggc/core/LICENSE/attributions/github_huin_gobinarytest_License.txt
#   ggc/core/LICENSE/attributions/github_seccomp_libseccomp_golang_License.txt
#   ggc/core/LICENSE/attributions/github_syndtr_gocapability_License.txt
# [BSD-3-Clause]
#   ggc/core/LICENSE/attributions/github_fsnotify_fsnotify_License.txt
#   ggc/core/LICENSE/attributions/github_golang_protobuf_License.txt
#   ggc/core/LICENSE/attributions/github_jeffallen_mqtt_License.txt
#   ggc/core/LICENSE/attributions/Golang_License.txt
# [ECLIPSE]
#   ggc/core/LICENSE/attributions/github_paho_mqtt_License.txt
# [MIT]
#   ggc/core/LICENSE/attributions/github_huin_mqtt_License.txt
#   ggc/core/LICENSE/attributions/github_mattn_go_sqlite3_License.txt
#   ggc/core/LICENSE/attributions/github_nu7hatch_gouuid_License.txt
#   ggc/core/LICENSE/attributions/github_Sirupsen_logrus_License.txt
#   ggc/core/LICENSE/attributions/github_urfave_cli_License.txt
# [Proprietary]
#   ggc/core/LICENSE/Greengrass AWS SW License (IoT additiona) vr6.txt
# [Copyright-Only Decication]
#   ggc/core/LICENSE/attributions/libb64_sourceforge_License.txt

LIC_FILES_CHKSUM = " \
    file://ggc/core/LICENSE/attributions/curl_haxx_se_License.txt;md5=3b77a99e8530d6c9ccc9aab9a7436f99 \
    file://ggc/core/LICENSE/attributions/github_aws_aws_sdk_go_License.txt;md5=d273d63619c9aeaf15cdaf76422c4f87 \
    file://ggc/core/LICENSE/attributions/github_coreos_go_systemd_License.txt;md5=715f3348ed8b9bf4fac3b08133384a4d \
    file://ggc/core/LICENSE/attributions/github_docker_docker_License.txt;md5=bba4ee48af378e39b452d742d29c710b \
    file://ggc/core/LICENSE/attributions/github_docker_go_units_License.txt;md5=bb99db20f1c48c2c4952c27c72855e36 \
    file://ggc/core/LICENSE/attributions/github_fsnotify_fsnotify_License.txt;md5=c38914c9a7ab03bb2b96d4baaee10769 \
    file://ggc/core/LICENSE/attributions/github_godbus_dbus_License.txt;md5=b03a62440372a9acf9692ad365932c87 \
    file://ggc/core/LICENSE/attributions/github_go_ini_ini_License.txt;md5=715f3348ed8b9bf4fac3b08133384a4d \
    file://ggc/core/LICENSE/attributions/github_golang_protobuf_License.txt;md5=16fe162f7848190010b6ec7bfaac030a \
    file://ggc/core/LICENSE/attributions/github_huin_gobinarytest_License.txt;md5=f2b3138d9d314bccf5297dea7e3e6d14 \
    file://ggc/core/LICENSE/attributions/github_huin_mqtt_License.txt;md5=12fd125064676697934b7d8c09bed0e8 \
    file://ggc/core/LICENSE/attributions/github_jeffallen_mqtt_License.txt;md5=b7269d52765d477e10f319c19d8a9d33 \
    file://ggc/core/LICENSE/attributions/github_jmespath_go_jmespath_License.txt;md5=640d33f0070c9dc3a194d2ed7db02974 \
    file://ggc/core/LICENSE/attributions/github_mattn_go_sqlite3_License.txt;md5=948f36a2300ac729e60416063190f664 \
    file://ggc/core/LICENSE/attributions/github_nu7hatch_gouuid_License.txt;md5=6b18748dcc29fda05fa5aaef44d517fd \
    file://ggc/core/LICENSE/attributions/github_opencontainers_runc_License.txt;md5=587c01b2dcc5dc3b4bed51b918c64731 \
    file://ggc/core/LICENSE/attributions/github_opencontainers_runtime_spec_License.txt;md5=ef95ed297310c3d09ba16c06d5e161a5 \
    file://ggc/core/LICENSE/attributions/github_paho_mqtt_License.txt;md5=655315b482a5955efb91d34de332ee48 \
    file://ggc/core/LICENSE/attributions/github_pquerna_ffjson_License.txt;md5=d273d63619c9aeaf15cdaf76422c4f87 \
    file://ggc/core/LICENSE/attributions/github_seccomp_libseccomp_golang_License.txt;md5=9205c4c469bfb9d3a63f346539ee445b \
    file://ggc/core/LICENSE/attributions/github_Sirupsen_logrus_License.txt;md5=29baae91637760ae68feb57ca93e5a0a \
    file://ggc/core/LICENSE/attributions/github_syndtr_gocapability_License.txt;md5=321f58fa53a0b1bb9a887f14660d436b \
    file://ggc/core/LICENSE/attributions/github_urfave_cli_License.txt;md5=f1f14a2449300559aed90bedc36a71ed \
    file://ggc/core/LICENSE/attributions/github_vishvananda_netlink_License.txt;md5=c95fd0efd62139c155e956a448df8fd6 \
    file://ggc/core/LICENSE/attributions/Golang_License.txt;md5=3d7ed06383c65a3161b36c6a0b0b98f5 \
    file://ggc/core/LICENSE/attributions/libb64_sourceforge_License.txt;md5=0f8f14bb84b17652f29aad01072e545f \
    file://ggc/core/LICENSE/attributions/sqlite_org_License.txt;md5=380e2694a297aa32879ca2ae9c6c029b\
"

# Bitbake does not support spaces in filenames, but GG License does have spaces,
# so workaround the problem by renaming the file before using it.
GG_LIC_FILENAME = "Greengrass AWS SW License (IoT additional) vr6.txt"
GG_LIC_FILENAME_NOSPACES = "${@d.getVar('GG_LIC_FILENAME', True).replace(' ','_')}"
LIC_FILES_CHKSUM += "file://ggc/core/LICENSE/${GG_LIC_FILENAME_NOSPACES};md5=7df5bf535d02b2f83c260250fe330b6c"

SRC_URI[arm.md5sum] = "93ae820af2bf2527bafdb34598d174ed"
SRC_URI[arm.sha256sum] ="8fe99ba17917df2e192b7065e400e2dc85c4a0fbf7654fa0d141642cde92d88f"

# For ARCH64 we use another tarball.
SRC_URI[aarch64.md5sum] = "e4ec6dba43dcba4d2ec1b04d7c851cd3"
SRC_URI[aarch64.sha256sum] ="f45e502435850fb9a9931a46fd96329f95f53ff2d6d5aaa4bed11094c0237f4c"

# Rename GG license file
do_unpack[postfuncs] += "rename_license"
rename_license() {
	cd ${S}/ggc/core/LICENSE/
	mv "${GG_LIC_FILENAME}" "${GG_LIC_FILENAME_NOSPACES}"
}
