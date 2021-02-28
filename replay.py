import subprocess


def push_files():
    subprocess.check_call("adb push events.txt /sdcard/", shell=True)
    subprocess.check_call("adb push replayevents /data/local/tmp/", shell=True)
    subprocess.check_call("adb shell chmod +x /data/local/tmp/replayevents", shell=True)


def remove_files():
    subprocess.check_call("adb shell rm /sdcard/events.txt", shell=True)
    subprocess.check_call("adb shell rm /data/local/tmp/replayevents", shell=True)


def replay():
    print("Replay started")
    subprocess.check_call("adb shell su -c /data/local/tmp/replayevents", shell=True)
    print("Replay ended")


if __name__ == "__main__":
    try:
        push_files()
        replay()
    except KeyboardInterrupt:
        pass
    finally:
        remove_files()
