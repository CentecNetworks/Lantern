#!/bin/sh

########################################################################
# Copyright (C) 2004-2013 Centec Networks. All rights reserved.
########################################################################

UDISK_DEBIAN_ROOTFS=/mnt/udisk
FLASH_DEBIAN_ROOTFS=/mnt/flash
TIMEOUT=60

STARTUP_LOG=/mnt/flash/cold/log/startup_log

mount -t proc none /proc
mount -t sysfs none /sys

busybox --install -s
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s

mknod /dev/asic_allctrl c 99 0
mknod /dev/ctc_hw c 100 0
mknod /dev/ctc_asic_normal c 101 0
mknod /dev/ctc_asic_fatal c 103 0

mkdir /dev/pts
mount -t devpts none /dev/pts

ln -s /proc/self/fd/0 /dev/stdin

echo -n "Mounting JFFS2 from /dev/mtdblock1... "
if ! mount -t jffs2 /dev/mtdblock1 /mnt/flash > /dev/null 2>&1; then
    echo "failed!"

    echo "Formatting /dev/mtdblock1 to JFFS2..."
    flash_eraseall -j /dev/mtd1

    echo -n "Mounting JFFS2 from /dev/mtdblock1... "
    mount -t jffs2 /dev/mtdblock1 /mnt/flash
else
    echo "OK!"
fi

# support two mtd block on flash.
if [ -e /dev/mtdblock2 ]; then
  if [ ! -e /mnt/data ]; then
      mkdir /mnt/data
  fi
  echo -n "Mounting JFFS2 from /dev/mtdblock2... "
  if ! mount -t jffs2 /dev/mtdblock2 /mnt/data > /dev/null 2>&1; then
      echo "failed!"

      echo "Formatting /dev/mtdblock2 to JFFS2..."
      flash_eraseall -j /dev/mtd2

      echo -n "Mounting JFFS2 from /dev/mtdblock2... "
      mount -t jffs2 /dev/mtdblock2 /mnt/data
  else
      echo "OK!"
  fi
fi

echo "busybox started, ready to serve..."

if [ ! -d /mnt/flash/conf ]; then
	mkdir /mnt/flash/conf
fi

if [ -f /mnt/flash/.zone.conf ]; then
    source /mnt/flash/.zone.conf
fi

if [ ! -d /mnt/flash/boot ]; then
    mkdir /mnt/flash/boot
fi

hostname Switch

touch /var/run/utmp

if [ ! -d /mnt/flash/cold ]; then
    mkdir -p /mnt/flash/cold
fi

if [ ! -d /mnt/flash/cold/log ]; then
    mkdir -p /mnt/flash/cold/log
fi

if [ ! -d /mnt/flash/cold/bak ]; then
    mkdir -p /mnt/flash/cold/bak
fi

if [ ! -d /mnt/flash/cold/running ]; then
    mkdir -p /mnt/flash/cold/running
fi


if [ -f $STARTUP_LOG ] ; then
    touch $STARTUP_LOG
fi

insmod /lib/ctc_hw.ko
insmod /lib/ctc_asic_io.ko
insmod /lib/peth_km.ko

pizza_box=y
boardtype=`board_type_detect board_type`
selfip=`board_type_detect selfip`

#hwclock --hctosys

macaddr=`board_type_detect mac`

#ifconfig eth1 hw ether $macaddr
ifconfig eth1 up mtu 9500

# Configure loopback interface
ifconfig lo 127.0.0.1

# Configure VM
sysctl -w vm.overcommit_memory=2 2>&1 > /dev/null
sysctl -w vm.overcommit_ratio=80 2>&1 > /dev/null

# Configure coredump
sysctl -w kernel.core_pattern='|/usr/bin/core_helper %e' 2>&1 > /dev/null

# Configure socket
sysctl -w net.core.rmem_max=8000000 2>&1 > /dev/null
sysctl -w net.core.rmem_default=1000000 2>&1 > /dev/null
sysctl -w net.core.wmem_max=8000000 2>&1 > /dev/null
sysctl -w net.core.wmem_default=1000000 2>&1 > /dev/null

#Configure default IGMP version=2
sysctl -w net.ipv4.conf.default.force_igmp_version=2 2>&1 > /dev/null

#Configure socket max memberships
sysctl -w net.ipv4.igmp_max_memberships=1000 2>&1 > /dev/null

# Configure TCP/IP stack
sysctl -w net.ipv4.conf.all.accept_source_route=1 2>&1 > /dev/null
sysctl -w net.ipv6.conf.all.disable_ipv6=1 2>&1 > /dev/null
sysctl -w net.ipv6.conf.default.autoconf=0 2>&1 > /dev/null
sysctl -w net.ipv6.conf.all.autoconf=0 2>&1 > /dev/null
sysctl -w net.ipv6.conf.default.dad_transmits=0 2>&1 > /dev/null
sysctl -w net.ipv6.conf.all.dad_transmits=0 2>&1 > /dev/null
sysctl -w net.ipv6.conf.default.router_solicitations=0 2>&1 > /dev/null
sysctl -w net.ipv6.conf.all.router_solicitations=0 2>&1 > /dev/null
sysctl -w net.ipv6.conf.default.router_solicitation_delay=0 2>&1 > /dev/null
sysctl -w net.ipv6.conf.all.router_solicitation_delay=0 2>&1 > /dev/null
sysctl -w net.ipv4.route.max_size=65536 2>&1 > /dev/null
sysctl -w net.ipv4.route.gc_thresh=4096 2>&1 > /dev/null
sysctl -w net.ipv6.route.max_size=65536 2>&1 > /dev/null
sysctl -w net.ipv6.route.gc_thresh=4096 2>&1 > /dev/null

