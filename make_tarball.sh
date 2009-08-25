#!/bin/sh

REVISION=`LC_ALL=C svn info | grep Revision | cut -d" " -f2-`
VERSION=`cat version`".$REVISION"
DIR="juffed-${VERSION}"

TARGZ=""
TARBZ2=""
P7Z=""

print_usage() {
	echo ""
	echo "Usage: make_tarball.sh [--targz|--tarbz2|--7z|--all]"
	echo ""
	echo "Valid options:"
	echo "    --targz                    : Make a .tar.gz archive"
	echo "    --tarbz2                   : Make a .tar.bz2 archive"
	echo "    --7z                       : Make a .7z archive"
	echo "    --all                      : Make all tree archives"
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

rm -rf $DIR 2>/dev/null
mkdir $DIR

# checkout SVN
svn co http://juffed.svn.sourceforge.net/svnroot/juffed/trunk $DIR

# set the proper version
cat CMakeLists.txt | sed -r "s/DEV 1/DEV 0/" > $DIR/CMakeLists.txt
echo $VERSION > $DIR/version

# remove unnecessary stuff
rm -rf $DIR/win32/
find $DIR -name ".svn" -exec rm -rf '{}' 2>/dev/null ';'
rm $DIR/make_tarball.sh

# prepare 'debian' directory
mv $DIR/debian.in $DIR/debian
sed -i "s/@FULL_VERSION@/$VERSION/" $DIR/debian/control
sed -i "s/@FULL_VERSION@/$VERSION/" $DIR/debian/changelog

# pack tarballs if necessary
if [ -n "${TARGZ}" ]; then
	tar -czf "juffed-${VERSION}.tar.gz" $DIR
fi

if [ -n "${TARBZ2}" ]; then
	tar -cjf "juffed-${VERSION}.tar.bz2" $DIR
fi

if [ -n "${P7Z}" ]; then
	7z a "juffed-${VERSION}.7z" $DIR
fi
