TYPE="_final"
O_FLAG="O_FLAG=-O3"
DEBUG_FLAG="DEBUG=0"
LOG_TYPE=
LOG_MSG="GoldHEN:SDK:KernelLog"

if [[ $2 == "-no_opt" ]]
then
    echo "[+] Building without compiler optimization"
    O_FLAG="O_FLAG="
fi

if [[ $1 == "-debug" ]]
then
    echo "[+] Building with Debug flag"
    TYPE="_debug"
	DEBUG_FLAG="DEBUG=1"
fi

if [[ $1 == "-printf" ]] || [[ $3 == "-printf" ]]
then
    echo "[+] Building with userland logging"
	LOG_TYPE="PRINTF=1"
	LOG_MSG="libc:printf"
fi

BUILD_PRX="bin/plugins/prx$TYPE"
BUILD_ELF="bin/plugins/elf$TYPE"

rm -rf $BUILD_PRX $BUILD_ELF
mkdir -p $BUILD_PRX $BUILD_ELF

current_dir=$(pwd)

echo "[+] Rebuilding SDK with Log Type: $LOG_MSG"

cd $GOLDHEN_SDK
make $LOG_TYPE DEBUGFLAGS=1

cd $current_dir

cd plugin_src
MAKEFILE="Makefile"

for dir in ./*
do
    if [[ -f "$dir/$MAKEFILE" ]]; then
        echo "[+] build dir: $dir"
        cd $dir
        make clean || exit 1
        make $DEBUG_FLAG $LOG_TYPE $O_FLAG || exit 1
        cd ..
    fi
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
