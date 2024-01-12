#!/bin/sh

export QT_QPA_PLATFORM=eglfs

# Use the KMS/DRM backend
export QT_QPA_EGLFS_INTEGRATION=eglfs_kms

if [ -e /usr/share/qt5/cursor.json ];
then
	export QT_QPA_EGLFS_KMS_CONFIG=/usr/share/qt5/cursor.json
fi

# Force to keep the MODE SETTING set
export QT_QPA_EGLFS_ALWAYS_SET_MODE=1

# Force to use KMS ATOMIC
export QT_QPA_EGLFS_KMS_ATOMIC=1

# EGLFS environment variables accessible for qt 5.12
# Documentation: https://doc.qt.io/qt-5/embedded-linux.html
