TYPE="_final"
FINAL="FINAL=-D__FINAL__=1 TYPE=$TYPE $O_FLAG"

if [[ $2 == "-no_opt" ]]
then
    echo "[+] Building without compiler optimization"
    O_FLAG="O_FLAG="
fi

if [[ $1 == "-debug" ]]
then
    echo "[+] Building with Debug flag"
    TYPE="_debug"
    FINAL="FINAL=-D__FINAL__=0 TYPE=$TYPE $O_FLAG"
fi

BUILD_PRX="bin/plugins/prx$TYPE"
BUILD_ELF="bin/plugins/elf$TYPE"

rm -rf $BUILD_PRX $BUILD_ELF
mkdir -p $BUILD_PRX $BUILD_ELF

cd plugin_src
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
