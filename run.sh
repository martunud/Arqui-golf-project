#!/bin/bash
sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512

#Para mac, el audio funciona con coreaudio
#sudo qemu-system-x86_64 -machine type=pc,pcspk-audiodev=speaker -audiodev driver=coreaudio,id=speaker -hda Image/x64BareBonesImage.qcow2 -m 512
