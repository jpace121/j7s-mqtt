# j7s-mqtt

## Dependencies

On Debian 11:
```
sudo apt install libgpiod-dev
```

Need to manually install paho libraries.
I'll **eventually** add these to my apt repo.

Currently, these are installed system wide.

```
sudo pip3 install vcstool

vcs import < paho.repos
cd paho/paho.mqqt.c
make
sudo make install
cd ../paho.mqqt.cpp
mkdir build
cd build
cmake ..
make
sudo make install

sudo ldconfig
```
