#!/bin/sh

PACKAGE_NAME="juffed-plugins"

print_usage() {
	echo ""
	echo "Usage: make_tarball.sh [--tarbz2|--7z|--all] [--version=VERSION]"
	echo ""
	echo "Valid options:"
	echo "    --tarbz2                   : Make also a .tar.bz2 archive"
	echo "    --7z                       : Make also a .7z archive"
	echo "    --all                      : Make all archives: .tar.gz, .tar.bz2, .7z"
	echo "    --version=VERSION          : Set package version to VERSION"
	echo "    --help                     : Print this help"
	echo ""
}

TARBZ2=""
P7Z=""

svn up
REVISION=`LC_ALL=C svn info | grep Revision | cut -d" " -f2-`
VERSION=`cat version`".$REVISION"

#	Parse command line arguments
for arg in ${@}; do
	case ${arg} in
	
	--help)
		print_usage
		exit 0
		;;

	--tarbz2)
		TARBZ2="yes"
		;;

	--7z)
		P7Z="yes"
		;;
		
	--all)
		TARBZ2="yes"
		P7Z="yes"
		;;

	--version=*)
		VERSION=`echo "${arg}" | cut -d"=" -f2`
		;;
		
	*)
		echo "Unrecognized option: ${arg}"
		print_usage
		exit 1
		;;

	esac
done

DIR="${PACKAGE_NAME}-${VERSION}"
rm -rf $DIR 2>/dev/null
mkdir $DIR

# checkout SVN
svn co https://juffed-plugins.googlecode.com/svn/trunk/ $DIR

# remove unnecessary stuff
find $DIR -name ".svn" -exec rm -rf '{}' 2>/dev/null ';'
rm $DIR/make_tarball.sh

# prepare 'debian' directory
mv $DIR/debian.in $DIR/debian
sed -i "s/@VERSION@/$VERSION/" $DIR/debian/control
sed -i "s/@VERSION@/$VERSION/" $DIR/debian/changelog

# make a tarball and a .dsc file
dpkg-source -b $DIR

# pack other tarballs if necessary
if [ -n "${TARBZ2}" ]; then
	tar -cjf "${PACKAGE_NAME}_${VERSION}.tar.bz2" $DIR
fi

if [ -n "${P7Z}" ]; then
	7z a "${PACKAGE_NAME}_${VERSION}.7z" $DIR
fi
