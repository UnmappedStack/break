# break

A Cargo-like build system for C, which is self hosting - meaning it can build itself!

## Usage
### Installation
#### First time installing
The first time that you install `break`, you almost certainly won't have a copy of `break` already on your system, meaning you'll need to build with `make` instead.

To install `break` to your system, run the following commands. Note that `break` only works on Linux systems.

```bash
git clone https://github.com/UnmappedStack/break
cd break
make install
```

You will likely be asked for a password for root privileges, this is required to install it to your `/usr/bin` directory.

#### Self hosting (building `break` with itself)
You'll still need root privileges to run some of these commands for the same reasons described above, and `break` still only works on Linux systems.

```bash
git clone https://github.com/UnmappedStack/break
cd break
breakpkg build --release
sudo cp target/release/breakpkg /usr/bin/breakpkg
```

### Usage
There are a few basic commands you can use with `break`:

- Initiate a new `break` project with `breakpkg init` in the current directory. The directory must be empty. It will create a `break.toml` and a `src/` directory with a "hello world" as well as an `include/` directory.
    ```bash
    breakpkg init
    ```
- Build the project with `breakpkg build`. You can also use `--release` to output a release version of the executable, which will have optimisation. This will output the executable to `target/release/[projectname]` or `target/debug/[projectname]`, depending on whether or not you use `--release`.
    ```bash
    breakpkg build
    # OR #
    breakpkg build --release
    ```
- Build and run the project with `breakpkg run`. This builds and runs the project.
    ```bash
    breakpkg run
    ```
- Run the project without building with `breakpkg runonly`. If there's a release executable then it will run that, otherwise it'll run a debug executable.
    ```bash
    breakpkg runonly
    ```
- Find other information with `breakpkg version` and `breakpkg help`.

#### Writing a `break.toml`
`break` has a *very basic* TOML parser. Note that it is far from complete and lacks many TOML features. This is a list of all the options, hopefully you can learn from example:
```toml
[package]
name = "testproject"
version = "0.0.1"
edition = "2024"
compiler = "gcc"
freestanding = "false"
warnerror = "true" # enabling makes the compiler use `-Wall -Werror`
```
The compiler that you use must allow GCC-like command line arguments. I personally recommend Clang.

## License

This project is under the Mozilla Public License. See `LICENSE` for more information.
