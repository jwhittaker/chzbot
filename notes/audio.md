# Playing Audio

## Sources

https://wiki.archlinux.org/index.php/Advanced_Linux_Sound_Architecture#An_example_of_setting_default_device_using_.22defaults.22_node

https://learn.adafruit.com/usb-audio-cards-with-a-raspberry-pi/updating-alsa-config


List of commands for audio information

## Sound Hardware Support

Since the two broadcom hardware PWM pins are being assigned for servo control, a USB sound device is used instead.

```
sudo apt-get install -qq -y alsa-utils alsa-tools alsaplayer-text
sudo systemctl restart alsa-restore.service
sudo speaker-test -c2


# snd_usb_audio is a loaded alsa module
cat /usr/share/alsa/alsa.conf
```

Didn't I have to change something in the conf to get it to use the USB?

## Sound Software

For audio playback and conversion.

```
sudo usermod -aG audio mse
sudo /bin/sh -c 'echo "dtparam=spi=on" >> /boot/config.txt'
sudo /bin/sh -c 'echo "dtparam=i2c_arm=on" >> /boot/config.txt'
```



```
amixer is a shell command to change audio settings
alsamixer provides a more intuitive ncurses based interface for audio device configuration.
```


### My Setup

```
USB Mixer: usb_id=0x0d8c000c, ctrlif=0, ctlerr=0
Card: C-Media USB Headphone Set at usb-20980000.usb-1.1, full speed
idVendor           0x0d8c C-Media Electronics, Inc.
idProduct          0x000c Audio Adapter
```


## Install Sizes

- `mplayer` **254 MB**
- `ffmpeg` **256 MB**
- `audacious` **394 MB**
- `vlc` **485 MB**
- `sox` & `libsox-fmt-all` & `libsox-dev` **4 MB**

## sox

Choosing only sox, because it does what we require and takes the least space.

`sudo apt-get install -qq -y sox libsox-fmt-all libsox-dev`

http://sox.sourceforge.net/

[Formats and features](http://sox.sourceforge.net/Docs/Features)

### Add permission to use sound

Our user needs to be added to the audio group.

`sudo usermod -aG audio mse`

### Raw play without setup

`sox soundfile.wav -t raw -r 8012 -u -b -c 1 | cat > /dev/audio`

### Show File Metadata

`soxi ~/sounds/mp3/MSE01.mp3`

### Play File

`play ~/sounds/mp3/MSE01.mp3`

```
play WARN sox: alsa output clipped 46 samples; decrease volume?
```

Playback with decreased amplitude:

`play -v 0.85 ~/sounds/mp3/MSE01.mp3`

### Quiet Mode

Doesn't draw stuff to the terminal while playing back.

`play -q MSE01.mp3`


## C Forking

spawn child fork to play the `play` program from within C.

Get full path to program
`$ which sox`
`/usr/bin/sox`

https://www.usna.edu/Users/cs/aviv/classes/ic221/s16/lec/14/lec.html
With execvp, you do not need to specify the full path because execvp will search the local environment variable PATH for the executable. Not recommended for security reasons.
 This is the same as the argv array argument to main; essentially, when you call exec you are calling that programs main function.

Just like in main, the argv array must be NULL terminated
`char * ls_args[] = { "ls" , "-l", NULL};`


**Linux Programming Interface - by Michael Kerrisk**

### system()

`system()` is a higher level interface that can have security issues and is less efficient.
`int system(const char *command);`
http://man7.org/linux/man-pages/man3/system.3.html
The system() library function uses fork(2) + execl(3) + waitpid()
https://stackoverflow.com/questions/1697440/difference-between-system-and-exec-in-linux
https://ece.uwaterloo.ca/~dwharder/icsrts/Tutorials/fork_exec/

`execl("/bin/sh", "sh", "-c", command, (char *) 0);`

system() provides simplicity and convenience: it handles all of the
details of calling fork(2), execl(3), and waitpid(2), as well as the
necessary manipulations of signals; in addition, the shell performs
the usual substitutions and I/O redirections for command.  The main
cost of system() is inefficiency: additional system calls are
required to create the process that runs the shell and to execute the
shell.

```
#include <unistd.h>
int execle(const char * pathname , const char * arg , ... /* , (char *) NULL, char *const envp [] */ );
int execlp(const char * filename , const char * arg , ... /* , (char *) NULL */);
int execvp(const char * filename , char *const argv []);
int execv(const char * pathname , char *const argv []);
int execl(const char * pathname , const char * arg , ... /* , (char *) NULL */);
Return Val: None of the above returns on success; all return –1 on error
```

### fork(), exec(), waitpid()
https://stackoverflow.com/questions/2667095/how-to-get-the-return-value-of-a-program-ran-via-calling-a-member-of-the-exec-fa
https://ece.uwaterloo.ca/~dwharder/icsrts/Tutorials/fork_exec/

#### Overview

```
child_pid = fork();
if(child_pid == 0) {
  exec("program", "args", NULL);
}
waitpid(child_pid, &status_value, WUNTRACED)
printf("%s", WEXITSTATUS(status_value))
```

The shell instead waits for the child to finish process before prompting again, and there is a system call called wait() to just do that.
Once the parent calls wait(), it will block until a child changes state. In essence, it is waiting on its children to terminate. This is described as a blocking function because it blocks and does not continue until an event is complete.
Once it returns, wait() will returns the pid of the child process that terminated

To learn about the exit status of a program we can use the macros from sys/wait.h which check the termination status and return the exit status. From the man page:

```
WIFEXITED(status)
       returns true if the child terminated normally, that is, 
       by calling exit(3) or _exit(2), or by returning from main().

WEXITSTATUS(status)
       returns  the  exit  status of the child.  This consists of the least significant 
       8 bits of the status argument that the child specified in a call to exit(3) or _exit(2) or as the
       argument for a return statement in main().  
       This macro should only be employed if WIFEXITED returned true.
```


#### Notes

https://stackoverflow.com/questions/42840950/waitpid-and-fork-execs-non-blocking-advantage-over-a-syscall

The `WNOHANG` flag (set in the options argument) will make the call to waitpid() non-blocking.
You'll have to call it periodically to check if the child is finished yet.
Or you could setup SIGCHLD to take care of the children.

If you want to do other stuff whilst the child process is off doing it's thing, you can set up a trap for SIGCHLD that handles the child finishing/exiting. 

```
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

pid_t pid;
int finished=0;

void
zombie_hunter(int sig) {
  int status;
  waitpid(pid, &status, 0);
  printf("Got status %d from child\n",status);
  finished=1;
}

int
main(void) {
  sigaction(SIGCHLD,zombie_hunter);
  pid = fork();
  if (pid == -1) {
    exit(1);
  } 
  else if (pid == 0) {
    sleep(10);
    exit(0);
  }
  while(!finished) {
    printf("waiting...\n");
    sleep(1);
  }
}
```

if the parent doesn't need to know or care when the child terminates, then it is possible to avoid any need to wait for it at all, ever.
 
But I want to know when the sound is done playing so it doesn't overlap or stomp what's going.

### Boot config file 

`sudo vim /boot/config.txt`

```
# Enable UART
enable_uart=1
```