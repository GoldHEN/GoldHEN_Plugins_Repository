rm -rf dll/*.prx

echo "#define GIT_COMMIT \"$(git rev-parse HEAD)\"" > common/git_ver.h
echo "#define GIT_VER \"$(git branch --show-current)_1.$(git rev-list HEAD --count)\"" >> common/git_ver.h
echo "#define GIT_NUM $(git rev-list HEAD --count)" >> common/git_ver.h
echo "#define BUILD_DATE \"$(date '+%b %d %Y @ %T (%Z %z)')\"" >> common/git_ver.h

if [ $1 -gt "0" ]
then
    FINAL="FINAL="
fi

FINAL="FINAL=-DFINAL"
cd plugin-src
for dir in ./*; do (cd "$dir" && make clean && make $FINAL); done
