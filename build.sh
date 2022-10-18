rm -rf bin/plugins/*.prx bin/plugins/*.elf

echo "#define GIT_COMMIT \"$(git rev-parse HEAD)\"" > common/git_ver.h
echo "#define GIT_VER \"$(git branch --show-current)_1.$(git rev-list HEAD --count)\"" >> common/git_ver.h
echo "#define GIT_NUM $(git rev-list HEAD --count)" >> common/git_ver.h
echo "#define BUILD_DATE \"$(date '+%b %d %Y @ %T (%Z %z)')\"" >> common/git_ver.h

FINAL="FINAL=-DFINAL"

if [ $1 -gt "0" ]
then
    FINAL="FINAL="
fi

cd plugin-src
for dir in ./*; do (echo "$dir" && cd "$dir" && make clean && make $FINAL); done
