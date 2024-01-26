# A Kernel Seedling
TODO: The "Process Count" kernel module is a simple tool designed to provide information about the number of running processes on a Linux system. When loaded, this module creates a `/proc/count` file that users can read to obtain the current count of processes actively running on the system.


## Building
```shell
TODO: make
```

## Running
```shell
TODO: sudo insmod process_monitor.ko cat /proc/process_monitor


```
TODO: results? 162

## Cleaning Up
```shell
TODO: make clean sudo rmmod proc_count.ko
```

## Testing
```python
python -m unittest
```
TODO: results? 

    Ran 3 tests in 0.712s

    OK


Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```
TODO: kernel ver? 5.14.8-arch1-1