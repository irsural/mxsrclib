#! /bin/sh

pushd ../..
rm -f *.obj *.tds *.~* *.il? *.bak *.*~ *.dcu ~W*.tmp ~$*.*

pushd arch/cur/
rm -f *.obj *.tds *.~* *.il? *.bak *.*~ *.dcu ~W*.tmp ~$*.*
popd
pushd arch/cbuilder/
rm -f *.obj *.tds *.~* *.il? *.bak *.*~ *.dcu ~W*.tmp ~$*.*
popd
pushd arch/dos32/
rm -f *.obj *.tds *.~* *.il? *.bak *.*~ *.dcu ~W*.tmp ~$*.*
popd
pushd arch/linux/
rm -f *.obj *.tds *.~* *.il? *.bak *.*~ *.dcu ~W*.tmp ~$*.*
popd
pushd arch/qt/
rm -f *.obj *.tds *.~* *.il? *.bak *.*~ *.dcu ~W*.tmp ~$*.*
popd
pushd arch/visualcpp/
rm -f *.obj *.tds *.~* *.il? *.bak *.*~ *.dcu ~W*.tmp ~$*.*
popd

popd