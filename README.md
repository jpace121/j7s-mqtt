# j7s-mqtt

For cpp build, need to install paho libraries. Currently, these are installed system
wide, but it is my goal to eventually get them to build with colcon.

```
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
