echo "#define GIT_COMMIT \"$(git rev-parse HEAD)\"" > common/git_ver.h
echo "#define GIT_VER \"$(git branch --show-current)\"" >> common/git_ver.h
echo "#define GIT_NUM $(git rev-list HEAD --count)" >> common/git_ver.h
echo "#define BUILD_DATE \"$(date '+%b %d %Y @ %T')\"" >> common/git_ver.h

TYPE="_final"
FINAL="FINAL=-DFINAL TYPE=$TYPE $O_FLAG"

if [ $2 -gt "0" ]
then
    echo "[+] Building without compiler optimization"
    O_FLAG="O_FLAG="
fi

if [ $1 -gt "0" ]
then
    echo "[+] Building with Debug flag"
    TYPE="_debug"
    FINAL="FINAL= TYPE=$TYPE $O_FLAG"
fi

BUILD_PRX="bin/plugins/prx$TYPE"
BUILD_ELF="bin/plugins/elf$TYPE"

rm -rf $BUILD_PRX $BUILD_ELF
mkdir -p $BUILD_PRX $BUILD_ELF

cd plugin-src
for dir in ./*
  do
    echo "[+] build dir: $dir"
    cd "$dir"
    make clean $FINAL || exit 1
    make $FINAL || exit 1
    cd ..
  done

cd ..

BUILD=$BUILD_PRX
BUILD_MD5="md5.txt"
BUILD_SHA256="sha256.txt"

echo "MD5:" > $BUILD/$BUILD_MD5
echo "SHA256:" > $BUILD/$BUILD_SHA256

FILES="$BUILD/*.prx"
for f in $FILES
  do
    echo "$(sha256sum $f)" >> $BUILD/$BUILD_MD5
    echo "$(md5sum $f)" >> $BUILD/$BUILD_SHA256
  done
