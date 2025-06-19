
## Installation of arm compiler:
https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads







## Table of Contents

- [Add Pico SDK as a Git Submodule](#add-pico-sdk-as-a-git-submodule)
- [Add FreeRTOS as a pico submodule](#add-freertos-as-a-pico-submodule)


## Pros/cons Adding modules as submodules. 

A common way to include the pico-sdk and FreeRTOS in projects is by adding them as submodules.

However, another way is to keep these libraries cloned somewhere on your computer and point to them using environment variables, settings in the CMake build files. or running the scripts. 

Here are some pros and cons of using submodules:

**Pros:**

- The submodule is tied to a specific commit hash. This means if the external library changes in a way that breaks compatibility, your project will still be linked and use the version it was tested with.
- Users don’t have to worry about downloading or updating dependencies separately. Running `git submodule update --init --recursive` will get the right versions automatically.

**Cons:**

- Submodules add complexity to your git workflow, especially when updating them.
- It can take extra disk space because each submodule is its own git repository inside your project.
- Sometimes it’s better to keep shared libraries in one place and point to them, especially if you work on multiple projects using the same dependencies. Again disk space is saved. 


## Add Pico SDK as a Git Submodule

In short submodules is a way for you to include a project within a project - often used for third party dependecies. Read more here:  https://git-scm.com/book/en/v2/Git-Tools-Submodules

```bash
git submodule add https://github.com/raspberrypi/pico-sdk.git pico-sdk
```

This adds a file to your repo .gitmodules, which tracks which submodules you've added and where they live — see the block below for an example:

```ini
[submodule "pico/pico-sdk"]
	path = pico/pico-sdk
	url = https://github.com/raspberrypi/pico-sdk.git
```


The Pico SDK itself contains submodules — for example, `tinyusb`, `lwip`, and `btstack`. You can see the full list by inspecting the Pico SDK’s own `.gitmodules` file. These submodules may also have their own submodules (nested dependencies). To ensure that all dependencies, including nested ones, are initialized and downloaded, we use the following command:

```bash
git submodule update --init --recursive
```

When we ran the git submodule add command earlier, Git did two things:
1. It created a .gitmodules file, which tells Git where to fetch the submodule from.
2. It created a folder at the specified path (pico-sdk), which looks like a normal folder on your machine.

However, Git does not treat that folder like a regular directory with files. Instead, it tracks it as a gitlink — a special kind of entry in Git's index that points to a specific commit in the submodule's own Git history.

This means the contents of pico/pico-sdk are not actually stored in your main repository — only a reference to a commit from the submodule repo is.

So to commit this we do:

```bash
git add .gitmodules pico-sdk
```

This stages the `.gitmodules` file aswell as the gitlink. To be super clear - we are not commiting all of the SDK source code - just telling git where to find it and which commit to use. 

We can now commit this:

```bash
git commit -m "Add Raspberry Pi Pico SDK as submodule"
```

## Add FreeRTOS as a pico submodule

Similarly, we do the exact same thing for the FreeRTOS kernel. 

```bash
git submodule add https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS-Kernel

git submodule update --init --recursive

git add .gitmodules FreeRTOS-Kernel

git commit -m "Add FreeRTOS kernel as a submodule"
```


## Configuration of FreeRTOS

When using FreeRTOS, we are required to define a file called `FreeRTOSConfig.h`. We can read about this file and all its customizability [here](https://www.freertos.org/Documentation/02-Kernel/03-Supported-devices/02-Customization) or [here](https://github.com/FreeRTOS/FreeRTOS-Kernel-Book/blob/main/ch02.md#223-freertosconfigh). 

Some of this file settings should be unique and setup for each application, others settings could either be copy pasted between apps or linked dynamically. This files let us modify: 


- **Scheduler Behavior** — e.g. preemption and tick rate  
- **Memory Allocation** — e.g. for task creation, see [static](https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/02-xTaskCreateStatic) or [dynamic](https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate)
- **Hook Functions & Debugging** — e.g. setting callbacks for idle (runs when the task queue is empty) and ticks (called every RTOS tick interrupt), or for stack overflow and failed memory allocations
- **Runtime Statistics** — e.g. Trace Facility, [Trace Hook Macros](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/09-RTOS-trace-feature)
- **Synchronization Primitives** — e.g. mutexes, semaphores, queues
- **Co-Routines** — [intended for use on very small processors, not 32-bit MCUs like the RP2040](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/07-Co-routine-states)
- **Software Timers** — definitions for whether they should be used, what their priority is, etc.
- **Interrupt Priorities** — kernel interrupt priority, max syscall API interrupt priority
- **MPU and Secure Settings** — controls privilege levels, memory protection, and secure access  
  - e.g. `configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS`, `configTOTAL_MPU_REGIONS`, `configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY`, etc.
- **Optional API Functions** — it’s best to explicitly define what functions from FreeRTOS you will use here; however, most linkers will remove unused functions from the binary anyway


Create a Folder called Config in the top level workspace. FreeRTOS is expecting a File called  `FreeRTOSConfig.h` included from `FreeRTOS-Kernel/include/FreeRTOS.h`






## Setting up CMAKE.

Lets set up a top level `CMakeLists.txt` first. 

```cmake 
cmake_minimum_required(VERSION 3.14)

# Set project data
set(PROJECT_NAME "RP2040_FREERTOS_PROJECT")
set(BUILD_NUMBER "1")

# Set app name(s) and version(s)
set(APP_1_NAME "TEMPLATE")
set(APP_1_VERSION_NUMBER "1.5.0")

# Specify the app(s) source code
set(APP_1_SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/App-Template")
set(APP_2_SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/App-Scheduling")
set(APP_3_SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/App-IRQs")
set(APP_4_SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/App-Timers")

# FROM 1.3.0 -- Move common source code to a separate directory
set(COMMON_CODE_DIRECTORY "${CMAKE_SOURCE_DIR}/Common")

# FROM 1.4.2 -- Set app-side debugging on debug builds
# Now uses CMake generator expression to extract config type
add_compile_definitions($<$<CONFIG:Debug>:DEBUG=1>)

# Set env variable 'PICO_SDK_PATH' to the local Pico SDK
# Comment out the set() if you have a global copy of the
# SDK set and $PICO_SDK_PATH defined in your $PATH
set(ENV{PICO_SDK_PATH} "${CMAKE_SOURCE_DIR}/pico-sdk")

# Set flags and directory variables
set(FREERTOS_CFG_DIRECTORY "${CMAKE_SOURCE_DIR}/Config")
set(FREERTOS_SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel")

# Include the Pico SDK
include(pico_sdk_import.cmake)

# Name the project
project(${PROJECT_NAME}
        LANGUAGES C CXX ASM
        DESCRIPTION "FreeRTOS-based RP2040 applications"
)

# Initialise the Pico SDK
pico_sdk_init()

# Add FreeRTOS as a library
add_library(FreeRTOS STATIC
    ${FREERTOS_SRC_DIRECTORY}/event_groups.c
    ${FREERTOS_SRC_DIRECTORY}/list.c
    ${FREERTOS_SRC_DIRECTORY}/queue.c
    ${FREERTOS_SRC_DIRECTORY}/stream_buffer.c
    ${FREERTOS_SRC_DIRECTORY}/tasks.c
    ${FREERTOS_SRC_DIRECTORY}/timers.c
    ${FREERTOS_SRC_DIRECTORY}/portable/MemMang/heap_3.c
    ${FREERTOS_SRC_DIRECTORY}/portable/GCC/ARM_CM0/port.c
)

# Build FreeRTOS
target_include_directories(FreeRTOS PUBLIC
    ${FREERTOS_CFG_DIRECTORY}/
    ${FREERTOS_SRC_DIRECTORY}/include
    ${FREERTOS_SRC_DIRECTORY}/portable/GCC/ARM_CM0
)

# Include the apps' source code
add_subdirectory(${APP_1_SRC_DIRECTORY})
add_subdirectory(${APP_2_SRC_DIRECTORY})
add_subdirectory(${APP_3_SRC_DIRECTORY})
add_subdirectory(${APP_4_SRC_DIRECTORY})
```



## Setting it up using CMake FetchContent module

Instead of adding FreeRTOS and The pico-sdk as submodules in our project, an alternative is to use the FetchContent module in cmake. 


## Installing picotool

Since version 2.0.0 the sdk uses picotool to do the elf to uf2 conversions, and to hash and sign binaries. 
If picotool is not installed you will see a message each time you compile that it did not find a picotool available on your system and that it will download it. 

Setting it up will significantly speed up compile times, since it will no longer download and install it each time.

clone the picotool repo 

```bash
		git clone https://github.com/raspberrypi/picotool
``` 

to somewhere on your computer. i chose "~/Documents/dev"

cd into the repo

```bash
		cmake .. -DCMAKE_INSTALL_PREFIX=~/Documents/dev/picotool/install -DPICO_SDK_PATH="your_sdk_path"
```











