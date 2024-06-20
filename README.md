<h1 align="center">MBMS MW Android</h1>
<p align="center">
  <img src="https://img.shields.io/badge/Status-Under_Development-yellow" alt="Under Development">
  <img src="https://img.shields.io/github/v/tag/5G-MAG/rt-mbms-mw-android?label=version" alt="Version">
  <img src="https://img.shields.io/badge/License-5G--MAG%20Public%20License%20(v1.0)-blue" alt="License">
</p>

## Introduction

The MBMS Middleware for Android enables the reception of media content via 5G Broadcast and
unicast (OTT streaming). Based on an MBMS Service Announcement the media manifests and segments are
either received directly via 5G Broadcast or fetched via unicast from a CDN. The MBMS Middleware
exposes the received media files via a local webserver to the MediaPlayer. As a result, the device
can dynamically switch between broadcast and unicast consumption without the media player being
aware of how the media files were originally received.

### About the implementation

The implementation is basically a port of
the [MBMS Middleware](https://github.com/5G-MAG/rt-mbms-mw) to Android. It uses
the [MbmsGroupCallSession API](https://developer.android.com/reference/android/telephony/MbmsGroupCallSession)
for accessing packets received via 5G Broadcast.

## Install dependencies

The MBMS MW Android requires Github Large File Storage to be installed before cloning the
repository. Please check
the [Github documentation](https://docs.github.com/en/repositories/working-with-files/managing-large-files/installing-git-large-file-storage)
for details.

## Downloading

Release versions can be downloaded from
the [releases](https://github.com/5G-MAG/rt-mbms-mw-android/releases) page.

The source can be obtained by cloning the github repository.

```
cd ~
git clone https://github.com/5G-MAG/rt-mbms-mw-android
```

## Building

Call the following command in order to generate the `apk` bundles.

````
./gradlew assemble
````

The resulting `apk` bundles can be found in `app/build/outputs/apk`. The debug build is located
in `debug` folder the release build in the `release` folder.

## Install

To install the `apk` on an Android device follow the following steps:

1. Connect your Android device to your development machine
2. Call `adb devices` to list the available Android devices. The output should look like the
   following:

````
List of devices attached
CQ30022U4R	device
````

3. Install the `apk` on the target
   device: `adb -s <deviceID> install -r app/build/outputs/apk/debug/app-debug.apk`. Using `-r`
   we reinstall an existing app, keeping its data.

## Running

After installing the Media Session Handler application can be started from the Android app selection
screen.

As an alternative we can also run the app from the command
line: `adb shell am start -n com.fivegmag.a5gmsmediasessionhandler/com.fivegmag.a5gmsmediasessionhandler.MainActivity `

## Development

This project follows
the [Gitflow workflow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow)
. The `development`
branch of this project serves as an integration branch for new features. Consequently, please make
sure to switch to the `development`
branch before starting the implementation of a new feature. 
