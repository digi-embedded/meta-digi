# Copyright (C) 2024,2025, Digi International Inc.

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# Directory for models.
MODELS_DIR = "models"

# Directory for transformation tasks.
VELA_MODELS_DIR = "vela_models"

SRC_URI += " \
    file://0001-Customize-EiQ-demos.patch \
    file://0002-improvements-capture-x-windows-and-increase-resoluti.patch \
    file://0003-check-vela-return-code.patch \
    file://0004-use-local-copy-of-ssd_mobilenet_v1_quant-model.patch \
    file://ssd_mobilenet_v1_quant.tflite \
    file://ssd_mobilenet_v1_quant_vela.tflite \
    file://scripts/launch_eiq_demo.sh \
    file://service/eiqdemo.service \
"

inherit python3native systemd

# Custom task to download and transform the models using Vela.
do_download_transform_models() {
    ${PYTHON} download_models.py
}
do_download_transform_models[depends] += " \
    ethos-u-vela-native:do_populate_sysroot \
    python3-requests-native:do_populate_sysroot \
"
do_download_transform_models[dirs] = "${S}"
do_download_transform_models[network] = "1"
addtask download_transform_models after do_patch before do_install

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
    cp ${WORKDIR}/ssd_mobilenet_v1_quant.tflite "${D}${bindir}/${PN}-${PV}/${MODELS_DIR}"

    # Install the transformed Vela models.
    install -d "${D}${bindir}/${PN}-${PV}/${VELA_MODELS_DIR}"
    for archive in "${S}/${VELA_MODELS_DIR}"/*.tflite; do
        cp "${archive}" "${D}${bindir}/${PN}-${PV}/${VELA_MODELS_DIR}"
    done
    cp ${WORKDIR}/ssd_mobilenet_v1_quant_vela.tflite "${D}${bindir}/${PN}-${PV}/${VELA_MODELS_DIR}"

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
