#! /bin/sh

pushd ../..
chmod a-x *.*

pushd arch/cur/
chmod a-x *.*
popd
pushd arch/cbuilder/
chmod a-x *.*
popd
pushd arch/dos32/
chmod a-x *.*
popd
pushd arch/linux/
chmod a-x *.*
popd
pushd arch/qt/
chmod a-x *.*
popd
pushd arch/visualcpp/
chmod a-x *.*
popd

popd
