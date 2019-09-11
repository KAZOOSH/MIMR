#!/bin/bash

DESTDIR="/opt/hotprint/"

SYSTEMCTL="sudo systemctl"
CP="sudo cp"
RM="sudo rm"
MKDIR="sudo mkdir"
CHOWN="sudo chown"
CHMOD="sudo chmod"

SDIR="."
DDIR="/lib/systemd/system"

function uninstallUnit {
	$SYSTEMCTL stop $1
	$SYSTEMCTL disable $1
	$RM "$DDIR/$1"
}

function installUnit {
	$CP "$SDIR/$1" "$DDIR/$1"
	$SYSTEMCTL enable $1
	$SYSTEMCTL start $1
}


function install {
	echo "setting up ravelox autostart"
	#installUnit "ravelox.service"
}

function uninstall {
	echo "remove ravelox autostart"
	uninstallUnit "ravelox.service"
}



case "$1" in
	install)
		install
	;;

	uninstall)
		uninstall
	;;

	*)
		echo $"Usage: $0 {install|uninstall}"
		exit 1

esac

