#
#    Copyright (c) 2020 Project nler Authors
#    Copyright (c) 2018 Nest Labs, Inc.
#    All rights reserved.
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
#      This file is the GNU automake template for the Nest Labs
#      Embedded Runtime POSIX threads-specific library.
#

include $(abs_top_nlbuild_autotools_dir)/automake/pre.am

lib_LIBRARIES                   = \
    libnlerpthreads.a             \
    $(NULL)

libnlerpthreads_a_CPPFLAGS      = \
    -I$(top_srcdir)/include       \
    $(NULL)

libnlerpthreads_a_SOURCES       = \
    nlerinit-pthreads.c           \
    nlertime-pthreads.c           \
    nleventpooled-pthreads.c      \
    nleventqueue-pthreads.c       \
    nllock-pthreads.c             \
    nlsemaphore-pthreads.c        \
    nltask-pthreads.c             \
    $(NULL)

include_HEADERS                 = \
    nlernative.h                  \
    nlertaskpriority.h            \
    nlertaskstack.h               \
    $(NULL)

include $(abs_top_nlbuild_autotools_dir)/automake/post.am

