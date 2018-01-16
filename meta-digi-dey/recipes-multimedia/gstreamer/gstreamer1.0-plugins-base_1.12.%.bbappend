FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

# ion allocator will be enabled only when detecting that ion.h exists, which
# is built out from kernel. For now, ion allocator is supported on mx7ulp.
DEPENDS_append_mx7ulp = " virtual/kernel"

SRC_URI_IMX_PATCHES = " \
    file://0001-basetextoverlay-make-memory-copy-when-video-buffer-s.patch \
    file://0002-gstplaysink-don-t-set-async-of-custom-text-sink-to-f.patch \
    file://0005-gstplaybin-remove-default-deinterlace-flag.patch \
    file://0006-taglist-not-send-to-down-stream-if-all-the-frame-cor.patch \
    file://0007-handle-audio-video-decoder-error.patch \
    file://0008-gstaudiobasesink-print-warning-istead-of-return-ERRO.patch \
    file://0009-MMFMWK-7030-Linux_MX6QP_ARD-IMXCameraApp-When-Enable.patch \
    file://0010-MMFMWK-7259-Remove-dependence-on-imx-plugin-git.patch \
    file://0011-Disable-orc-optimization-for-lib-video-in-plugins-ba.patch \
    file://0012-Remove-phymem-allocator-from-base-to-bad.patch \
    file://0013-dmabuf-set-fd-memory-to-keep-mapped.patch \
    file://0014-fdmemory-need-unmap-if-mapping-flags-are-not-subset-.patch \
    file://0015-basetextoverlay-need-avoid-idx-exceed-me.patch \
"

SRC_URI_append_mx6 = "${SRC_URI_IMX_PATCHES}"
SRC_URI_append_mx7 = "${SRC_URI_IMX_PATCHES}"
