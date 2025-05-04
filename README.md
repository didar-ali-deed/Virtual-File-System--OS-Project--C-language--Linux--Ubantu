# Virtual File System Emulator (C Project)

This project implements a simple, in-memory Unix-like file system in C. Supported features:

## 🛠 Features
- Basic file system: `mkdir`, `touch`, `ls`, `cd`, `pwd`
- File management: `rm`, `rmdir`, `cp`, `mv`
- Ownership & permissions: `chown`, `chmod`
- Advanced: `ln` (symlinks), `find`, `df`, `du`
- Persistence: `save` and `load` the file system state

## ▶️ How to Compile and Run

```bash
make
./vfs
```

## 💡 Usage Example

```bash
mkdir test
cd test
touch file.txt
cd ..
ls
save fs.txt
load fs.txt
```

## 📁 Structure

- `src/` - source files (`main.c`, `vfs.c`)
- `include/` - header file (`vfs.h`)
- `Makefile` - build configuration
- `README.md` - this file

Enjoy your virtual Unix-like file system!