# For nsm/uarp
sysctl -w net.ipv4.neigh.default.ucast_solicit=0 2>&1 > /dev/null
sysctl -w net.ipv4.neigh.default.mcast_solicit=0 2>&1 > /dev/null
sysctl -w net.ipv4.neigh.default.app_solicit=1 2>&1 > /dev/null
sysctl -w net.ipv4.neigh.default.retrans_time_ms=3000 2>&1 > /dev/null

#for icmp redirect, for bug 13190
sysctl -w net.ipv4.conf.all.send_redirects=0 2>&1 > /dev/null
# For nsm/ndp
sysctl -w net.ipv6.neigh.default.ucast_solicit=0 2>&1 > /dev/null
sysctl -w net.ipv6.neigh.default.mcast_solicit=0 2>&1 > /dev/null
sysctl -w net.ipv6.neigh.default.app_solicit=1 2>&1 > /dev/null
sysctl -w net.ipv6.neigh.default.retrans_time_ms=3000 2>&1 > /dev/null
sysctl -w net.ipv6.neigh.default.gc_thresh1=256 2>&1 > /dev/null
sysctl -w net.ipv6.neigh.default.gc_thresh2=1024 2>&1 > /dev/null
sysctl -w net.ipv6.neigh.default.gc_thresh3=2048 2>&1 > /dev/null

# enlarge kernel neigh number
echo 1024 > /proc/sys/net/ipv4/neigh/default/gc_thresh1
echo 4096 > /proc/sys/net/ipv4/neigh/default/gc_thresh2
echo 8192 > /proc/sys/net/ipv4/neigh/default/gc_thresh3

/sbin/ifconfig eth0 $selfip netmask 255.255.255.0 up

#sysctl -w net.ipv4.neigh.eth0.app_solicit=0 2>&1 >/dev/null
#sysctl -w net.ipv4.neigh.eth0.ucast_solicit=3 2>&1 >/dev/null
#sysctl -w net.ipv4.neigh.eth0.mcast_solicit=3 2>&1 >/dev/null

# Enable TCP Syncookies
echo 1 > /proc/sys/net/ipv4/tcp_syncookies

#echo "Starting CROND..."
#crond -c /etc/crontabs -L /dev/null &

#Open source system startup
time_count=1
start_mini_linux=1

prepare_debian () {
    DEBIAN_ROOTFS="$1"
    
    #mount flash on debian
    if [ ! -d $DEBIAN_ROOTFS/mnt/flash ]; then
        mkdir -p $DEBIAN_ROOTFS/mnt/flash
    fi
    mount -t jffs2 /dev/mtdblock1 $DEBIAN_ROOTFS/mnt/flash  > /dev/null 2>&1
    
    cp -rf /etc/debian_cfg/inittab $DEBIAN_ROOTFS/etc
}

if [ -f /mnt/flash/debian_from_flash ] ; then
    echo -n "Starting Debian linux system from flash ..."
    while true; do
        line=`/bin/mount | grep /mnt/flash`
        if [ "$line" != "" ] ; then
            start_mini_linux=0
            break;
        fi
        if [ $time_count -gt $TIMEOUT ] ; then
            start_mini_linux=1
            echo ""
            echo "Unable to mount /mnt/flash"
            break;
        fi
        echo -n "."
        time_count=`expr $time_count + 1` 
        sleep 1;
    done
    
    if [ $start_mini_linux == 0 ]; then
        if [ -d $FLASH_DEBIAN_ROOTFS/usr ]; then
            start_mini_linux=0
            #/usr/sbin/start_debian.sh flash
            prepare_debian $FLASH_DEBIAN_ROOTFS 
            exec /sbin/switch_root $FLASH_DEBIAN_ROOTFS /sbin/init 5
        else
            echo ""
            echo "Debian linux system on flash is not existed"
            start_mini_linux=1
        fi
    fi
elif [ -f /mnt/flash/debian_from_udisk ] ; then
    echo -n "Starting Debian linux system from usb storage ..."
    while true; do
        line=`/bin/mount | grep /mnt/udisk`
        if [ "$line" != "" ] ; then
            start_mini_linux=0
            break;
        fi
        if [ $time_count -gt $TIMEOUT ] ; then
            start_mini_linux=1
            echo ""
            echo "Unable to mount /mnt/udisk"
            break;
        fi
        echo -n "."
        time_count=`expr $time_count + 1`
        sleep 1;
    done

    if [ $start_mini_linux == 0 ]; then
        echo ""
        if [ -d $UDISK_DEBIAN_ROOTFS/usr ]; then
            start_mini_linux=0
            prepare_debian $UDISK_DEBIAN_ROOTFS
            exec /sbin/switch_root $UDISK_DEBIAN_ROOTFS /sbin/init 5
        else
            echo ""
            echo "Debian linux system on usb storage is not existed"
            start_mini_linux=1
        fi
    fi
else
    #dummy
    echo "dummy" > /dev/null
fi

if [ $start_mini_linux == 1 ]; then
    #start busybox init
    echo ""
    echo "Starting Mini linux system ..."
    exec /bin/init -z
fi
