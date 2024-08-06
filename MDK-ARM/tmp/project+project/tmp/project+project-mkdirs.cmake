# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project"
  "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/1"
  "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project"
  "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project/tmp"
  "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project/src/project+project-stamp"
  "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project/src"
  "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project/src/project+project-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project/src/project+project-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/19308/Desktop/Keil 5-project/project/MDK-ARM/tmp/project+project/src/project+project-stamp${cfgdir}") # cfgdir has leading slash
endif()
