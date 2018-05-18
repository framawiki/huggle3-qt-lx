#!/bin/sh


extensions='mass-delivery'
orig=`pwd`

for xx in $extensions
do
    if [ -d "extension_list/$xx" ];then
       if [ ! -d "extension_list/$xx/src" ];then
           cd "extension_list/$xx/" || exit 1
           git submodule init
           git submodule update
           cd - || exit 1
       fi
       # we need to check if there is a symlink to huggle folder
       if [ ! -e "extension_list/$xx"/src/huggle ];then
           echo "There is no symlink in extension_list/$xx/src/huggle I am creating one"
           ln -s `pwd` "extension_list/$xx"/src/huggle || exit 1 
       fi
       if [ ! -d "$xx"_build ]; then
           echo "Creating build directory"
           mkdir "$xx"_build || exit 1
       fi
       cd "$orig/$xx"_build || exit 1
       cmake "$orig/extension_list/$xx" || exit 1
       make
       cd - || exit 1
    fi
done
