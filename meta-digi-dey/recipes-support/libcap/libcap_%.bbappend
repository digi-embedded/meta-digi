# Copyright (C) 2024 Digi International Inc.

# Don't link the pam_cap module against libpam at build-time to avoid
# a libpam dependency in the recovery initramfs-
EXTRA_OEMAKE += " \
    FORCELINKPAM=no \
"
