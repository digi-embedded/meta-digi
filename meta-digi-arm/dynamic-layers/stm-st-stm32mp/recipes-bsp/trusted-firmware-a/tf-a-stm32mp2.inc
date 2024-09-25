FILESEXTRAPATHS:prepend := "${THISDIR}/tf-a-stm32mp:"

PROVIDES += "virtual/trusted-firmware-a"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit deploy
#inherit sign-stm32mp
inherit fip-utils-stm32mp2
#inherit external-dt


STAGING_EXTDT_DIR = "${TMPDIR}/work-shared/${MACHINE}/external-dt"

# Include TF-A config definitions
require tf-a-stm32mp2-config.inc

# ------------------------------------
# Set MBEDTLS support
TFA_MBEDTLS_DIR ?= "mbedtls"
# MBEDTLS v2.28.5
SRC_URI_MBEDTLS = "git://github.com/ARMmbed/mbedtls.git;protocol=https;destsuffix=git/${TFA_MBEDTLS_DIR};branch=mbedtls-2.28;name=mbedtls"
SRCREV_mbedtls = "47e8cc9db2e469d902b0e3093ae9e482c3d87188"
LIC_FILES_CHKSUM_MBEDTLS = "file://mbedtls/LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"
LICENSE_MBEDTLS = "Apache-2.0"
# Add MBEDTLS to our sources
SRC_URI:append = " ${@bb.utils.contains('SIGN_ENABLE', '1', '${SRC_URI_MBEDTLS}', '', d)}"
# Update license variables
LICENSE:append = "${@bb.utils.contains('SIGN_ENABLE', '1', ' & ${LICENSE_MBEDTLS}', '', d)}"
LIC_FILES_CHKSUM:append = "${@bb.utils.contains('SIGN_ENABLE', '1', ' ${LIC_FILES_CHKSUM_MBEDTLS}', '', d)}"
# Add mbed TLS to version
SRCREV_FORMAT:append = "${@bb.utils.contains('SIGN_ENABLE', '1', '_mbedtls', '', d)}"
# ------------------------------------

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD:pn-${PN} = "${WORKDIR}/build"

DEPENDS += "dtc-native openssl-native"
DEPENDS:append = " ${@bb.utils.contains('TF_A_ENABLE_DEBUG_WRAPPER', '1', 'stm32wrapper4dbg-native', '', d)}"

# Default log level
ST_TF_A_DEBUG ??= "1"
ST_TF_A_DEBUG_TRACE ??= "0"
ST_TF_A_LOG_LEVEL_RELEASE ??= "20"
ST_TF_A_LOG_LEVEL_DEBUG ??= "40"

# Configure make settings
EXTRA_OEMAKE += 'PLAT=${TFA_PLATFORM}'
EXTRA_OEMAKE += 'ARCH=${TFA_ARM_ARCH}'
EXTRA_OEMAKE += 'ARM_ARCH_MAJOR=${TFA_ARM_MAJOR}'
EXTRA_OEMAKE += 'CROSS_COMPILE=${TARGET_PREFIX}'
# Debug support
EXTRA_OEMAKE += "${@bb.utils.contains('ST_TF_A_DEBUG_TRACE', '1', 'DEBUG=${ST_TF_A_DEBUG}', '', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('ST_TF_A_DEBUG_TRACE', '1', 'LOG_LEVEL=${ST_TF_A_LOG_LEVEL_DEBUG}', 'LOG_LEVEL=${ST_TF_A_LOG_LEVEL_RELEASE}', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'TFA_EXTERNAL_DT=${STAGING_EXTDT_DIR}/tf-a', '', d)}"
# OPTEE in sysram
EXTRA_OEMAKE:append:stm32mp1common = " ${@bb.utils.contains('ST_OPTEE_IN_SYSRAM', '1', 'STM32MP1_OPTEE_IN_SYSRAM=1', '', d)}"

# Define default TF-A namings
TF_A_BASENAME ?= "tf-a"
TF_A_SUFFIX ?= "stm32"

# Output the ELF generated
ELF_DEBUG_ENABLE ?= ""
TF_A_ELF_SUFFIX = "elf"

BL1_NAME ?= "bl1/bl1"
BL1_ELF = "${BL1_NAME}.${TF_A_ELF_SUFFIX}"
BL1_BASENAME = "${@os.path.basename(d.getVar("BL1_NAME"))}"
BL1_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL1_NAME"))}"

