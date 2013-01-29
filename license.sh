#!/bin/bash

for i in `find . -type f`; do mkdir -p tmp/`dirname $i`; sed -e 's/\$FU-Copyright\$/FUB-LICENSE/g' $i > tmp/$i; done
cd tmp
cp -Rf * ..
cd ..
rm -rf tmp
