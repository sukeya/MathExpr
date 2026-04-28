#!/bin/bash

sudo apt update
sudo apt install -y ninja-build build-essential

# install cmake
# add Kitware apt repository
sudo apt install -y ca-certificates
# add Kitware's signing key
test -f /usr/share/doc/kitware-archive-keyring/copyright ||
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
# add repository to source list
echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | sudo tee /etc/apt/sources.list.d/kitware.list >/dev/null
sudo apt update
# rm the obtained signed key
test -f /usr/share/doc/kitware-archive-keyring/copyright ||
sudo rm /usr/share/keyrings/kitware-archive-keyring.gpg
# install keyring to stay up to date
sudo apt install -y kitware-archive-keyring
sudo apt install -y cmake
