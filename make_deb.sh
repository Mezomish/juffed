#!/bin/sh

BUILD_DIR=build
VERSION=`cat version`
RELEASE="1"
TEMP_DIR="temp_fake_root_dir"
ARCH="i386"
DISTR=""

print_usage() {
	echo ""
	echo "Usage: make_deb.sh [options]"
	echo ""
	echo "Valid options:"
	echo "    --distr=<distr>            : Distribution suffix"
	echo "    --arch=<arch>              : Processor architecture (default: i386)"
	echo "    --build-dir=<dir>          : Build directory (default: 'build')"
	echo "    --release=<release>        : Package release (default: 1)"
	echo "    --do-not-clean             : Do not remove temporary directory"
	echo "    --help                     : Print this help"
	echo ""
}

exit_with_error() {
	echo "Error occured, exiting...."
	exit 1
}

#	Parse command line arguments
for arg in ${@}; do
	case ${arg} in
	
	--help)
		print_usage
		exit 0
		;;

	--distr=*)
		DISTR=`echo ${arg} | cut -c 9-`
		;;
	
	--build-dir=*)
		BUILD_DIR=`echo ${arg} | cut -c 13-`
		;;

	--release=*)
		RELEASE=`echo ${arg} | cut -c 11-`
		;;
		
	--arch=*)
		ARCH=`echo ${arg} | cut -c 8-`
		;;
		
	--do-not-clean)
		DO_NOT_CLEAN="yes"
		;;
		
	*)
		echo "Unrecognized option: ${arg}"
		print_usage
		exit 1
		;;

	esac
done

if [ -z "$DISTR" ]; then
	echo ""
	echo "Error: Distribution is not set";
	print_usage
	exit 1
fi

VERSION=`cat ${BUILD_DIR}/version.real`

rm -rf ${TEMP_DIR}
mkdir -p ${TEMP_DIR}/usr

cd build
make DESTDIR="../${TEMP_DIR}" install || exit_with_error
cd ..
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
