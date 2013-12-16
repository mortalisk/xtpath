#!/bin/sh

if [ ! -d 'external' ];
then
  mkdir external
fi

if [ ! -f 'boost_1_55_0.tar.bz2' ];
then
  wget "http://downloads.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.bz2?r=http%3A%2F%2Fwww.boost.org%2Fusers%2Fhistory%2Fversion_1_55_0.html&ts=1387203048&use_mirror=kent" -O boost_1_55_0.tar.bz2
  
fi

if [ ! -d 'external/boost_1_55_0' ];
then
  tar xfvj 'boost_1_55_0.tar.bz2' -C 'external'
fi

(cd 'external/boost_1_55_0' && exec ./bootstrap.sh --prefix=../boost --with-libraries=test)
(cd 'external/boost_1_55_0' && exec ./b2 install)
