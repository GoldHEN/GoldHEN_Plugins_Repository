echo "#define GIT_COMMIT \"$(git rev-parse HEAD)\"" > common/git_ver.h
echo "#define GIT_VER \"$(git branch --show-current)\"" >> common/git_ver.h
echo "#define GIT_NUM $(git rev-list HEAD --count)" >> common/git_ver.h
echo "#define BUILD_DATE \"$(date '+%b %d %Y @ %T (%Z %z)')\"" >> common/git_ver.h

FINAL="FINAL=-DFINAL=1 TYPE=_final"
mkdir -p bin/plugins/prx_final bin/plugins/elf_final

if [ $1 -gt "0" ]
then
    mkdir -p bin/plugins/prx_debug bin/plugins/elf_debug
    FINAL="FINAL= TYPE=_debug"
fi

cd plugin-src
for dir in ./*
  do
    echo "$dir"
    cd "$dir"
    make clean $FINAL || exit 1
    make $FINAL || exit 1
    cd ..
  done

cd ..

BUILD=bin/plugins/prx_final

if [ $1 -gt "0" ]
then
    BUILD=bin/plugins/prx_debug
fi

touch $BUILD/md5.txt $BUILD/sha256.txt
echo "MD5:" > $BUILD/md5.txt
echo "SHA256:" > $BUILD/sha256.txt

FILES="$BUILD/*.prx"
for f in $FILES
  do
    echo "$(sha256sum $f)" >> $BUILD/sha256.txt
    echo "$(md5sum $f)" >> $BUILD/md5.txt
  done
