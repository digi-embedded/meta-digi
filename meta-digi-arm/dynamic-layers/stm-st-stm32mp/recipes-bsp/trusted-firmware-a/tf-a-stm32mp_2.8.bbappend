# Copyright (C) 2024, Digi International Inc.

do_deploy:append() {
    unset i
    for config in ${TF_A_CONFIG}; do
        i=$(expr $i + 1)
        dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
        tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            TF_A_FILENAME="${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}"
            if [ -f "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_FILENAME}" ]; then
                ln -s "arm-trusted-firmware/${TF_A_FILENAME}" "${DEPLOYDIR}"
            fi
        done
    done

    # Last value of 'dt' is good for metadata binary, so use that.
    if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
            if [ -f "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_METADATA_BINARY}" ]; then
                ln -s "arm-trusted-firmware/${TF_A_METADATA_BINARY}" "${DEPLOYDIR}/${TF_A_METADATA_NAME}-${dt}.${TF_A_METADATA_SUFFIX}"
            fi
    fi

    unset i
    for config in ${FIP_CONFIG}; do
        i=$(expr $i + 1)
        dt_config="$(echo ${FIP_DEVICETREE} | cut -d',' -f${i})"
        for dt in ${dt_config}; do
            FIP_FILENAME="${FIP_BASENAME}-${dt}-${config}${FIP_SIGN_SUFFIX}.${FIP_SUFFIX}"
            if [ -f "${DEPLOYDIR}/fip/${FIP_FILENAME}" ]; then
                ln -s "fip/${FIP_FILENAME}" "${DEPLOYDIR}"
            fi
        done
    done
}
