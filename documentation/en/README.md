# Munix

![Munix Logo](../../pictures/munix-logo.jpg)

**An operating System made from Scratch that actually works**

[![GPL-3.0 License](https://img.shields.io/badge/License-GPL3-blue.svg)](https://opensource.org/licenses/GPL-3.0)
![Status](https://img.shields.io/badge/Status-Prototype-yellow)

## 📌 Descripción

Munix is an monolithic operating system written from scratch in C and NASM, currently in prototype phase.

## 🚀 Features

- **Monolithic Kernel** Written in C and assembly (NASM)
- **Shell revolucionaria** with it's own language
  - non-existent variables (id based variables to avoid hash tables)
  - Customized Lexer, parser and evaluator
- Support for MBR (Master Boot Record)
- **Customized C Standard Libary** (partially compatible with libc)
- Support for Standard Hardware:
  - **Keyboard** : PS/2
  - **Display** : VGA 
  - **Disk** : ATA-PIO

## 🖥️ System requirements

- : i386 (x86 32-bit)
- Supported Hardware:
  - Every system with UEFI support
  - PS/2 Keyboard (i think that USB Legacy from UEFI/BIOS Options work too)
  - Standard VGA
  - ATA-PIO Controller 

## 🛠️ Installing

1. **Prerequisites**:
   - i386 toolchain
   - GCC
   - NASM
   - GRUB (`grub-mkrescue`)
   - QEMU (for testing the system)

2. **Process** (from folder Munix/MunixOs):
   ```bash
   ./debug.sh compile
   ./debug.sh run 
   ```
  *Note*: for more information about the construction script see [Script Documentation](debug-docs.md)

*(Note: Currently the system runs by a iso generated with grub-mkrescue)*

## 📸 Screenshots

![Shell Screenshot 01](../../screenshots/screenshot1.png)
-----
![Shell Screenshot 02](../../screenshots/screenshot2.png)

## 🧠 Munix Shell (mbash)


```mbash
0:"var" = 42  # Id-based variable
echo $0:"var"  # Show value
```

Features:
- Customizable Command Handler (it's customizable from the source code at `Munix/MunixOs/Munix/kernel/mbash/eval.c`, by default returns EVAL_OK)
- Advanced Interpreted Language

## 📜 Roadmap

- [x] Support for MBR Partitions
- [ ] Support for x86_64
- [ ] Basic FileSystem, like FAT32
- [x] ATA-PIO Drivers
- [ ] Basic Scheduler
- [ ] Functioning Shell Installer

## 🤝 Contributing

¡Every contribution is welcome! Please:
1. Open an issue to discuss the changes
2. Fork the project
3. Creater your branch (`git checkout -b feature/AmazingFeature`)
4. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
5. Push your branch (`git push origin feature/AmazingFeature`)
6. Open a pull request

## 📚 Resources

This project is possible thanks to:
- [OSDev Wiki](https://wiki.osdev.org/) - Invaluable Resources for development of the operating system

## 📄 License

Distributed under GPL-3.0 License, see LICENSE for more information

---

*"It isn't a bug, it's an undocumented feature."* - Munix Philosophy
