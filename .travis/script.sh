#!/bin/sh

#
#    Copyright 2018 Google LLC All Rights Reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

#
#    Description:
#      This file is the script for Travis CI hosted, distributed continuous 
#      integration 'script' step.
#

die()
{
    echo " *** ERROR: " ${*}
    exit 1
}


case "${BUILD_TARGET}" in

    linux-auto-gcc-distcheck)
        ./configure -C && make && make distcheck
        ;;

    linux-auto-clang|osx-auto-clang)
        ./configure -C --with-build-platform=auto && make check
        ;;

    linux-auto-gcc)
        ./configure -C --with-build-platform=auto --enable-coverage && make check
        ;;

    linux-nspr-clang|osx-nspr-clang)
        ./configure -C --with-build-platform=nspr && make check
        ;;

    linux-nspr-gcc)
        ./configure -C --with-build-platform=nspr --enable-coverage && make check
        ;;

    linux-pthreads-clang|osx-pthreads-clang)
        ./configure -C --with-build-platform=pthreads && make check
        ;;

    linux-pthreads-gcc)
        ./configure -C --with-build-platform=pthreads --enable-coverage && make check
        ;;

    *)
        die "Unknown build target \"${BUILD_TARGET}\"."
        ;;
        
esac
