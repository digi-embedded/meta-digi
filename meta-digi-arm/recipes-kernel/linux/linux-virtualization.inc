# Copyright (C) 2019 Digi International

# Apply kernel configuration required for Docker
do_configure_prepend() {
	mkdir -p ${B}

	kernel_conf_variable NAMESPACES y
	kernel_conf_variable MULTIUSER y
	kernel_conf_variable NET_NS y
	kernel_conf_variable NET y
	kernel_conf_variable PID_NS y
	kernel_conf_variable IPC_NS y
	kernel_conf_variable POSIX_MQUEUE y
	kernel_conf_variable UTS_NS y
	kernel_conf_variable CGROUPS y
	kernel_conf_variable CGROUP_CPUACCT y
	kernel_conf_variable CGROUP_DEVICE y
	kernel_conf_variable CGROUP_FREEZER y
	kernel_conf_variable CGROUP_SCHED y
	kernel_conf_variable CPUSETS y
	kernel_conf_variable SMP y
	kernel_conf_variable MEMCG y
	kernel_conf_variable KEYS y
	kernel_conf_variable VETH y
	kernel_conf_variable NETDEVICES y
	kernel_conf_variable NET_CORE y
	kernel_conf_variable BRIDGE y
	kernel_conf_variable BRIDGE_NETFILTER y
	kernel_conf_variable NETFILTER y
	kernel_conf_variable INET y
	kernel_conf_variable NETFILTER_ADVANCED y
	kernel_conf_variable NF_NAT_IPV4 y
	kernel_conf_variable NF_CONNTRACK_IPV4 y
	kernel_conf_variable NF_CONNTRACK y
	kernel_conf_variable IP_NF_FILTER y
	kernel_conf_variable IP_NF_IPTABLES y
	kernel_conf_variable IP_NF_TARGET_MASQUERADE y
	kernel_conf_variable IP_NF_NAT y
	kernel_conf_variable NETFILTER_XT_MATCH_ADDRTYPE y
	kernel_conf_variable NETFILTER_XT_MATCH_CONNTRACK y
	kernel_conf_variable NETFILTER_XT_MATCH_IPVS y
	kernel_conf_variable IP_VS y
	kernel_conf_variable IP_NF_NAT y
	kernel_conf_variable POSIX_MQUEUE y
	kernel_conf_variable OVERLAY_FS y

	sed -e "${CONF_SED_SCRIPT}" < '${WORKDIR}/defconfig' >> '${B}/.config'
}

KERNEL_MODULE_AUTOLOAD += "nf_conntrack_ipv6 openvswitch"
