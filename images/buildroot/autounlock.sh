#!/bin/sh

#DEBUG=echo
#PREFIX=$PWD/PBA32/overlay

ATTEMPTS=3
SED_DIR=$PREFIX/etc/sed-crypt

FAILED=0
SUCCEEDED=0
# This will find out what devices exist that have OPEL active.

MBRCNT=0

TPM_NV_PER_AUTHREAD=0x00040000
TPM_NV_PER_OWNERREAD=0x00020000

setarray() {
	eval $(eval echo $1_$2)='$3'
}

getarray() {
	if [ -n "$(eval echo \$$1_$2)" ]; then
		echo $(eval echo \$$1_$2)
	fi
}

try_unlock() {
	ITR=$1
	local DEV=$(getarray SED_DEV $ITR)
	local NAME=$(getarray SED_NAME $ITR)
	cryptsetup open ${SED_DIR}/${NAME}.img unlock-${NAME} --key-file $2
	if [ $? -eq 0 ]; then
		mkdir -p /media/unlock-${NAME}
		mount -t squashfs /dev/mapper/unlock-${NAME} /media/unlock-${NAME}
		$DEBUG sedutil-cli --setLockingRange 0 RW "$(cat /media/unlock-${NAME}/keyfile)" ${DEV}
		if [ $? -eq 0 ]; then
			echo "$NAME successfully unlocked."

			DEV_MBR_ENABLED=$(sedutil-cli --query $DEV | grep "[[:space:]]MBREnabled = " | sed 's/.*[[:space:]]MBREnabled = \([YyNn]\).*/\1/g')
			DEV_MBR_DONE=$(sedutil-cli --query $DEV | grep "[[:space:]]MBRDone = " | sed 's/.*[[:space:]]MBRDone = \([YyNn]\).*/\1/g')
			if [ "$DEV_MBR_ENABLED" = "Y" -o "$DEV_MBR_ENABLED" = "y" ]; then
				if [ "$DEV_MBR_DONE" = "N" -o "$DEV_MBR_DONE" = "n" ]; then
					setarray MBR_DEV $MBRCNT ${DEV}
					setarray MBR_PASS $MBRCNT "$(cat /media/unlock-${NAME}/keyfile)"
					MBRCNT=$((MBRCNT + 1))
				fi
			fi

			SUCCEEDED=$((SUCCEEDED + 1))
			if [ $(getarray SED_FAILED $ITR) -gt 0 ]; then
				FAILED=$((FAILED - 1))
			fi

			unset $(eval echo SED_DEV_$ITR SED_NAME_$ITR SED_FAILED_$ITR)
			SED_COUNT=$((SED_COUNT - 1))
		else
			echo "$NAME failed to unlock."
			FAILED=$((FAILED + 1))
			setarray SED_FAILED $ITR 1
		fi
		umount /media/unlock-${NAME}
		rmdir /media/unlock-${NAME}
		cryptsetup close unlock-${NAME}
	fi
}

process_yubikey() {
	ID=`ykinfo -s | awk '{print $2}'`

	if [ "x$ID" != "x" -a -r ${SED_DIR}/${ID}.yk ]; then
		TMPDIR=`mktemp -d`
		$DEBUG mount -t tmpfs tmpfs $TMPDIR

		SLOT=`cat ${SED_DIR}/${ID}.yk`
		I=0
		while [ $I -lt $ATTEMPTS -a $SED_COUNT -gt 0 ]; do
			read -p "Yubikey Passphrase for $ID [$((I + 1))/$ATTEMPTS]: " -sr PASS
			echo
			TMPFILE=`mktemp -p $TMPDIR`
			ykchalresp -${SLOT} "${PASS}" > $TMPFILE

			J=0
			for J in $(seq 0 $((SED_DEV_CNT - 1)))
			do
				if [ -n "$(getarray SED_DEV $J)" ]; then
					try_unlock $J $TMPFILE
				fi
			done

			rm -f $TMPFILE
			I=$((I + 1))
		done
		
		$DEBUG umount $TMPDIR
		$DEBUG rmdir $TMPDIR
	fi
}

