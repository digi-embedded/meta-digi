# Copyright (C) 2018 Digi International

do_install_append_class-nativesdk() {
   mkdir -p ${D}${SDKPATHNATIVE}/environment-setup.d
}

do_generate_qt_environment_file[noexec] = "1"
do_install_append () {
    do_generate_qt_environment_file
}
