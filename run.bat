@echo off
"C:\Program Files\qemu\qemu-system-x86_64.exe" -M q35 -m 2G -cdrom bin/vision.iso -boot d -debugcon stdio