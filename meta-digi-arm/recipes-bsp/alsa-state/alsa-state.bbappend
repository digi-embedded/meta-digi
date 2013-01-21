FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}/${MACHINE}/:"

SRC_URI += "file://asound.inline_play.state \
	    file://asound.inline.state \
	    file://asound.micro_play.state \
            file://asound.micro.state \
            file://asound.play.state"
