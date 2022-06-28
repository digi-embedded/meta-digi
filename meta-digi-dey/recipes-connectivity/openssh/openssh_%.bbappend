# Copyright (C) 2022 Digi International.

# Don't create an empty openssh-dev package.
#
# When building an SDK (with populate_sdk), all '-dev' packages are
# installed. And empty 'openssh-dev' package would pull in 'openssh-sshd'
# package even if our image is only depending on 'openssh-sftp'.
#
# This causes a conflict with 'dropbear' server, which is only using
# openssh-sftp.
ALLOW_EMPTY:${PN}-dev = "0"
