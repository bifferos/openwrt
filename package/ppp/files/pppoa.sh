stop_interface_pppoa() {
	stop_interface_ppp "$1"
}

setup_interface_pppoa() {
	local iface="$1"
	local config="$2"

	local device
	config_get device "$config" device

	local vpi
	config_get vpi "$config" vpi

	local vci
	config_get vci "$config" vci

	for module in slhc ppp_generic pppoatm; do
		/sbin/insmod $module 2>&- >&-
	done

	local encaps
	config_get encaps "$config" encaps

	case "$encaps" in
		1|vc) encaps="vc-encaps" ;;
		*) encaps="llc-encaps" ;;
	esac

	local mtu
	config_get mtu "$config" mtu
	mtu=${mtu:-1500}

	start_pppd "$config" \
		plugin pppoatm.so ${vpi:-8}.${vci:-35} ${encaps} \
		mtu $mtu mru $mtu
}
