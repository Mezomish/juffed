#!/bin/sh

print_usage() {
	echo ""
	echo "Usage: make_tarball.sh [--tarbz2|--7z|--all]"
	echo ""
	echo "Valid options:"
	echo "    --tarbz2                   : Make also a .tar.bz2 archive"
	echo "    --7z                       : Make also a .7z archive"
	echo "    --all                      : Make all archives: .tar.gz, .tar.bz2, .7z"
	echo "    --help                     : Print this help"
	echo ""
}

TARBZ2=""
P7Z=""

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
		
	*)
		echo "Unrecognized option: ${arg}"
		print_usage
		exit 1
		;;

	esac
done

DIR="juffed-${VERSION}"

svn up
DEV=`grep DEV CMakeLists.txt | grep SET | grep 1`
if [ -n "$DEV" ]; then 
	REVISION=`LC_ALL=C svn info | grep Revision | cut -d" " -f2-`
	VERSION=`cat version`".$REVISION"
else
	VERSION=`cat version`
fi

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

# Here I stopped creating a 'debian' directory (requested by Ubuntu maintainers).
# It'll be kept as 'debian.in' for those who wants to pack a deb-package 
# themselves (just rename it to 'debian')
#mv $DIR/debian.in $DIR/debian
sed -i "s/@FULL_VERSION@/$VERSION/" $DIR/debian.in/control
sed -i "s/@FULL_VERSION@/$VERSION/" $DIR/debian.in/changelog

# make a tarball and a .dsc file
#dpkg-source -b $DIR

# make a tarball
tar -czf "juffed_${VERSION}.tar.gz" $DIR

# pack other tarballs if necessary
if [ -n "${TARBZ2}" ]; then
	tar -cjf "juffed_${VERSION}.tar.bz2" $DIR
fi

if [ -n "${P7Z}" ]; then
	7z a "juffed_${VERSION}.7z" $DIR
fi
