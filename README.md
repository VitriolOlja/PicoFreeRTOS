# PicoFreeRTOS


This is a guide mostly written for myself on how to use FreeRTOS on the Raspberry PICO rp2040
It is highly based on the port by [Tony Smith](https://github.com/smittytone/RP2040-FreeRTOS/tree/main)
aswell as [this one](https://github.com/carlk3/FreeRTOS-FAT-CLI-for-RPi-Pico/blob/master/src/FreeRTOS%2BFAT%2BCLI/src/freertos_callbacks.c) by Carl J Kugler. 


## Building the application

For a short hand guide of the scripts to build the application, see below. For more detailed view of how to set up the sdk, freertos and picotool directories see [`guide.md`](guide.md)

```bash
    mkdir build
```

```bash
    cd build
```

I chose to use ninja as a generator here, but you can of course use make aswell. 

```bash
    cmake -G Ninja .. `
      -DPICO_SDK_PATH="path to your pico sdk"`
      -DFREERTOS_KERNEL_PATH="path to your freertos kernel " `
      -DPICO_BOARD=pico_w `
      -Dpicotool_DIR= "path to picotool directory"
```

```bash
    ninja
```




