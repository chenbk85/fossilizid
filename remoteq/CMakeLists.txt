PROJECT(remoteq)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8) 

AUX_SOURCE_DIRECTORY(. DIR_SRC)
file(GLOB header_files ./ ./*.h)

SET(SRC_LIST 
    ${DIR_SRC}
)

if(WIN32)
AUX_SOURCE_DIRECTORY(./windows/ windows_DIR_SRC)
file(GLOB windows_head_file ./windows/ ./windows/*.h)

AUX_SOURCE_DIRECTORY(./reliable/windows/ reliable_windows_DIR_SRC)
file(GLOB reliable_windows_head_file ./reliable/windows/ ./reliable/windows/*.h)


AUX_SOURCE_DIRECTORY(./fast/windows/ fast_windows_DIR_SRC)
file(GLOB fast_windows_head_file ./fast/windows/ ./fast/windows/*.h)

set(header_files
    ${header_files}
    ${windows_head_file}
    ${reliable_windows_head_file}
    ${fast_windows_head_file}
)

SET(SRC_LIST
    ${windows_DIR_SRC}
    ${reliable_windows_DIR_SRC}
    ${fast_windows_DIR_SRC}
)
endif(WIN32)

ADD_LIBRARY(remoteq ${SRC_LIST} ${header_files})