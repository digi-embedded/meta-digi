# Copyright (C) 2024, Digi International Inc.

# Directory for models.
MODELS_DIR = "models"

# Directory for transformation tasks.
VELA_MODELS_DIR = "vela_models"

# The Vela native tool is required to transform the models.
DEPENDS += "ethos-u-vela-native"

SRC_URI += " \
    file://patches/0001-Customize-EiQ-demos.patch \
    file://patches/0002-dms-update-the-demo-to-use-the-landmark-full-model.patch \
    file://patches/0003-download_models-update-the-download-location-of-some.patch \
    file://patches/0004-improvements-capture-x-windows-and-increase-resoluti.patch \
    file://scripts/launch_eiq_demo.sh \
    file://service/eiqdemo.service \
"

# Custom task to download and transform the models using Vela.
do_download_transform_models() {
    cd "${S}"
    python3 "${S}/download_models.py"
}
do_download_transform_models[network] = "1"

# Add the custom task to download and transform the models.
addtask download_transform_models after do_patch before do_install

inherit systemd

do_install () {
    # Install scripts to /usr/bin.
    install -d "${D}${bindir}/${PN}-${PV}/"
    cp -r "${S}/dms" "${D}${bindir}/${PN}-${PV}/"
    cp -r "${S}/face_recognition" "${D}${bindir}/${PN}-${PV}/"
    cp -r "${S}/object_detection" "${D}${bindir}/${PN}-${PV}/"
    cp -r "${S}/gesture_detection" "${D}${bindir}/${PN}-${PV}/"

    # Install the original models.
    install -d "${D}${bindir}/${PN}-${PV}/${MODELS_DIR}"
    for archive in "${S}/${MODELS_DIR}"/*.tflite; do
        cp "${archive}" "${D}${bindir}/${PN}-${PV}/${MODELS_DIR}"
    done

    # Install the transformed Vela models.
    install -d "${D}${bindir}/${PN}-${PV}/${VELA_MODELS_DIR}"
    for archive in "${S}/${VELA_MODELS_DIR}"/*.tflite; do
        cp "${archive}" "${D}${bindir}/${PN}-${PV}/${VELA_MODELS_DIR}"
    done

    # Install the launch script.
    install -d ${D}${sysconfdir}/demos/scripts
    install -m 755 ${WORKDIR}/scripts/launch_eiq_demo.sh ${D}${sysconfdir}/demos/scripts/
    # Create symlinks to execute each demo.
    ln -sf launch_eiq_demo.sh ${D}${sysconfdir}/demos/scripts/launch_eiq_demo_dms.sh
    ln -sf launch_eiq_demo.sh ${D}${sysconfdir}/demos/scripts/launch_eiq_demo_gesture_detection.sh
    ln -sf launch_eiq_demo.sh ${D}${sysconfdir}/demos/scripts/launch_eiq_demo_face_recognition.sh
    ln -sf launch_eiq_demo.sh ${D}${sysconfdir}/demos/scripts/launch_eiq_demo_object_detection.sh

    # Install the systemd service.
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/service/eiqdemo.service ${D}${systemd_unitdir}/system/
}

SYSTEMD_SERVICE:${PN} = "eiqdemo.service"

PACKAGES += " \
    ${PN}-service \
"

FILES:${PN} += " \
    ${bindir}/${PN}-${PV}/* \
    ${sysconfdir}/* \
"

FILES:${PN}-service = " \
    ${systemd_unitdir}/system/eiqdemo.service \
"

# Make this recipe available only for the CC93 platform.
COMPATIBLE_MACHINE = "(ccimx93-dvk)"
