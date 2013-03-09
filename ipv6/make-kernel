#!/bin/bash

# Download, configure and build a kernel
# $1 - optional, kernel version ("3.7.2", "3.8",...)
#      if not specified, uses a git tree  (net-next by default)

# This is were the modules and kernel will be installed
# Must be the same as $KERNEL_DIR in v6eval-env
INSTALL_DIR="/tmp/install"

# Here is where the kernel will be downloaded and built
WORK_DIR="/tmp"

LOG_DIR="/tmp/logs"

GIT_NET_NEXT="git://git.kernel.org/pub/scm/linux/kernel/git/davem/net-next.git"
GIT_LINUS="git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git"

KERNEL_MIRROR_BASE="http://mirrors.xservers.ro/linux/kernel/v3.x/"

KERNEL_BASE_CONFIG="$PWD/kernel-config"

JOBS=6

log() {
    date=$(date --rfc-3339='seconds')
    echo "$date $1" >> $LOGFILE
}

get_kernel_git() {
    cd "$WORK_DIR"

    case $1 in
        net-next)
	    url="$GIT_NET_NEXT"
	    dir="net-next"
	    ;;
	linus)
	    url="$GIT_LINUS"
	    dir="linux"
	    ;;
    esac	

    if [ -d "$dir" ]; then
	cd "$dir"
	git pull
    else
        git clone "$url"
    fi
}

get_kernel_archive() {
    version="$1"

    cd "$WORK_DIR"
    
    log "Downloading kernel $version"
    if ! wget "$KERNEL_MIRROR_BASE/linux-$version.tar.bz2" &>> $LOGFILE; then
	    log "error: could not download the archive"
	    exit 1
    fi

    log "Extracting..."
    tar xfj "linux-$version.tar.bz2" &>> $LOGFILE
}

configure_kernel() {
    path="$1"

    cd "$path"

    log "Copy config"
    cp "$KERNEL_BASE_CONFIG" .config

    log "Configure"
    make olddefconfig &>> $LOGFILE
}

build_kernel() {
    path="$1"

    cd "$path"

    log "Start building"
    if ! make -j$JOBS &>> $LOGFILE; then
	log "error: build problem"
	exit 1
    fi

    make INSTALL_MOD_PATH="$INSTALL_DIR" modules_install || (
	log "error: modules installation error"
	exit 1
    )

    cp arch/x86/boot/bzImage "${INSTALL_DIR}/linux"
}

init() {
    export LOGFILE="$LOG_DIR/$(date +%d.%m.%Y-%H:%M.log)"
    test -d "$LOG_DIR" || mkdir -p "$LOG_DIR"
    test -d "$INSTALL_DIR" || mkdir -p "$INSTALL_DIR"
    test -d "$WORK_DIR" || mkdir -p "$WORK_DIR"
}

run() {
    ver="$1"
    if [ -n "$ver" ]; then
        get_kernel_archive "$ver"
	configure_kernel "${WORK_DIR}/linux-$ver"
	build_kernel "${WORK_DIR}/linux-$ver"
    else
	get_kernel_git "net-next"
	configure_kernel "${WORK_DIR}/net-next"
	build_kernel "${WORK_DIR}/net-next"
    fi
}

KVERSION="$1"

init
(time run $KVERSION) &>> $LOGFILE

