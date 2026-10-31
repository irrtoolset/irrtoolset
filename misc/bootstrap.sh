#!/bin/sh
# Install the tools and libraries needed to build IRRToolSet.
# Set DRY_RUN=1 to print the command instead of running it.
set -eu

run() {
    if [ "${DRY_RUN:-}" = 1 ]; then
        echo "$*"
    elif [ "$(id -u)" -ne 0 ] && command -v sudo >/dev/null 2>&1; then
        sudo "$@"
    else
        "$@"
    fi
}

have() { command -v "$1" >/dev/null 2>&1; }

if have apk; then
    run apk add build-base git cmake bison flex readline-dev
elif have apt-get; then
    run apt-get update
    run apt-get install -y build-essential git cmake bison flex libreadline-dev
elif have dnf; then
    run dnf install -y gcc-c++ make git cmake bison flex readline-devel
elif have yum; then
    run yum install -y gcc-c++ make git cmake bison flex readline-devel
elif have pacman; then
    run pacman -S --needed --noconfirm base-devel git cmake bison flex readline
elif have zypper; then
    run zypper install -y gcc-c++ make git cmake bison flex readline-devel
elif have pkg; then
    run pkg install -y cmake git bison flex readline
elif have brew; then
    # sudo must not be used with brew
    if [ "${DRY_RUN:-}" = 1 ]; then
        echo brew install cmake bison flex readline
    else
        brew install cmake bison flex readline
    fi
else
    echo "bootstrap: unsupported platform, install a C++ compiler, cmake, bison, flex and readline manually" >&2
    exit 1
fi
