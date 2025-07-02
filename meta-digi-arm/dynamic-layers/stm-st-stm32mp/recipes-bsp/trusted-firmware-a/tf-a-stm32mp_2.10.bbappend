#
# Copyright (C) 2022-2025, Digi International Inc.
#

# Inherit custom DIGI sign class to skip signing tool and key parsing restrictions
inherit sign-stm32mp-digi

# Select internal or Github TF-A repo
TFA_URI_STASH = "${DIGI_MTK_GIT}/emp/arm-trusted-firmware.git;protocol=ssh"
TFA_URI_GITHUB = "${DIGI_GITHUB_GIT}/arm-trusted-firmware.git;protocol=https"
TFA_GIT_URI ?= "${@oe.utils.conditional('DIGI_INTERNAL_GIT', '1' , '${TFA_URI_STASH}', '${TFA_URI_GITHUB}', d)}"

SRCBRANCH = "v2.10/stm32mp/maint"
SRCREV = "${AUTOREV}"

SRC_URI = " \
    ${TFA_GIT_URI};branch=${SRCBRANCH} \
"

# stm32mp15 = header-version 1
SIGN_TOOL_EXTRA_soc:ccmp15 = " ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '', d)}"
# stm32mp13 = header-version 2
SIGN_TOOL_EXTRA_soc:ccmp13 = " ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"
# stm32mp2 = header-version 2.2
SIGN_TOOL_EXTRA_soc:stm32mp2common = " --header-version 2.2 ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"

TF_A_CONFIG[optee-nand] = "\
    ${STM32MP_DT_FILES_NAND},\
    ${TF_A_CONFIG_OPTS_optee} ${TF_A_CONFIG_OPTS_features} ${TF_A_CONFIG_OPTS_fwupdate} STM32MP_RAW_NAND=1 STM32MP_USB_PROGRAMMER=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NAND}' if ${TF_A_MTD_START_OFFSET_NAND} else ''},\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
TF_A_CONFIG[opteemin-nand] ?= "\
    ${STM32MP_DT_FILES_NAND},\
    ${TF_A_CONFIG_OPTS_optee} ${TF_A_CONFIG_OPTS_features} ${TF_A_CONFIG_OPTS_fwupdate} STM32MP_RAW_NAND=1 STM32MP_USB_PROGRAMMER=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NAND}' if ${TF_A_MTD_START_OFFSET_NAND} else ''},\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
# TF_A_CONFIG[uart] (same as 'optee-programmer-uart')
TF_A_CONFIG[uart] ?= "\
    ${STM32MP_DEVICETREE_PROGRAMMER},\
    ${TF_A_CONFIG_OPTS_optee} STM32MP_UART_PROGRAMMER=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
# TF_A_CONFIG[usb] (same as 'optee-programmer-uart')
TF_A_CONFIG[usb] ?= "\
    ${STM32MP_DEVICETREE_PROGRAMMER},\
    ${TF_A_CONFIG_OPTS_optee} STM32MP_USB_PROGRAMMER=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"

DEPENDS += " \
    ${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'trustfence-sign-tools-native', '', d)} \
"

# This dependency is required so that the PKI generation completes before
# proceeding with set_fip_sign_key() where we extract the password that
# is later used on the do_deploy of the fip-utils-stm32mp.bbclass.
do_install[depends] = " \
    trustfence-sign-tools-native:do_populate_sysroot \
    openssl-native:do_populate_sysroot \
"

# Generate PKI tree if it doesn't exist.
# This is an prepend to do_compile because in this recipe, the keys
# must be ready before that.
do_generate_pki_tree() {
	if ${@oe.utils.conditional('TRUSTFENCE_SIGN','1','true','false',d)}; then
		check_gen_pki_tree
	fi
}
addtask generate_pki_tree before do_compile after do_configure

# Obtain password to use in TF-A generation
# Get password from file using the given key index
do_compile[prefuncs] += "${@oe.utils.conditional('TRUSTFENCE_SIGN', '1', 'set_tfa_sign_key', '', d)}"
python set_tfa_sign_key() {
    passfile = d.getVar('TRUSTFENCE_PASSWORD_FILE')
    if (os.path.isfile(passfile)):
        with open(passfile, "r") as file:
            p = file.read().strip()
            if (p):
                d.setVar('SIGN_KEY_PASS', p)
}

TF_A_SOC_MATCH ?= " ${DIGI_SOM} "

