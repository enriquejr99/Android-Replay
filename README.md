# android-replay

Record and replay touchscreen events (e.g. tapping/swipes) and key presses (e.g. volume up/down) on an Android device without lag through ADB.

<img src="https://user-images.githubusercontent.com/79606297/109591927-58840780-7b06-11eb-8805-32d6cf3cb8a4.gif"/>

## How it works
Other tools using Android's `sendevent` binary to replay events lag as this binary opens and closes a `/dev/input/` event file with each event it injects. The custom `replayevents` binary solves this issue by opening all the event files at start and only closing them once the replay is over. Events are replayed in a timely manner by sleeping through periods where there is no input activity.

## Requirements

- Python 3
- ADB
- Device with an ARM CPU

Tested on a Samsung Galaxy S9+ running Android 10, but it should work with most other devices and Android versions. The source file for the custom binary `replayevents.c` can be compiled to x86 too, but the one included here is the ARM version.

To find out if your device uses an ARM CPU use: `adb shell cat /proc/cpuinfo`

## Usage
Android device must be plugged in for ADB to work. Notice the replay is handled by the `replayevents` binary, therefore exiting the Python script will not stop the replay. To stop the replay, you may find and kill the process running it.

### Record
1. Open a command prompt and run the `record.py` script
2. Use the device as desired
3. Stop the script

### Replay
1. Open a command prompt and run the `replay.py` script
2. Watch your device repeat the input events recorded

## Modifying the binary
Use the source file for the custom binary `replayevents.c` to modify the tool as needed. Then use `arm-linux-gnueabi-gcc -static -march=armv7-a replayevents.c -o replayevents` to compile it.
