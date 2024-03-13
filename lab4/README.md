
# Hey! I'm Filing Here

In this lab, I successfully implemented the essential components required for creating a 1 MiB ext2 filesystem image. This includes the initialization of superblocks, inode tables, block groups, and the creation of a file system structure that consists of two directories, one regular file, and one symbolic link.

## Building

To compile the project and create the executable, run the following command in the terminal:

```bash
make
```

## Running

After building the project, you can create the ext2 filesystem image by executing:

```bash
./ext2-create
```

This command runs the compiled program, which generates a file named `cs111-base.img` in the current directory. 

To mount this filesystem and inspect its structure, use:

```bash
mkdir mnt
sudo mount -o loop cs111-base.img mnt
ls -l mnt
```

This will mount the filesystem image to the `mnt` directory, allowing you to explore its contents using standard file system operations.

## Cleaning up

To clean up the build artifacts and remove the compiled executable, run:

```bash
make clean
```

If you have mounted the filesystem, ensure to unmount it before deleting any directories:

```bash
sudo umount mnt
rmdir mnt
```
