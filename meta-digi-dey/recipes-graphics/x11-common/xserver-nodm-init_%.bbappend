# Copyright (C) 2018 Digi International

do_install:append() {
    # Redirect output to log file
    sed -i -e "/^exec/{s,\$\*,\$\* >/var/log/Xsession.log 2>\&1,}" ${D}${sysconfdir}/xserver-nodm/Xserver
}
