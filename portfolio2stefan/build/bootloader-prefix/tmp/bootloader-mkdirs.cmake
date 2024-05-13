# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.2.1/components/bootloader/subproject"
  "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader"
  "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader-prefix"
  "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader-prefix/tmp"
  "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader-prefix/src"
  "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Dev/IoTSem2Portfolio2/portfolio2stefan/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
