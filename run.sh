#Flags para audio y la hora
sudo qemu-system-x86_64 -machine type=pc,pcspk-audiodev=speaker -audiodev driver=coreaudio,id=speaker -hda Image/x64BareBonesImage.qcow2 -m 512 -rtc base=localtime
