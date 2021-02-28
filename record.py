import subprocess


def record():
    print("Recording started")
    print("Stop it with: CTRL + C")
    subprocess.check_call("adb exec-out getevent -t > events.txt", shell=True)


if __name__ == "__main__":
    try:
        record()
    except KeyboardInterrupt:
        print()
        print("Recording stopped")
