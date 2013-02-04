#!/bin/sh

# Topology
#VM1                                   VM2
#eth0        - tap0  br0          tap2 - eth0
#192.168.33.2        192.168.33.1        192.168.33.3
#eth1        - tap1  br1          tap3 - eth1
#192.168.34.2        192.168.34.1        192.168.34.3

LABNAME="v6eval"
PROGNAME=$(readlink -f $0)
PROGARGS="$@"

ROOT=$(readlink -f ${ROOT:-/})
LINUX=$(readlink -f ${LINUX:-./linux})

WHICH=$(which which)
DEPS="screen brctl start-stop-daemon kvm cu socat perl"
CHROOTDEPS="ip"

HOMESHARE=$PWD/home

TAPS=""

info() {
    echo "[1;34m[+] $@[0m"
}

error() {
    echo "[1;31m[+] $@[0m"
}

# Setup a TMP directory
setup_tmp() {
    TMP=$(mktemp -d)
    trap "rm -rf $TMP" EXIT
    info "TMP is $TMP"
}

# Check for dependencies needed by this tool
check_dependencies() {
    for dep in $DEPS; do
        $WHICH $dep 2> /dev/null > /dev/null || {
            error "Missing dependency: $dep"
            exit 1
        }
    done
    [ -d $ROOT ] || {
        error "Chroot $ROOT does not exists"
    }
    for dep in $CHROOTDEPS; do
        PATH=$ROOT/usr/local/bin:$ROOT/usr/bin:$ROOT/bin:$ROOT/sbin:$ROOT/usr/local/sbin:$ROOT/usr/sbin \
            $WHICH $dep 2> /dev/null > /dev/null || {
            error "Missing dependency: $dep (in $ROOT)"
            exit 1
        }
    done
}

# Run our lab in screen
setup_screen() {
    [ x"$TERM" = x"screen" ] || \
        exec screen -ln -S $LABNAME -c /dev/null -t main "$PROGNAME" "$PROGARGS"
	#exec tmux new-session -s "vm" -n "main" "$PROGNAME $PROGARGS"
    sleep 1
    screen -X caption always "%{= wk}%-w%{= BW}%n %t%{-}%+w %-="
    screen -X zombie cr
}

# Generate  MAC address. Needs input seed
# $1   - input seed
# $mac - output MAC address
generate_mac() {
	mac=$(echo $1 | sha1sum | awk '{print "52:54:" substr($1,0,2) ":" substr($1, 2, 2) ":" substr($1, 4, 2) ":" substr($1, 6, 2)}')
}

# Start a VM
start_vm() {
    info "Start VM $1"
    name="$1"
    shift

    netargs=""
    # Add 2 tap devices to the VM
    # The second one can be used to test routing functionalities.
    for net in $NET; do
	IFACE=$(tunctl -b)
	TAPS="$TAPS $IFACE"
	generate_mac "$name-$net-$IFACE"
	if [ "$name" == "r2" ]; then
		mac="00:00:cc:dd:ee:ff"
	fi
	netargs="-netdev type=tap,id=guest0,ifname=$IFACE -device virtio-net-pci,netdev=guest0,mac=$mac"
	ip link set up dev "$IFACE"

	brctl addif br0 "$IFACE"
	#ip a a 192.168.33.1/24 dev br0

	IFACE=$(tunctl -b)
	TAPS="$TAPS $IFACE"
	generate_mac "$name-$net-$IFACE"
	netargs="$netargs -netdev type=tap,id=guest1,ifname=$IFACE -device virtio-net-pci,netdev=guest1,mac=$mac"
	ip link set up dev "$IFACE"

	brctl addif br1 "$IFACE"
	#ip a a 192.168.34.1/24 dev br1
    done

    # /root is mounted with version 9p2000.u to allow access to /dev,
    # /sys and to mount new partitions over them. This is not the case
    # for 9p2000.L.
    screen -t $name \
        start-stop-daemon --make-pidfile --pidfile "$TMP/vm-$name.pid" \
        --start --startas $($WHICH kvm) -- \
        -nodefconfig -no-user-config -nodefaults \
        -m 512m \
        -display none \
        \
        -chardev stdio,id=charserial0,signal=off \
        -device isa-serial,chardev=charserial0,id=serial0 \
        -chardev socket,id=charserial1,path=$TMP/vm-$name-serial.pipe,server,nowait \
        -device isa-serial,chardev=charserial1,id=serial1 \
        \
        -chardev socket,id=con0,path=$TMP/vm-$name-console.pipe,server,nowait \
        -mon chardev=con0,mode=readline,default \
        \
        -fsdev local,security_model=passthrough,id=fsdev-root,path=${ROOT},readonly \
        -device virtio-9p-pci,id=fs-root,fsdev=fsdev-root,mount_tag=/dev/root \
        -fsdev local,security_model=none,id=fsdev-home,path=${HOMESHARE} \
        -device virtio-9p-pci,id=fs-home,fsdev=fsdev-home,mount_tag=homeshare \
        -fsdev local,security_model=none,id=fsdev-lab,path=$(dirname "$PROGNAME") \
        -device virtio-9p-pci,id=fs-lab,fsdev=fsdev-lab,mount_tag=labshare \
        \
        -gdb unix:$TMP/vm-$name-gdb.pipe,server,nowait \
        -kernel $LINUX \
        -append "init=$PROGNAME console=ttyS0 uts=$name root=/dev/root rootflags=trans=virtio,version=9p2000.u ro rootfstype=9p POSTINIT=\"$POSTINIT\"" \
        $netargs \
        "$@"
    echo "GDB server listening on.... $TMP/vm-$name-gdb.pipe"
    echo "monitor listening on....... $TMP/vm-$name-console.pipe"
    echo "ttyS1 listening on......... $TMP/vm-$name-serial.pipe"

    screen -X select 0
}

