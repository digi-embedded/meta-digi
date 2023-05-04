python __anonymous() {
    RPN = d.getVar("REMOVE_POSTINST_RPN")
    if RPN is None:
        RPN = d.getVar("PN")
    if RPN:
        d.setVar('pkg_postinst_ontarget:%s' % RPN, "")
}