BL2_NAME ?= "bl2/bl2"
BL2_ELF = "${BL2_NAME}.${TF_A_ELF_SUFFIX}"
BL2_BASENAME = "${@os.path.basename(d.getVar("BL2_NAME"))}"
BL2_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL2_NAME"))}"

BL31_NAME ?= "bl31/bl31"
BL31_ELF = "${BL31_NAME}.${TF_A_ELF_SUFFIX}"
BL31_BASENAME = "${@os.path.basename(d.getVar("BL31_NAME"))}"
BL31_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL31_NAME"))}"
BL31_SUFFIX ?= "bin"

BL32_NAME ?= "bl32/bl32"
BL32_ELF = "${BL32_NAME}.${TF_A_ELF_SUFFIX}"
BL32_BASENAME = "${@os.path.basename(d.getVar("BL32_NAME"))}"
BL32_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL32_NAME"))}"
BL32_SUFFIX ?= "bin"

DT_SUFFIX ?= "dtb"
FWCONFIG_NAME ?= "fw-config"

# Output the firwmare ddr
TF_A_FWDDR ?= "0"
TF_A_FWDDR:stm32mp25common = "1"

FWDDR_NAME ?= "ddr_pmu"
FWDDR_SUFFIX ?= "bin"

# Set default TF-A config
TF_A_CONFIG ?= ""

# Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ??= "1"

# Set default configuration to allow signing
TF_A_SIGN_SUFFIX ??= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"
TF_A_SIGN_OF ?= "0x00000001"
TF_A_SIGN_OF:stm32mp1common ?= "0x00000001"
TF_A_SIGN_OF:stm32mp25common ?= "0x00000001"
TF_A_SIGN_OF:stm32mp25revabcommon ?= "0x00000001"

TF_A_ENCRYPT_SUFFIX ??= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"
TF_A_ENCRYPT_DC ?= "0x0E5F2025"
TF_A_ENCRYPT_DC:stm32mp1common ?= "0x0E5F2025"
TF_A_ENCRYPT_DC:stm32mp25common ?= "0x25205f0e"
TF_A_ENCRYPT_DC:stm32mp25revabcommon ?= "0x25205f0e"

TF_A_ENCRYPT_IMGVER ?= "0"
TF_A_ENCRYPT_OF ?= "0x80000003"
TF_A_ENCRYPT_OF:stm32mp1common ?= "0x80000003"
TF_A_ENCRYPT_OF:stm32mp2common ?= "0x10000003"


# Set metadata generation
TF_A_ENABLE_METADATA ??= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', '1', '0', d)}"
TF_A_METADATA_NAME ?= "metadata"
TF_A_METADATA_SUFFIX ?= "bin"
TF_A_METADATA_BINARY ??= "${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"

TF_A_METADATA_TOOL ?= "tools/fwu_gen_metadata/fwumd_tool.py"
TF_A_METADATA_JSON ?= "plat/st/common/default_metadata.json"

# Configure specific build flags
EXTRA_OEMAKE += "${@bb.utils.contains('SIGN_ENABLE', '1', 'TRUSTED_BOARD_BOOT=1', '', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('SIGN_ENABLE', '1', 'MBEDTLS_DIR=${TFA_MBEDTLS_DIR}', '', d)}"
EXTRA_OEMAKE:append:stm32mp2common = " ${@bb.utils.contains('SIGN_ENABLE', '1', 'BRANCH_PROTECTION=0', '', d)} "

EXTRA_OEMAKE += "${@bb.utils.contains('ENCRYPT_ENABLE', '1', 'DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1', '', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('ENCRYPT_ENABLE', '1', bb.utils.contains('FIP_BL31_ENABLE', '1', 'ENCRYPT_BL31=1', '', d), '', d)} "

# Addons parameters for SIGN_TOOL
SIGN_TOOL_EXTRA ?= ""
SIGN_TOOL_EXTRA:stm32mp25common = "--header-version 2"
SIGN_TOOL_EXTRA:stm32mp25revabcommon = "--header-version 2"

# Specific for revA board
EXTRA_OEMAKE:append:stm32mp25revabcommon = " CONFIG_STM32MP25X_REVA=1 "

