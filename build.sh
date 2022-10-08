rm -rf dll/*.prx
FINAL="FINAL=-DFINAL"

if [ $1 -gt "0" ]
then
    FINAL="FINAL="
fi

cd no-share-watermark
make clean && make $FINAL
cd ../universal-fliprate-remover
make clean && make $FINAL
cd ../game-patch
make clean && make $FINAL
cd ..
# curl -T dll/game-patch.prx ftp://192.168.1.192:2121/data/GoldHEN/test.prx
