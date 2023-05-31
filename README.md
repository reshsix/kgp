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
KGPKEY=000102030405060708090A0B0C0D0E0F kgp encrypt LAPPLAND file file.enc
KGPKEY=000102030405060708090A0B0C0D0E0F kgp decrypt LAPPLAND file.enc file2
```
