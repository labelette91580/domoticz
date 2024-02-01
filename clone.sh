#! /bin/sh

DIR=domoticz91580
BRANCH=devmerge3

rm -rf domoticz91580

git clone https://github.com/labelette91580/domoticz.git $DIR
cd $DIR
git checkout $BRANCH
 cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
 make
nohup make &
