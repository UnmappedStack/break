# break

A Cargo-like build system for C.

The goal is to make the project self hosting (at some point™). Usage and build instructions will be provided once the project is complete enough.

## Usage
### Installation
To install `break` to your system, run the following commands. Note that `break` only works on Linux systems.

```bash
git clone https://github.com/UnmappedStack/break
cd break
make install
```

You will likely be asked for a password for root privileges, this is required to install it to your `/usr/bin` directory.

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
```
The compiler that you use must allow GCC-like command line arguments. I personally recommend Clang.

## License

This project is under the Mozilla Public License. See `LICENSE` for more information.
