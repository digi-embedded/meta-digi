#!/bin/sh

export QT_QPA_PLATFORM=eglfs

# Use the KMS/DRM backend
export QT_QPA_EGLFS_INTEGRATION=eglfs_kms

if [ -e /usr/share/qt6/cursor.json ]; then
	export QT_QPA_EGLFS_KMS_CONFIG=/usr/share/qt6/cursor.json
fi

# Force to keep the MODE SETTING set
export QT_QPA_EGLFS_ALWAYS_SET_MODE=1

# Force to use KMS ATOMIC
export QT_QPA_EGLFS_KMS_ATOMIC=1

# EGLFS environment variables accessible for qt 6.8
# Documentation: https://doc.qt.io/qt-6/embedded-linux.html