do_compile() {
    unset LDFLAGS
    unset CFLAGS
    unset CPPFLAGS

    unset i
    for config in ${TF_A_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list, extra make options and tf-a basename
        dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
        extra_opt=$(echo ${TF_A_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
        tf_a_make_target=$(echo ${TF_A_MAKE_TARGET} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Init specific soc settings
            soc_extra_opt=""
            soc_suffix=""
            soc_name=""
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    if [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ]; then
                        soc_extra_opt="$(echo ${soc} | awk '{print toupper($0)}')=1"
                        soc_suffix="-${soc}"

                        SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA}"
                        case ${soc} in
                        stm32mp13)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp13}"
                            ;;
                        stm32mp15)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp15}"
                            if echo ${config} | grep -q 'optee-'; then
                                soc_extra_opt="${soc_extra_opt} STM32MP1_OPTEE_IN_SYSRAM=1"
                            fi
                            ;;
                        stm32mp21)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp21}"
                            ;;
                        stm32mp23)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp23}"
                            ;;
                        stm32mp25)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp25}"
                            ;;
                        esac
                    fi
                done
            fi
            mkdir -p ${B}/${config}${soc_suffix}-${dt}
            if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
                rm -rf "${B}/${config}${soc_suffix}-${dt}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
                ${TF_A_METADATA_TOOL} ${TF_A_METADATA_TOOL_ARGS} "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
            fi

            # generate dt to check the content
            oe_runmake -C "${S}" BUILD_PLAT="${B}/${config}${soc_suffix}-${dt}" DTB_FILE_NAME="${dt}.dtb" ${extra_opt} ${soc_extra_opt} dtbs

            # check which pmic1l is present on dtb
            pcmi1_present=$(${STAGING_BINDIR_NATIVE}/fdtdump ${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-bl2.dtb 2>/dev/null | grep  -c "st,stpmic1l" || ${HOSTTOOLS_DIR}/true)
            if [ -f "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-bl2.dtb" ]; then
                if [ $pcmi1_present -gt 0 ]; then
                    # st pmic1l is present, need to force to compilation with specific pcmi1l optionn
                    soc_extra_opt="${soc_extra_opt} STM32MP_STPMIC1L=1"
                fi
            fi

            # Init specific ddr settings
            ddr_extra_opt=""
            if [ "${TF_A_FWDDR}" = "1" ]; then
                # Detect ddr type if it's present
                if [ -f "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-bl2.dtb" ]; then
                    ddr_dtb_node=$(${STAGING_BINDIR_NATIVE}/fdtget -l ${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-bl2.dtb /soc | grep ddr | head -n 1)
                    ddr_propertie=$(${STAGING_BINDIR_NATIVE}/fdtget ${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-bl2.dtb /soc/${ddr_dtb_node} st,mem-name || echo "none")
                    ddr_target=""
                    # potentials value of ddr_propertie:
                    # DDR3 16bits
                    # DDR4 32bits
                    # DDR4 8Gbits
                    # LPDDR4 32bits
                    case ${ddr_propertie} in
                        DDR3*)
                            ddr_extra_opt=" STM32MP_DDR3_TYPE=1 "
                            ddr_target="ddr3"
                            ;;
                        DDR4*)
                            ddr_extra_opt=" STM32MP_DDR4_TYPE=1 "
                            ddr_target="ddr4"
                            ;;
                        LPDDR4*)
                            ddr_extra_opt=" STM32MP_LPDDR4_TYPE=1 "
                            ddr_target="lpddr4"
                            ;;
                        *)
                            bbfatal "Missing st,mem-name information for ${dt}"
                            ;;
                    esac
                    bbnote "${dt}: ${tf_a_make_target} -> ${ddr_extra_opt}"
                    # Copy TF-A ddr binary with explicit devicetree filename
                    if [ -s "${FWDDR_DIR}/${ddr_target}_pmu_train.bin" ]; then
                        install -m 644 "${FWDDR_DIR}/${ddr_target}_pmu_train.bin" "${B}/${config}${soc_suffix}-${dt}/${FWDDR_NAME}-${dt}-${config}.${FWDDR_SUFFIX}"
                    else
                        bbfatal "Missing ddr firmware file ${ddr_target}_pmu_train.bin for ${dt}"
                    fi
                fi
            fi

            encrypt_extra_opt=""
            if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                    encrypt_key="${ENCRYPT_FIP_KEY_PATH_LIST}"
                    if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                        unset k
                        for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                            k=$(expr $k + 1)
                            [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FIP_KEY_PATH_LIST} | cut -d',' -f${k})
                        done
                    fi
                    if [ "$(file "${encrypt_key}" | sed 's#.*: \(.*\)$#\1#')" = "ASCII text" ]; then
                        # The encryption key is already available in hexadecimal format, so just extract it from file
                        encrypt_key="$(cat ${encrypt_key})"
                    else
                        encrypt_key="$(hexdump -e '/1 "%02x"' ${encrypt_key})"
                    fi
                    encrypt_extra_opt="ENC_KEY=${encrypt_key}"
            fi

            oe_runmake -C "${S}" BUILD_PLAT="${B}/${config}${soc_suffix}-${dt}" DTB_FILE_NAME="${dt}.dtb" ${extra_opt} ${soc_extra_opt} ${ddr_extra_opt} ${encrypt_extra_opt} ${tf_a_make_target}
            if [ -f "${B}/${config}${soc_suffix}-${dt}/bl2.bin" ]; then
                    cp "${B}/${config}${soc_suffix}-${dt}/bl2.bin" "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.bin"
            fi
            # Copy TF-A binary with explicit devicetree filename
            if [ -f "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}.${TF_A_SUFFIX}" ]; then
                cp "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}.${TF_A_SUFFIX}" "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}"
                if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    stm32wrapper4dbg -s "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}.${TF_A_SUFFIX}" -d "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}"
                fi

                if [ "${SIGN_ENABLE}" = "1" ]; then
                    # Init sign key for signing tools
                    sign_key="${SIGN_KEY_PATH_LIST}"
                    if [ -n "${STM32MP_SOC_NAME}" ]; then
                        unset k
                        for soc in ${STM32MP_SOC_NAME}; do
                            k=$(expr $k + 1)
                            if [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] || [ "$(echo ${dt} | grep -c ${TF_A_SOC_MATCH})" -eq 1 ] ;then
                                sign_key=$(echo ${SIGN_KEY_PATH_LIST} | cut -d',' -f${k})
                            fi
                        done
                    fi
                    # Init default encryption options for signing tool
                    tf_a_encrypt_opts=""
                    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                        # Init encrypt key for signing tools
                        encrypt_key="${ENCRYPT_FSBL_KEY_PATH_LIST}"
                        if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                            unset k
                            for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                                k=$(expr $k + 1)
                                [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FSBL_KEY_PATH_LIST} | cut -d',' -f${k})
                            done
                        fi
                        # Set encryption options for signing tools
                        tf_a_encrypt_opts="\
                            --enc-key ${encrypt_key} \
                            --enc-dc ${TF_A_ENCRYPT_DC} \
                            --image-version ${TF_A_ENCRYPT_IMGVER} \
                            "
                    fi
                    # Sign tf-a binary
                    bbnote "${SIGN_TOOL} \
                        -bin "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                        -o "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        ${SIGN_TOOL_EXTRA_soc} \
                        ${tf_a_encrypt_opts} "

                    ${SIGN_TOOL} \
                        -bin "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                        -o "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        ${SIGN_TOOL_EXTRA_soc} \
                        ${tf_a_encrypt_opts}
                    if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                         bbnote "${SIGN_TOOL} \
                            -bin "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                            -o "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key "${sign_key}" \
                            --type fsbl \
                            --silent \
                            ${SIGN_TOOL_EXTRA_soc} \
                            ${tf_a_encrypt_opts}"

                        ${SIGN_TOOL} \
                            -bin "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                            -o "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key "${sign_key}" \
                            --type fsbl \
                            --silent \
                            ${SIGN_TOOL_EXTRA_soc} \
                            ${tf_a_encrypt_opts}
                    fi
                fi
            fi
        done
    done

    if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
        rm -rf "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
        ${TF_A_METADATA_TOOL} ${TF_A_METADATA_TOOL_ARGS} "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
    fi
}