# -----------------------------------------------
# Handle TF-A config and set internal vars
#   TF_A_DEVICETREE
#   TF_A_EXTRA_OPTFLAGS
python () {
    import re

    tfaconfigflags = d.getVarFlags('TF_A_CONFIG')
    # The "doc" varflag is special, we don't want to see it here
    tfaconfigflags.pop('doc', None)
    tfaconfig = (d.getVar('TF_A_CONFIG') or "").split()
    tfabasename = d.getVar('TF_A_BASENAME')

    if not tfaconfig:
        raise bb.parse.SkipRecipe("TF_A_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('TF_A_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_DEVICETREE as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_EXTRA_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_EXTRA_OPTFLAGS as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_BINARIES') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_BINARIES as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_MAKE_TARGET') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_MAKE_TARGET as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_FILES') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_FILES as it is internal to TF_A_CONFIG var expansion.")

    if len(tfaconfig) > 0:
        for config in tfaconfig:
            for f, v in tfaconfigflags.items():
                if config == f:
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('TF_A_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[TF_A_CONFIG] Missing configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 5:
                        raise bb.parse.SkipRecipe('Only <DEVICETREE>,<EXTRA_OPTFLAGS>,<BINARY_BASENAME>,<MAKE_TARGET>,<FILES TYPE> can be specified!')
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to TF_A_DEVICETREE" % items[0])
                    d.appendVar('TF_A_DEVICETREE', items[0] + ',')
                    if len(items) > 1 and items[1]:
                        bb.debug(1, "Appending '%s' to TF_A_EXTRA_OPTFLAGS." % items[1])
                        d.appendVar('TF_A_EXTRA_OPTFLAGS', items[1] + ',')
                    else:
                        d.appendVar('TF_A_EXTRA_OPTFLAGS', '' + ',')
                    if len(items) > 2 and items[2]:
                        bb.debug(1, "Appending '%s' to TF_A_BINARIES." % items[2])
                        d.appendVar('TF_A_BINARIES', items[2] + ',')
                    else:
                        bb.debug(1, "Appending '%s' to TF_A_BINARIES." % tfabasename)
                        d.appendVar('TF_A_BINARIES', tfabasename + ',')
                    if len(items) > 3 and items[3]:
                        bb.debug(1, "Appending '%s' to TF_A_MAKE_TARGET." % items[3])
                        d.appendVar('TF_A_MAKE_TARGET', items[3] + ',')
                    else:
                        d.appendVar('TF_A_MAKE_TARGET', 'all' + ',')
                    if len(items) > 4 and items[4]:
                        bb.debug(1, "Appending '%s' to TF_A_FILES." % items[4])
                        d.appendVar('TF_A_FILES', items[4] + ',')
                    else:
                        d.appendVar('TF_A_FILES', 'bl2' + ',')
                    break

    # Manage case of signature:
    if d.getVar('SIGN_ENABLE') == "1":
        # If signature are activated, for winning space, the debug parameter will be remove and level of trace decrease
        if d.getVar('ST_TF_A_DEBUG_TRACE') == '1':
            bb.warn("TF-A SIGNATURE: force ST_TF_A_DEBUG_TRACE to '0' to disable DEBUG and decrease log level")
            d.setVar('ST_TF_A_DEBUG_TRACE', "0")
}

# -----------------------------------------------
# Enable use of work-shared folder
TFA_SHARED_SOURCES ??= "1"
STAGING_TFA_DIR = "${TMPDIR}/work-shared/${MACHINE}/tfa-source"
# Make sure to move ${S} to STAGING_TFA_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += "${S}"
do_unpack[cleandirs] += "${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '${STAGING_TFA_DIR}', '', d)}"
do_clean[cleandirs] += "${S}"
do_clean[cleandirs] += "${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '${STAGING_TFA_DIR}', '', d)}"
base_do_unpack:append () {
    # Specific part to update devtool-source class
    if bb.data.inherits_class('devtool-source', d):
        # We don't want to move the source to STAGING_TFA_DIR here
        if d.getVar('STAGING_TFA_DIR', d):
            d.setVar('STAGING_TFA_DIR', '${S}')

    shared = d.getVar("TFA_SHARED_SOURCES")
    if shared and oe.types.boolean(shared):
        # Copy/Paste from kernel class with adaptation to TFA var
        s = d.getVar("S")
        if s[-1] == '/':
            # drop trailing slash, so that os.symlink(tfasrc, s) doesn't use s as directory name and fail
            s=s[:-1]
        tfasrc = d.getVar("STAGING_TFA_DIR")
        if s != tfasrc:
            bb.utils.mkdirhier(tfasrc)
            bb.utils.remove(tfasrc, recurse=True)
            if d.getVar("EXTERNALSRC"):
                # With EXTERNALSRC S will not be wiped so we can symlink to it
                os.symlink(s, tfasrc)
            else:
                import shutil
                shutil.move(s, tfasrc)
                os.symlink(tfasrc, s)
}

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
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    if [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ]; then
                        soc_extra_opt="$(echo ${soc} | awk '{print toupper($0)}')=1"
                        soc_suffix="-${soc}"
                    fi
                done
            fi
            mkdir -p ${B}/${config}${soc_suffix}
            if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
                ${S}/${TF_A_METADATA_TOOL} jsonparse "${S}/${TF_A_METADATA_JSON}" -b "${B}/${config}${soc_suffix}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
            fi

            # Init specific ddr settings
            ddr_extra_opt=""
            if [ "${TF_A_FWDDR}" = "1" ]; then
                # Detect ddr type if it's present
                oe_runmake -C "${S}" BUILD_PLAT="${B}/${config}${soc_suffix}-${dt}" DTB_FILE_NAME="${dt}.dtb" ${extra_opt} ${soc_extra_opt} dtbs
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
                            bbwarn "Missing st,mem-name information for ${dt}"
                            ;;
                    esac
                    bbnote "${dt}: ${tf_a_make_target} -> ${ddr_extra_opt}"
                    # Copy TF-A ddr binary with explicit devicetree filename
                    if [ -n "${ddr_target}" ]; then
                        if [ -s "${S}/drivers/st/ddr/phy/firmware/bin/${ddr_target}_pmu_train.bin" ]; then
                            cp "${S}/drivers/st/ddr/phy/firmware/bin/${ddr_target}_pmu_train.bin" "${B}/${config}${soc_suffix}-${dt}/${FWDDR_NAME}-${dt}.${FWDDR_SUFFIX}"
                        else
                            bbwarn "Missing ddr firmware file ${ddr_target}_pmu_train.bin for ${dt}"
                        fi
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
                    encrypt_extra_opt="ENC_KEY=$(hexdump -e '/1 "%02x"' ${encrypt_key})"
            fi

            oe_runmake -C "${S}" BUILD_PLAT="${B}/${config}${soc_suffix}-${dt}" DTB_FILE_NAME="${dt}.dtb" ${extra_opt} ${soc_extra_opt} ${ddr_extra_opt} ${encrypt_extra_opt} ${tf_a_make_target}

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
                            [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && sign_key=$(echo ${SIGN_KEY_PATH_LIST} | cut -d',' -f${k})
                        done
                    fi
                    # Init default '-of' option for signing case
                    tf_a_sign_of_opt=""
                    dd if="${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" of=header.dump bs=1 count=4 skip=72 > /dev/null 2> /dev/null
                    temp_version=$(od -A o -t dI header.dump | head -n 1 | cut -d' ' -f2- | sed "s/ //g")
                    rm -f header.dump
                    [ "$(expr $temp_version / 65536)" = "2" ] && tf_a_sign_of_opt="-of ${TF_A_SIGN_OF}"
                    # Sign tf-a binary
                    echo "${SIGN_TOOL} \
                        -bin "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                        -o "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        ${SIGN_TOOL_EXTRA} \
                        ${tf_a_sign_of_opt}"
                    ${SIGN_TOOL} \
                        -bin "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                        -o "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        ${SIGN_TOOL_EXTRA} \
                        ${tf_a_sign_of_opt}
                    if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                        echo "${SIGN_TOOL} \
                            -bin "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                            -o "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key "${sign_key}" \
                            --type fsbl \
                            --silent \
                            ${SIGN_TOOL_EXTRA} \
                            ${tf_a_sign_of_opt}"
                        ${SIGN_TOOL} \
                            -bin "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                            -o "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key "${sign_key}" \
                            --type fsbl \
                            --silent \
                            ${SIGN_TOOL_EXTRA} \
                            ${tf_a_sign_of_opt}
                    fi
                fi

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
                    # Init default '-of' option for signing case
                    tf_a_sign_of_opt=""
                    dd if="${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" of=header.dump bs=1 count=4 skip=72 > /dev/null 2> /dev/null
                    temp_version=$(od -A o -t dI header.dump | head -n 1 | cut -d' ' -f2- | sed "s/ //g")
                    rm -f header.dump
                    [ "$(expr $temp_version / 65536)" = "2" ] && tf_a_sign_of_opt="-hv 2"
                    # Encrypt tf-a binary
                    echo '${SIGN_TOOL} \
                        -bin "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        -o "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        --enc-key ${encrypt_key} \
                        --enc-dc "${TF_A_ENCRYPT_DC}" \
                        --image-version "${TF_A_ENCRYPT_IMGVER}" \
                        -of "${TF_A_ENCRYPT_OF}" \
                        ${tf_a_sign_of_opt} '
                    ${SIGN_TOOL} \
                        -bin "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        -o "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        --enc-key ${encrypt_key} \
                        --enc-dc "${TF_A_ENCRYPT_DC}" \
                        --image-version "${TF_A_ENCRYPT_IMGVER}" \
                        -of "${TF_A_ENCRYPT_OF}" \
                        ${tf_a_sign_of_opt}
                    if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                        echo '${SIGN_TOOL} \
                            -bin "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            -o "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key ${sign_key} \
                            --type fsbl \
                            --silent \
                            --enc-key ${encrypt_key} \
                            --enc-dc "${TF_A_ENCRYPT_DC}" \
                            --image-version "${TF_A_ENCRYPT_IMGVER}" \
                            -of "${TF_A_ENCRYPT_OF}" \
                            ${tf_a_sign_of_opt}'
                        ${SIGN_TOOL} \
                            -bin "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            -o "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key ${sign_key} \
                            --type fsbl \
                            --silent \
                            --enc-key ${encrypt_key} \
                            --enc-dc "${TF_A_ENCRYPT_DC}" \
                            --image-version "${TF_A_ENCRYPT_IMGVER}" \
                            -of "${TF_A_ENCRYPT_OF}"\
                            ${tf_a_sign_of_opt}
                    fi
                fi
            fi
        done
    done

    if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
        ${S}/${TF_A_METADATA_TOOL} jsonparse "${S}/${TF_A_METADATA_JSON}" -b "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
    fi
}

do_deploy() {
    install -d ${DEPLOYDIR}
    install -d ${DEPLOYDIR}/arm-trusted-firmware

    unset i
    for config in ${TF_A_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list and tf-a basename
        dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
        tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
        tfa_file_type=$(echo ${TF_A_FILES} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Init soc suffix
            soc_suffix=""
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                done
            fi
            for file_type in ${tfa_file_type}; do
                case "${file_type}" in
                    bl2)
                        # Install TF-A binary
                        if [ -f "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" ]; then
                            install -m 644 "${B}/${config}${soc_suffix}-${dt}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/"
                            if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                                install -d "${DEPLOYDIR}/arm-trusted-firmware/debug"
                                install -m 644 "${B}/${config}${soc_suffix}-${dt}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/debug/"
                            fi
                        fi
                        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                            install -d "${DEPLOYDIR}/arm-trusted-firmware/debug"
                            if [ -f "${B}/${config}${soc_suffix}-${dt}/${BL2_ELF}" ]; then
                                install -m 644 "${B}/${config}${soc_suffix}-${dt}/${BL2_ELF}" "${DEPLOYDIR}/arm-trusted-firmware/debug/${tfa_basename}-${BL2_BASENAME_DEPLOY}${soc_suffix}-${config}.${TF_A_ELF_SUFFIX}"
                            fi
                        fi
                        if [ "${TF_A_FWDDR}" = "1" ]; then
                            install -d "${DEPLOYDIR}/arm-trusted-firmware/ddr"
                            # Install DDR firmware binary
                            if [ -f "${B}/${config}${soc_suffix}-${dt}/${FWDDR_NAME}-${dt}.${FWDDR_SUFFIX}" ]; then
                                if [ ! -s "${DEPLOYDIR}/arm-trusted-firmware/ddr/${FWDDR_NAME}-${dt}.${FWDDR_SUFFIX}" ]; then
                                    install -m 644 "${B}/${config}${soc_suffix}-${dt}/${FWDDR_NAME}-${dt}.${FWDDR_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/ddr/"
                                fi
                            fi
                        fi
                        ;;
                    bl31)
                        # Install BL31 files
                        install -d "${DEPLOYDIR}/arm-trusted-firmware/bl31"
                        # Install BL31 binary
                        if [ -f "${B}/${config}${soc_suffix}-${dt}/${BL31_BASENAME}.${BL31_SUFFIX}" ]; then
                            install -m 644 "${B}/${config}${soc_suffix}-${dt}/${BL31_BASENAME}.${BL31_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/bl31/${tfa_basename}-${BL31_BASENAME_DEPLOY}${soc_suffix}.${BL31_SUFFIX}"
                        fi
                        # Install BL31 devicetree
                        if [ -f "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-${BL31_BASENAME}.${DT_SUFFIX}" ]; then
                            install -m 644 "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-${BL31_BASENAME}.${DT_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/bl31/${dt}-${BL31_BASENAME}.${DT_SUFFIX}"
                        fi
                        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                            install -d "${DEPLOYDIR}/arm-trusted-firmware/bl31/debug"
                            if [ -f "${B}/${config}${soc_suffix}-${dt}/${BL31_ELF}" ]; then
                                install -m 644 "${B}/${config}${soc_suffix}-${dt}/${BL31_ELF}" "${DEPLOYDIR}/arm-trusted-firmware/bl31/debug/${tfa_basename}-${BL31_BASENAME_DEPLOY}${soc_suffix}-${config}.${TF_A_ELF_SUFFIX}"
                            fi
                        fi
                        ;;
                    bl32)
                        # Install BL32 files
                        install -d "${DEPLOYDIR}/arm-trusted-firmware/bl32"
                        # Install BL32 binary
                        if [ -f "${B}/${config}${soc_suffix}-${dt}/${BL32_BASENAME}.${BL32_SUFFIX}" ]; then
                            install -m 644 "${B}/${config}${soc_suffix}-${dt}/${BL32_BASENAME}.${BL32_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/bl32/${tfa_basename}-${BL32_BASENAME_DEPLOY}${soc_suffix}.${BL32_SUFFIX}"
                        fi
                        # Install BL32 devicetree
                        if [ -f "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-${BL32_BASENAME}.${DT_SUFFIX}" ]; then
                            install -m 644 "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-${BL32_BASENAME}.${DT_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/bl32/${dt}-${BL32_BASENAME}.${DT_SUFFIX}"
                        fi
                        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                            install -d "${DEPLOYDIR}/arm-trusted-firmware/bl32/debug"
                            if [ -f "${B}/${config}${soc_suffix}-${dt}/${BL32_ELF}" ]; then
                                install -m 644 "${B}/${config}${soc_suffix}-${dt}/${BL32_ELF}" "${DEPLOYDIR}/arm-trusted-firmware/bl32/debug/${tfa_basename}-${BL32_BASENAME_DEPLOY}${soc_suffix}-${config}.${TF_A_ELF_SUFFIX}"
                            fi
                        fi
                        ;;
                    fwconfig)
                        # Install fwconfig
                        install -d "${DEPLOYDIR}/arm-trusted-firmware/fwconfig"
                        if [ -f "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-${FWCONFIG_NAME}.${DT_SUFFIX}" ]; then
                            install -m 644 "${B}/${config}${soc_suffix}-${dt}/fdts/${dt}-${FWCONFIG_NAME}.${DT_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/fwconfig/${dt}-${FWCONFIG_NAME}-${config}.${DT_SUFFIX}"
                        fi
                        ;;
                esac
            done # for file_type in ${tfa_file_type}
        done # for dt in ${dt_config}
        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
            install -d "${DEPLOYDIR}/arm-trusted-firmware/debug"
            if [ -f "${B}/${config}${soc_suffix}-${dt}/${BL1_ELF}" ]; then
                install -m 644 "${B}/${config}${soc_suffix}-${dt}/${BL1_ELF}" "${DEPLOYDIR}/arm-trusted-firmware/debug/${tfa_basename}-${BL1_BASENAME_DEPLOY}-${config}.${TF_A_ELF_SUFFIX}"
            fi
        fi
    done # for config in ${TF_A_CONFIG}

    if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
        install -d "${DEPLOYDIR}/arm-trusted-firmware"
        if [ -f "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}" ]; then
            install -m 644 "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}" "${DEPLOYDIR}/arm-trusted-firmware/${TF_A_METADATA_BINARY}"
        fi
    fi
}
addtask deploy before do_build after do_compile