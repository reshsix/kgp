# kgp
Kinda Good Privacy

Personal program implementing experimental ciphers,
suggestions and possible attacks are welcome

The name is a pun with Pretty Good Privacy and sounds like KGB

## Requirements
A POSIX environment

## Installation
```sh
make
sudo make install
```

## Usage
```sh
export KGPKEY=ace7fd046b69412ae6dade27f9cac445
kgp encrypt MISAKA file file.enc
kgp decrypt MISAKA file.enc file2
```