display_help() {
    cat <<EOF

Some screen commands :
 C-a d     - Detach the screen (resume with screen -r $LABNAME)
 C-a "     - Select a window
 C-a space - Next window
 C-a C-a   - Previous window
EOF
    echo "Press enter to exit the lab"
    read a
}

cleanup() {
    for pid in $TMP/*.pid; do
        kill -15 -$(cat $pid) 2> /dev/null || true
    done
    sleep 1
    for pid in $TMP/*.pid; do
        kill -9 -$(cat $pid) 2> /dev/null || true
    done
    for iface in "$TAPS"; do
	    tunctl -d "$iface"
    done

    ifconfig br0 down
    ifconfig br1 down

    brctl delbr br0
    brctl delbr br1

    rm -rf $TMP # sh does not seem to handle "trap EXIT"
    screen -X quit
}

export STATE=${STATE:-0}
case $$,$STATE in
    1,0)
        # Initrd
        info "Setup hostname"
        hostname ${uts}
        info "Set path"
        export TERM=screen
        export HOME=/tmp/home
        export PATH=/usr/local/bin:/usr/bin:/bin:/sbin:/usr/local/sbin:/usr/sbin:$HOME/bin

        info "Setup overlayfs"
        mount -t tmpfs tmpfs /tmp -o rw
        mount -n -t proc  proc /proc
        mount -n -t sysfs sys /sys

        info "Mount home directory on /root"
	mkdir /tmp/home/
        mount -t 9p homeshare /tmp/home -o trans=virtio,version=9p2000.L,access=0,rw

        # In chroot
        info "Clean out /tmp and /run directories"
        for fs in /run /var/run /var/tmp /var/log; do
            mount -t tmpfs tmpfs $fs -o rw,nosuid,nodev
        done

        info "Setup interfaces"
        for intf in /sys/class/net/*; do
            intf=$(basename $intf)
            ip a l dev $intf 2> /dev/null >/dev/null || continue
            case $intf in
                lo|eth*|dummy*)
                    ip link set up dev $intf
                    ;;
            esac
        done
	# Need devpts for remote logins
	mkdir -p /dev/pts
	mount -t devpts none /dev/pts
	# Also, cu utility needs write access
	mount -t tmpfs none /usr/spool/uucp

	
        info "Setup terminal"
        export STATE=2
        exec setsid /sbin/agetty -L ttyS0 -a root -l "$PROGNAME" -i 115200
        ;;
    1,2)
        export TERM=screen

        info "Lab specific setup"
        export STATE=3
        . "$PROGNAME"

        info "Setup additional commands"
        cat <<EOF > ~/bin/reboot
#!/bin/sh
echo b > /proc/sysrq-trigger
EOF
        case $uts in
            r*)
                ;;
        esac

	if [ "$POSTINIT" ]; then
		info "Running postinit: $POSTINIT"
		"$POSTINIT"
	fi

	if [ $uts == "r1" ]; then
		echo 1 > /proc/sys/net/ipv6/conf/eth0/disable_ipv6
	fi

	modprobe slip
	mount -o bind /tmp/home/etc/ /etc

	# Second VM is the tested one
	if [ $uts == "r2" ]; then

		/etc/rc.d/sshd start

		# Wait for remote login on serial line
		while true; do
			#agetty -8 -iL ttyS1 9600 vt102 --nohostname --noclear 
			break;
		done
	fi


        while true; do
            info "Spawning a shell"
            cd $HOME
            export SSH_TTY=$(tty)
            if [ -f $HOME/.zshrc ]; then
                /bin/zsh -i
            else
                /bin/bash -i
            fi || sleep 1
        done
        ;;
    *,3)
        # Specific setup for this lab
        info "Enable forwarding"
        sysctl -w net.ipv4.ip_forward=1

        info "Setup IP addresses"
        case $uts in
            r1)
                ip addr add 192.168.33.2/24 dev eth0
		slattach -p slip -s 9600 /dev/ttyS1 &
		sleep 1
		ifconfig sl0 192.168.2.1 pointopoint 192.168.2.2 up
		#ip addr add 192.168.34.2/24 dev eth1
		ip l s down dev eth1
                ;;
            r2)
                ip addr add 192.168.33.3/24 dev eth0
		#ip addr add 192.168.34.3/24 dev eth1
		slattach -p slip -s 9600 /dev/ttyS1 &
		sleep 1
		ifconfig sl0 192.168.2.2 pointopoint 192.168.2.1 up
		ip l s down dev eth1
                ;;
        esac

        ;;
    *,*)
        [ $(id -u) != 0 ] || {
            error "You should not run this as root, unless you use tap devices"
            #exit 1
        }
        check_dependencies
        setup_screen
        setup_tmp

	mkdir -p "$HOMESHARE/bin"

	brctl addbr br0
	brctl addbr br1
	ifconfig br0 up
	ifconfig br1 up

        sleep 0.3
        NET=1 VLAN=0 POSTINIT="" start_vm r1
        NET=2 VLAN=0 POSTINIT="" start_vm r2

	# Create a serial line between the 2 VMs
	sleep 2
	socat "$TMP/vm-r1-serial.pipe" "$TMP/vm-r2-serial.pipe" &

        display_help
        cleanup

	killall socat

        ;;
esac

# Local Variables:
# mode: sh
# indent-tabs-mode: nil
# sh-basic-offset: 4
# End:
