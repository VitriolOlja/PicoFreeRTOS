


## Table of Contents

- [Add Pico SDK as a Git Submodule](#add-pico-sdk-as-a-git-submodule)
- [Add FreeRTOS as a pico submodule](#add-freertos-as-a-pico-submodule)


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
2. It created a folder at the specified path (pico/pico-sdk), which looks like a normal folder on your machine.

However, Git does not treat that folder like a regular directory with files. Instead, it tracks it as a gitlink — a special kind of entry in Git's index that points to a specific commit in the submodule's own Git history.

This means the contents of pico/pico-sdk are not actually stored in your main repository — only a reference to a commit from the submodule repo is.

So to commit this we do:<>

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








