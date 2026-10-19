<p align="center">
  <img src=".github/banner.svg" width="100%" alt="Reference Tools · 5G Broadcast for TV and Radio Services: MBMS Android Middleware">
</p>

<p align="center">
  Android middleware that receives media over 5G Broadcast or unicast (OTT streaming) and serves
  it to the media player on the device.
</p>

<p align="center">
  <img alt="Status" src="https://img.shields.io/endpoint?url=https%3A%2F%2Fwww.5g-mag.com%2Fbadges%2Fstatus%2Frt-mbms-mw-android.json">
  <a href="https://github.com/5G-MAG/rt-mbms-mw-android/releases"><img alt="Version"
    src="https://img.shields.io/github/v/release/5G-MAG/rt-mbms-mw-android?label=Version"></a>
  <a href="LICENSE"><img alt="License: GNU Affero General Public License v3.0"
    src="https://img.shields.io/badge/License-AGPL%20v3.0-blue"></a>
</p>

<p align="center">
  <a href="https://www.5g-mag.com/reference-tools/5g-broadcast/">Project page</a> &nbsp;&middot;&nbsp;
  <a href="https://github.com/5G-MAG/rt-mbms-mw-android/issues">Issues</a> &nbsp;&middot;&nbsp;
  <a href="https://www.5g-mag.com/contributing">Contributing</a>
</p>

---

## At a glance

|  |  |
|---|---|
| **Part of** | [5G Broadcast for TV and Radio Services](https://www.5g-mag.com/reference-tools/5g-broadcast/), alongside [rt-libflute](https://github.com/5G-MAG/rt-libflute), [rt-mbms-application](https://github.com/5G-MAG/rt-mbms-application), [rt-mbms-application-provider](https://github.com/5G-MAG/rt-mbms-application-provider), [rt-mbms-bmsc](https://github.com/5G-MAG/rt-mbms-bmsc), [rt-mbms-client](https://github.com/5G-MAG/rt-mbms-client), [rt-mbms-examples](https://github.com/5G-MAG/rt-mbms-examples), [rt-mbms-gw](https://github.com/5G-MAG/rt-mbms-gw), [rt-mbms-modem](https://github.com/5G-MAG/rt-mbms-modem), [rt-mbms-mw](https://github.com/5G-MAG/rt-mbms-mw), [rt-mbms-tx](https://github.com/5G-MAG/rt-mbms-tx), [rt-mbms-tx-for-qrd-and-crd](https://github.com/5G-MAG/rt-mbms-tx-for-qrd-and-crd), [rt-wui](https://github.com/5G-MAG/rt-wui) |

## Introduction

The MBMS Middleware for Android is a port of the
[MBMS Middleware](https://github.com/5G-MAG/rt-mbms-mw) to Android. Based on an MBMS Service
Announcement, it receives media manifests and segments directly over 5G Broadcast or fetches them
over unicast from a CDN, and serves the files to the media player from a local web server. The
device can therefore switch between broadcast and unicast without the media player knowing how
each file was received.

Packets received over 5G Broadcast are accessed through the
[MbmsGroupCallSession API](https://developer.android.com/reference/android/telephony/MbmsGroupCallSession).

### Acknowledgement

The MBMS MW Android was originally developed by Bitstem, Nakolos and ORS.

## Install dependencies

Install [Git Large File Storage](https://docs.github.com/en/repositories/working-with-files/managing-large-files/installing-git-large-file-storage)
before cloning the repository.

## Downloading

Release versions can be downloaded from
the [releases](https://github.com/5G-MAG/rt-mbms-mw-android/releases) page.

To build from source, clone the repository:

```
cd ~
git clone https://github.com/5G-MAG/rt-mbms-mw-android
```

## Building

Generate the `apk` bundles:

```
./gradlew assemble
```

The bundles are written to `app/build/outputs/apk`: the debug build in the `debug` folder, the
release build in the `release` folder.

### Troubleshooting

* If you get the error `gradlew: Permission Denied`, set the execution flag on the gradlew
  file: `chmod +x gradlew`

## Installing

To install the `apk` on an Android device:

1. Connect the Android device to your development machine.
2. Run `adb devices` to list the available Android devices. The output should look like this:

```
List of devices attached
CQ30022U4R	device
```

3. Install the `apk` on the target
   device: `adb -s <deviceID> install -r app/build/outputs/apk/debug/app-debug.apk`. The `-r`
   option reinstalls an existing app and keeps its data.

## Running

Once installed, start the MBMS MW Android application from the Android app selection screen.

## Development

This project follows
the [Gitflow workflow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow).
The `development` branch is the integration branch for new features, so switch to it before
starting work on a new feature.

## Contributing

Contributions are welcome. How to raise an issue, fork the repository and open a pull request, and
the Contributor License Agreement required before code can be merged, are described at
<https://www.5g-mag.com/contributing>.

## License

Distributed under the GNU Affero General Public License v3.0. See [LICENSE](LICENSE). The native
code in `app/src/main/cpp` is derived from the 5G-MAG MBMS Middleware. Attribution for that code
and for the third-party software used is in [ATTRIBUTION_NOTICE](ATTRIBUTION_NOTICE).
