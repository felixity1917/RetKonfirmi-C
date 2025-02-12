# RetKonfirmi-C
RetKonfirmi (EN: Lit., Web-Confirm) is a C Program that checks for and confirms the status of a website/page on whether it is up or not. This is a C Port of the ZSH Script of the same name.

Usage: `retkonfirmi-c www.example.com [OPTIONS]`

This program checks the status of a website and prints if it is up or down.

# Building
Simply run:
```sh
    make
```
Make sure to have libcurl installed on your system.

# Options:
```sh
  --hide_time        Hide the timestamp in the output.
  --monochrome       Disable colored output.
  --interval <sec>   Set the check interval in seconds (default: 15 seconds).
  --log              Enable logging of messages to a log file.
  --logdir <path>    Specify a custom directory for logs (default: $HOME).
  --help             Display this help message and exit.
```

# Example:
```sh
  ./retkonfirmi-c www.example.com --interval 30 --log --logdir /tmp
```