process_usbdev() {
	USB_DEV_FILE=`mktemp`
	lsblk -o KNAME,MAJ:MIN,FSTYPE,TYPE,LABEL -r -n | awk '{if ($3 ~ /ext/ || $3 ~ /fat/) { print; }}' >$USB_DEV_FILE
	exec 3<>$USB_DEV_FILE

	while read -u 3 -s DEV
	do
		SED_COUNT=$I
		if [ $SED_COUNT -le 0 ]; then
			break
		fi

		DEV_TYPE=$(echo $DEV | awk '{print $4}')
		if [ "$DEV_TYPE" != "part" -a "$DEV_TYPE" != "disk" ]; then
			continue
		fi

		DEV_NAME=$(echo $DEV | awk '{print $1}')
		DEV_PARENT=$DEV_NAME

		DEV_PPARENT=$(lsblk -o KNAME,PKNAME -r -n | grep "^$DEV_PARENT " | awk '{print $2}')
		while [ -n "$DEV_PPARENT" ]; do
			DEV_PARENT=$DEV_PPARENT
			DEV_PPARENT=$(lsblk -o KNAME,PKNAME -r -n | grep "^$DEV_PARENT " | awk '{print $2}')
		done

		PARENT=$(lsblk -o KNAME,TRAN,SERIAL,VENDOR,MODEL -r -n | grep "^$DEV_PARENT ")

		DEV_TRAN=$(echo $PARENT | awk '{print $2}')
		if [ "$DEV_TRAN" != "usb" ]; then
			continue
		fi

		DEV_MAJOR=$(echo $DEV | awk '{print $2}' | awk -F: '{print $1}')
		DEV_MINOR=$(echo $DEV | awk '{print $2}' | awk -F: '{print $2}')
		DEV_FSTYPE=$(echo $DEV | awk '{print $3}')
		DEV_LABEL=$(echo $DEV | awk '{print $5}' | sed -e 's/\\x20/ /g' -e 's/ *$//')
		DEV_SERIAL=$(echo $PARENT | awk '{print $3}' | sed -e 's/\\x20/ /g' -e 's/ *$//')
		DEV_VENDOR=$(echo $PARENT | awk '{print $4}' | sed -e 's/\\x20/ /g' -e 's/ *$//')
		DEV_MODEL=$(echo $PARENT | awk '{print $5}' | sed -e 's/\\x20/ /g' -e 's/ *$//')

		#echo "$DEV_NAME|$DEV_PARENT|$DEV_MAJOR|$DEV_MINOR|$DEV_FSTYPE|$DEV_TYPE|$DEV_LABEL|$DEV_TRAN|$DEV_SERIAL|$DEV_VENDOR|$DEV_MODEL"
		echo "Attempting auto-unlock using USB Device $DEV_VENDOR $DEV_MODEL $DEV_SERIAL"

		if [ ! -b /dev/$DEV_NAME ]; then
			$DEBUG mknod /dev/$DEV_NAME b $DEV_MAJOR $DEV_MINOR
		fi

		$DEBUG mkdir -p /media/$DEV_NAME
		$DEBUG mount -t $DEV_FSTYPE /dev/$DEV_NAME /media/$DEV_NAME

		I=0
		for I in $(seq 0 $((SED_DEV_CNT - 1)))
		do
			if [ -n "$(getarray SED_DEV $I)" ]; then
				if [ -r /media/$DEV_NAME/$(getarray SED_NAME $I).key ]; then
					try_unlock $I /media/$DEV_NAME/$(getarray SED_NAME $I).key
				fi
			fi
		done

		$DEBUG umount /media/$DEV_NAME
	done

	exec 3>&-
	rm -f $USB_DEV_FILE
}

