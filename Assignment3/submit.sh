#! /bin/bash
NAME="2015CS10262_2015CS10210"
DIR="ignore"
rm -rf $DIR/$NAME
mkdir -p $DIR/$NAME
cp -r CMakeLists.txt cmake include include_lib lib models resources src submit.sh $DIR/$NAME/
cd $DIR
rm -rf $NAME.zip
zip -r $NAME $NAME

