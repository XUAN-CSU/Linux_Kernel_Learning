savedcmd_hrt.ko := ld -r -m elf_x86_64 -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T /usr/src/linux-headers-6.14.0-27-generic/scripts/module.lds -o hrt.ko hrt.o hrt.mod.o .module-common.o