process_tpm() {
	if [ -r /dev/tpm0 -a -r ${SED_DIR}/tpm-idx ]; then
		echo "Attempting auto-unlock using TPM stored values."

		TMPDIR=`mktemp -d`
		$DEBUG mount -t tmpfs tmpfs $TMPDIR

		I=0
		for I in $(seq 0 $(($SED_DEV_CNT - 1)))
		do
			if [ -n "$(getarray SED_DEV $I)" ]; then
				exec 3<>${SED_DIR}/tpm-idx

				while read -u 3 -s DEV
				do
					TPM_IDX=$(echo $DEV | awk '{if ($1 == "'"$(getarray SED_NAME $I)"'") { print $2; }}')
					if [ -n "$TPM_IDX" ]; then
						TMPFILE=$TMPDIR/$TMP_IDX.tpm

						# Ensure that we only try and unlock a TPM NVRam slot once.  We get 3 attempts
						# and the TPM state is not going to change in the mean time.
						if [ ! -f $TMPDIR/$TMP_IDX.try ]; then
							PERMS=$(getcapability -cap 0x11 -scap $(printf "%x" $TPM_IDX) | awk -F ': ' '{if ($1 ~ /^Result for capability/) { print $3; }}')
							if [ -n "$PERMS" ]; then
								MATCHES=$(getcapability -cap 0x11 -scap $(printf "%x" $TPM_IDX) | awk -F ': ' '{if ($1 ~ /^Matches current TPM state/) { print $2; }}')
								if [ "x$MATCHES" = "xNo" ]; then
									continue
								fi

								SIZE=$(getcapability -cap 0x11 -scap $(printf "%x" $TPM_IDX) | awk -F '= ' '{if ($1 ~ /^dataSize /) { print $2; }}')
								if [ ! -f $TMPFILE ]; then
									if [ $((PERMS & TPM_NV_PER_OWNERREAD)) -ne 0 ]; then
										J=0
										while [ $J -lt $ATTEMPTS -a ! -f $TMPFILE ]; do
											read -p "TPM Owner Passphrase for NVRam Slot $TPM_IDX [$((J + 1))/$ATTEMPTS]: " -sr PASS
											echo
											if [ -n "$PASS" ]; then
												$DEBUG nv_readvalue -ix $(printf "%x" $TPM_IDX) -pwdo "${PASS}" -sz $SIZE -of $TMPFILE >/dev/null
											fi
											J=$((J + 1))
										done
									elif [ $((PERMS & TPM_NV_PER_AUTHREAD)) -ne 0 ]; then
										J=0
										while [ $J -lt $ATTEMPTS -a ! -f $TMPFILE ]; do
											read -p "TPM Passphrase for NVRam Slot $TPM_IDX [$((J + 1))/$ATTEMPTS]: " -sr PASS
											echo
											if [ -n "$PASS" ]; then
												$DEBUG nv_readvalue -ix $(printf "%x" $TPM_IDX) -pwdd "${PASS}" -sz $SIZE -of $TMPFILE >/dev/null
											fi
											J=$((J + 1))
										done
									else
										$DEBUG nv_readvalue -ix $(printf "%x" $TPM_IDX) -sz $SIZE -of $TMPFILE >/dev/null
									fi
								fi
							fi
							touch $TMPDIR/$TMP_IDX.try
						fi

						if [ -f $TMPFILE ]; then
							try_unlock $I $TMPFILE
							if [ -z "$(getarray SED_DEV $I)" ]; then
								READ_SUCCESS="$READ_SUCCESS $TPM_IDX"
								break
							fi
						fi
					fi
				done

				exec 3>&-
			fi
		done

		$DEBUG umount $TMPDIR
		$DEBUG rmdir $TMPDIR

		if [ -n "$READ_SUCCESS" ]; then
			# This will lock the appropriate nvram areas so that nvram can't be read
			# again until the next startup.
			for x in $(echo $READ_SUCCESS | tr ' ' '\n' | sort -u | tr '\n' ' '); do
				$DEBUG nv_readvalue -ix $(printf "%x" $TPM_IDX) -sz 0 >/dev/null
			done
		fi
	fi
}

I=0
for DEV in $(lsblk -o NAME,TRAN -r -d -n | awk '{if ($2 == "sata") { print $1 }}')
do
	DEV="/dev/$DEV"
	DATA="`sedutil-cli --query $DEV 2>/dev/null`"
	if [ $? -ne 0 ]; then
		continue
	fi
	DEV_LOCKING=$(echo "$DATA" | grep "[[:space:]]LockingEnabled = " | sed 's/.*[[:space:]]LockingEnabled = \([YyNn]\).*/\1/g')
	DEV_LOCKED=$(echo "$DATA" | grep "[[:space:]]Locked = " | sed 's/.*[[:space:]]Locked = \([YyNn]\).*/\1/g')

	DEV_MODEL=$(hdparm -i $DEV 2>/dev/null | awk -F ', ' '{if ($1 ~ /Model=/) { print $1 }}' | cut -f2- -d=)
	DEV_SERIAL=$(hdparm -i $DEV 2>/dev/null | awk -F ', ' '{if ($1 ~ /Model=/) { print $3 }}' | cut -f2- -d=)

	FULLDEV=ata-$(echo $DEV_MODEL | sed 's/ /_/g')_$(echo $DEV_SERIAL | sed 's/ /_/g')

	# echo "$DEV|$DEV_LOCKING|$DEV_LOCKED|$DEV_MODEL|$DEV_SERIAL|$FULLDEV"

	if [ $DEV_LOCKING = "N" -o $DEV_LOCKING = "n" ]; then
		continue
	fi
	if [ $DEV_LOCKED = "N" -o $DEV_LOCKED = "n" ]; then
		continue
	fi

	setarray SED_DEV $I "$DEV"
	setarray SED_NAME $I "$FULLDEV"
	setarray SED_FAILED $I 0

	I=$((I + 1))
	SED_DEV_CNT=$I
done

SED_COUNT=$I
# USB device is ALWAYS passwordless, try it first
if [ $SED_COUNT -gt 0 ]; then
	process_usbdev
fi

# Next try TPM, which MIGHT be passwordless
if [ $SED_COUNT -gt 0 ]; then
	process_tpm
fi

# Yubikey challenge always requires a password.
if [ $SED_COUNT -gt 0 ]; then
	process_yubikey
fi

if [ $FAILED -gt 0 -o $SUCCEEDED -eq 0 ]; then
	echo "Automatic unlocking incomplete or failed."
	$DEBUG /sbin/linuxpba
else
	I=0
	while [ $I -lt $MBRCNT ]; do
		$DEBUG sedutil-cli --setMBRDone on "$(getarray MBR_PASS $I)" $(getarray MBR_DEV $I)
		I=$((I + 1))
	done
	echo "Disabled Shaodw MBR, rebooting."
	reboot
fi
