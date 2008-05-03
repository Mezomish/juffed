#!/bin/sh

VERSION=`cat version`
RELEASE="1"
TEMP_DIR="temp_fake_root_dir"
DO_NOT_CLEAN=
ARCH="i386"

print_usage() {
	echo ""
	echo "    Usage: make_deb.sh [options]"
	echo ""
	echo "    Valid options:"
	echo "        --release=<release>        : Package release (default: 1)"
	echo "        --distr=<distr>            : Distribution suffix"
	echo "        --arch=<arch>              : Processor architecture (default: i386)"
	echo "        --do-not-clean             : Do not remove temporary directory"
	echo "        --help                     : Print this help"
	echo ""
}

exit_with_error() {
	echo "Error occured, exiting...."
	exit 1
}

#	Parse command line arguments
for arg in $@
do
	if [ ${arg:0:10} == "--release=" ]
	then
		RELEASE=${arg:10}
	fi
	
	if [ ${arg:0:8} == "--distr=" ]
	then
		DISTR=${arg:8}
	fi

	if [ ${arg:0:7} == "--arch=" ]
	then
		ARCH=${arg:7}
	fi
	
	if [ $arg == "--help" ]
	then
		print_usage
		exit 0
	fi
	
	if [ $arg == "--do-not-clean" ]
	then
		DO_NOT_CLEAN="yes"
	fi
done

rm -rf ${TEMP_DIR}
mkdir -p ${TEMP_DIR}/usr

make FAKE_ROOT=${TEMP_DIR} install || exit_with_error
strip ${TEMP_DIR}/usr/bin/juffed

cp -R DEBIAN_template ${TEMP_DIR}/DEBIAN
SIZE=`du -s ${TEMP_DIR}/usr | cut -f 1`

sed -i "s/@VERSION/${VERSION}-${RELEASE}${DISTR}/" ${TEMP_DIR}/DEBIAN/control
sed -i "s/@SIZE/${SIZE}/" ${TEMP_DIR}/DEBIAN/control
sed -i "s/@ARCH/${ARCH}/" ${TEMP_DIR}/DEBIAN/control

PKG_NAME="juffed_${VERSION}-${RELEASE}${DISTR}""_${ARCH}.deb"

dpkg -b ${TEMP_DIR} ${PKG_NAME}

if [ -z "${DO_NOT_CLEAN}" ]
then
	rm -rf $TEMP_DIR
fi
