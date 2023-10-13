# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("libflute")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/libflute
        SYSTEM
        ${PROJECT_SOURCE_DIR}/lib
        ${PROJECT_SOURCE_DIR}/lib/multipart-parser
        ${PROJECT_SOURCE_DIR}/lib/tinyxml2
        ${PROJECT_SOURCE_DIR}/lib/base64
        ${PROJECT_SOURCE_DIR}/lib/boost-1_79_0/include
        ${PROJECT_SOURCE_DIR}/lib/cpprestsdk/include
        ${PROJECT_SOURCE_DIR}/lib/openssl-1.1.1k-clang/include
)

add_library( # Sets the name of the library.
        libflute

        # Sets the library as a shared library.
        SHARED
)

target_sources(libflute
        PRIVATE

        android-jni-bindings.cpp

        libflute/Receiver.cpp
        libflute/AlcPacket.cpp
        libflute/File.cpp
        libflute/EncodingSymbol.cpp
        libflute/FileDeliveryTable.cpp

        lib/base64/base64.cpp
        lib/tinyxml2/tinyxml2.cpp

        CacheManagement.cpp
        ContentStream.cpp
        HlsPrimaryPlaylist.cpp
        HlsMediaPlaylist.cpp
        Middleware.cpp
        Service.cpp
        ServiceAnnouncement.cpp
        RestHandler.cpp
        seamless/CdnClient.cpp
        seamless/CdnFile.cpp
        seamless/SeamlessContentStream.cpp
        seamless/Segment.cpp

        PUBLIC
        #Receiver.h File.h
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.


find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        libflute

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${PROJECT_SOURCE_DIR}/lib/openssl-1.1.1k-clang/${ANDROID_ABI}/lib/libcrypto.a
        ${PROJECT_SOURCE_DIR}/lib/openssl-1.1.1k-clang/${ANDROID_ABI}/lib/libssl.a
        ${PROJECT_SOURCE_DIR}/lib/boost-1_79_0/libs/${ANDROID_ABI}/libboost_thread.a
        ${PROJECT_SOURCE_DIR}/lib/boost-1_79_0/libs/${ANDROID_ABI}/libboost_filesystem.a
        ${PROJECT_SOURCE_DIR}/lib/boost-1_79_0/libs/${ANDROID_ABI}/libboost_chrono.a
        ${PROJECT_SOURCE_DIR}/lib/boost-1_79_0/libs/${ANDROID_ABI}/libboost_atomic.a
        ${PROJECT_SOURCE_DIR}/lib/boost-1_79_0/libs/${ANDROID_ABI}/libboost_system.a
        ${PROJECT_SOURCE_DIR}/lib/cpprestsdk/libs/${ANDROID_ABI}/${CMAKE_BUILD_TYPE}/libcpprest.a
        )
