
RDEPENDS:${PN}:remove = "qtquick1-dev \
    qtquick1-mkspecs \
    qtquick1-plugins \
    qtquick1-qmlplugins \
    qttranslations-qtquick1 \
    qtwebkit-dev \
    qtwebkit-mkspecs \
    qtwebkit-qmlplugins \
    qt3d-dev \
    qt3d-mkspecs \
    qt3d-qmlplugins \
"

RDEPENDS:${PN}:append:imxgpu = " \
    qtdeclarative-tools \
"
