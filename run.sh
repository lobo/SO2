#!/bin/bash
#export QEMU_AUDIO_DRV=alsa

qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 1024 -soundhw all -vga std
