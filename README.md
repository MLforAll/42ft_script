# ft_script

This 42 project aims to recode the `script` command line utility to learn more about pty/tty.

## Features

This ft_script implements all options of the original

- `a` Append the output to _file_ or typescript, retaining the prior contents
- `d` When playing back a session with the -p flag, do not sleep between records
- `F` Immediately flush output after each write
- `k` Log keys sent to the program as well as output
- `p` Play back a session recorded with the -r flag in real time
- `q` Run in quiet mode, omit the start, stop and command status messages
- `r` Record a session with input, output, and timestamping
- `t` Specify the interval at which the script output file will be flushed to disk (in seconds)

The `-r`, `-d` and `-p` flags are compatible with the origin `script` utility. You can record a session with `script` and play it with `ft_script`, or vice-versa.

It also adds the `SCRIPT` env var to the sub-shell (the value is _file_ or typescript)

## Building

To build, just issue :

~~~
$ make
~~~

To clean :

~~~
$ make clean
~~~

or, if you want to clean the built library as well :

~~~
$ make fclean
~~~