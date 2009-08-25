#!/bin/sh

REVISION=`LC_ALL=C svn info | grep Revision | cut -d" " -f2-`
VERSION=`cat version`".$REVISION"
DIR="juffed-${VERSION}"

TARGZ=""
TARBZ2=""
P7Z=""

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

#	Parse command line arguments
for arg in ${@}; do
	case ${arg} in
	
	--help)
		print_usage
		exit 0
		;;

	--targz)
		TARGZ="yes"
		;;
	
	--tarbz2)
		TARBZ2="yes"
		;;

	--7z)
		P7Z="yes"
		;;
		
	--all)
		TARGZ="yes"
		TARBZ2="yes"
		P7Z="yes"
		;;
		
	*)
		echo "Unrecognized option: ${arg}"
		print_usage
		exit 1
		;;

	esac
done

mkdir $DIR

svn co http://juffed.svn.sourceforge.net/svnroot/juffed/trunk $DIR
cat CMakeLists.txt | sed -r "s/DEV 1/DEV 0/" > $DIR/CMakeLists.txt
echo $VERSION > $DIR/version

rm -rf $DIR/win32/
find $DIR -name ".svn" -exec rm -rf '{}' ';'

mv $DIR/debian.in $DIR/debian
sed -i "s/@FULL_VERSION@/$VERSION/" $DIR/debian/control
sed -i "s/@FULL_VERSION@/$VERSION/" $DIR/debian/changelog

if [ -n "${TARGZ}" ]; then
	tar -czf "juffed-${VERSION}.tar.gz" $DIR
fi

if [ -n "${TARBZ2}" ]; then
	tar -cjf "juffed-${VERSION}.tar.bz2" $DIR
fi

if [ -n "${P7Z}" ]; then
	7z a "juffed-${VERSION}.7z" $DIR
fi
