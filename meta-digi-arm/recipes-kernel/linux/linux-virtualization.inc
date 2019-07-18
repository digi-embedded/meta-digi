# Copyright (C) 2019 Digi International

# Apply kernel configuration required for Docker
do_configure_prepend() {
	mkdir -p ${B}

	kernel_conf_variable NETFILTER_XT_MATCH_ADDRTYPE m
	kernel_conf_variable IP_NF_FILTER m
	kernel_conf_variable NF_CONNTRACK_IPV4 y
	kernel_conf_variable DM_THIN_PROVISIONING m
	kernel_conf_variable IP_NF_NAT m
	kernel_conf_variable IP_NF_TARGET_MASQUERADE m
	kernel_conf_variable OVERLAY_FS y
	kernel_conf_variable CGROUP_DEVICE y
	kernel_conf_variable CPUSETS y
	kernel_conf_variable PROC_PID_CPUSET y
	kernel_conf_variable RESOURCE_COUNTERS y
	kernel_conf_variable CGROUP_MEM_RES_CTLR y
	kernel_conf_variable CGROUP_SCHED y
	kernel_conf_variable FAIR_GROUP_SCHED y
	kernel_conf_variable RT_GROUP_SCHED y
	kernel_conf_variable CHECKPOINT_RESTORE y
	kernel_conf_variable NAMESPACES y
	kernel_conf_variable UTS_NS y
	kernel_conf_variable IPC_NS y
	kernel_conf_variable USER_NS y
	kernel_conf_variable PID_NS y
	kernel_conf_variable NET_NS y
	kernel_conf_variable CLS_CGROUP m
	kernel_conf_variable BLK_CGROUP y
	kernel_conf_variable NETPRIO_CGROUP m
	kernel_conf_variable DEVPTS_MULTIPLE_INSTANCES y
	kernel_conf_variable VIRTIO_BLK y
	kernel_conf_variable SCSI_VIRTIO y
	kernel_conf_variable VIRTIO_NET m
	kernel_conf_variable HVC_DRIVER y
	kernel_conf_variable VIRTIO_CONSOLE y
	kernel_conf_variable HW_RANDOM_VIRTIO m
	kernel_conf_variable VIRTIO y
	kernel_conf_variable VIRTIO_RING y
	kernel_conf_variable VIRTIO_PCI y
	kernel_conf_variable VIRTIO_BALLOON y
	kernel_conf_variable VIRTIO_MMIO y
	kernel_conf_variable BLK_DEV_LOOP y
	kernel_conf_variable NLS_CODEPAGE_437 y
	kernel_conf_variable NLS_ISO8859_1 y
	kernel_conf_variable VFAT_FS y
	kernel_conf_variable RD_GZIP y
	kernel_conf_variable VETH y
	kernel_conf_variable MACVLAN y
	kernel_conf_variable OPENVSWITCH m
	kernel_conf_variable NET_SCH_INGRESS m
	kernel_conf_variable NET_ACT_POLICE m
	kernel_conf_variable BRIDGE_NF_EBTABLES m
	kernel_conf_variable BRIDGE_EBT_T_NAT m
	kernel_conf_variable NETFILTER_XT_TARGET_CHECKSUM m
	kernel_conf_variable CHECKPOINT_RESTORE y
	kernel_conf_variable CGROUP_FREEZER y
	kernel_conf_variable BRIDGE_NETFILTER y
	kernel_conf_variable NETFILTER_XT_MATCH_IPVS m

	# Symbol dependencies
	kernel_conf_variable IP_VS y
	kernel_conf_variable NETFILTER_ADVANCED y
	kernel_conf_variable NETFILTER_NETLINK_LOG y
	kernel_conf_variable NF_CONNTRACK y
	kernel_conf_variable NETFILTER_XTABLES y
	kernel_conf_variable NF_LOG_IPV4 y
	kernel_conf_variable NF_REJECT_IPV4 y
	kernel_conf_variable IP_NF_IPTABLES y
	kernel_conf_variable NF_LOG_IPV6 y
	kernel_conf_variable NAMESPACES y
	kernel_conf_variable EXPERT y
	kernel_conf_variable NETFILTER_ADVANCED y
	kernel_conf_variable NF_LOG_ARP y
	kernel_conf_variable NF_CONNTRACK_FTP m
	kernel_conf_variable NF_CONNTRACK_IRC m
	kernel_conf_variable NF_CONNTRACK_SIP m
	kernel_conf_variable NF_CT_NETLINK m
	kernel_conf_variable NETFILTER_XT_MARK m
	kernel_conf_variable NETFILTER_XT_TARGET_CHECKSUM m
	kernel_conf_variable NETFILTER_XT_TARGET_LOG m
	kernel_conf_variable NETFILTER_XT_TARGET_NFLOG m
	kernel_conf_variable NETFILTER_XT_TARGET_TCPMSS m
	kernel_conf_variable NETFILTER_XT_MATCH_CONNTRACK m
	kernel_conf_variable NETFILTER_XT_MATCH_POLICY m
	kernel_conf_variable NETFILTER_XT_MATCH_STATE m
	kernel_conf_variable NF_REJECT_IPV6 y
	kernel_conf_variable IP_NF_TARGET_REJECT m
	kernel_conf_variable IP_NF_MANGLE m
	kernel_conf_variable NF_CONNTRACK_IPV6 m
	kernel_conf_variable IP6_NF_IPTABLES y
	kernel_conf_variable NET_SCHED y
	kernel_conf_variable NET_SCH_INGRESS m
	kernel_conf_variable NET_CLS_ACT y
	kernel_conf_variable NET_ACT_POLICE m

	sed -e "${CONF_SED_SCRIPT}" < '${WORKDIR}/defconfig' >> '${B}/.config'
}

KERNEL_MODULE_AUTOLOAD += "nf_conntrack_ipv6 openvswitch"