# The purpose of this function is to create symlinks to the files needed
# by the uuu installer that are located in subdirectories.
deploy_symlinks_atf() {
	# Remove trailing slash (/) from ST variable
	TF_A_BASEDIR="$(echo ${FIP_DIR_TFA_BASE} | cut -c2-)"
	unset i
	for config in ${TF_A_CONFIG}; do
		i=$(expr $i + 1)
		# Initialize devicetree list and tf-a basename
		dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
		tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
		for dt in ${dt_config}; do
			TF_A_FILENAME="${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}"
			if [ -f "${DEPLOYDIR}/${TF_A_BASEDIR}/${TF_A_FILENAME}" ]; then
				# symlink TF-A
				ln -sf "${TF_A_BASEDIR}/${TF_A_FILENAME}" "${DEPLOYDIR}"
			fi
		done
	done

	# Last value of 'dt' is good for metadata binary, so use that.
	if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
		if [ -f "${DEPLOYDIR}/${TF_A_BASEDIR}/${TF_A_METADATA_BINARY}" ]; then
			# symlink metadata
			ln -sf "${TF_A_BASEDIR}/${TF_A_METADATA_BINARY}" "${DEPLOYDIR}/${TF_A_METADATA_NAME}-${MACHINE}.${TF_A_METADATA_SUFFIX}"
		fi
	fi
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}"
do_deploy() {
    export_binaries ${DEPLOYDIR}${FIP_DIR_TFA_BASE}
    deploy_symlinks_atf
}
