#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

#define EVIOCGVERSION _IOR('E', 0x01, int)

typedef uint32_t __u32;
typedef uint16_t __u16;
typedef __signed__ int __s32;

struct input_event
{
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
};

int main(int argc, char *argv[])
{
    // Don't pass arguments they have no effect
    if (argc != 1)
    {
        fprintf(stderr, "Too many arguments passed\n");
        return 1;
    }

    // Open recorded events file
    FILE *recording_fd = fopen("/sdcard/events.txt", "r");
    if (recording_fd == NULL)
    {
        fprintf(stderr, "Could not open file at /sdcard/events.txt\n");
        return 1;
    }

    int i, dir, version, event_fds[64];
    struct dirent **namelist;
    char path[128];

    // Open all the event files in alphabetical order and save the fd to an array
    dir = scandir("/dev/input/", &namelist, 0, alphasort);
    if (dir < 0)
    {
        fprintf(stderr, "Could not scan directory /dev/input/\n");
    }
    else
    {
        // Start from i = 2 to skip the "." and ".." items
        for (i = 2; i < dir; i++)
        {
            // Full path to event file
            strcpy(path, "/dev/input/");
            strcat(path, namelist[i]->d_name);
            
            event_fds[i - 2] = open(path, O_RDWR);
            if (event_fds[i - 2] < 0)
            {
                fprintf(stderr, "Could not open %s\n", path);
                return 1;
            }
            if (ioctl(event_fds[i - 2], EVIOCGVERSION, &version))
            {
                fprintf(stderr, "Could not get driver version for %s\n", path);
                return 1;
            }

            memset(path, 0, sizeof(path));
        }
    }

    char line[196], device[128], type[32], code[32], value[32];
    int event_fd, device_id;
    struct input_event event;
    ssize_t ret;
    double timestamp, timestamp_prev = -1.0;
    unsigned int sleep_diff;

    // Loop through each event in the events.txt file
    while (fgets(line, sizeof(line), recording_fd) != NULL)
    {
        // Remove the square brackets
        memmove(&line[0], &line[0 + 1], strlen(line) - 0);
        memmove(&line[15], &line[15 + 1], strlen(line) - 15);

        // Get each line value
        sscanf(line, "%lf %s %s %s %s", &timestamp, device, type, code, value);

        // Skip the device description lines that come in events.txt
        if (timestamp < 1)
        {
            continue;
        }

        // String to integer
        memset(&event, 0, sizeof(event));
        event.type = (int)strtol(type, NULL, 16);
        event.code = (int)strtol(code, NULL, 16);
        event.value = (uint32_t)strtoll(value, NULL, 16);

        // Converts "/dev/input/event0:" to "0" and gets the corresponding fd for that device
        device[strlen(device) - 1] = '\0';
        device_id = atoi(strtok(device, "/dev/input/event"));
        event_fd = event_fds[device_id];

        // Sleeps between events to match recording
        if (timestamp_prev != -1.0)
        {
            sleep_diff = (unsigned int)((timestamp - timestamp_prev) * 1000000);
            usleep(sleep_diff);
        }
        timestamp_prev = timestamp;

        // Injects event
        ret = write(event_fd, &event, sizeof(event));
        if (ret < (ssize_t)sizeof(event))
        {
            fprintf(stderr, "Write event failed: %s\n", strerror(errno));
            return -1;
        }

        memset(line, 0, sizeof(line));
        memset(device, 0, sizeof(device));
        memset(type, 0, sizeof(type));
        memset(code, 0, sizeof(code));
        memset(value, 0, sizeof(value));
    }

    return 0;
}
