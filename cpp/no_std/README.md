# Use Google Test to verify

## Get cmake
```
sudo apt-get install cmake
```
## Get gtest
```
sudo apt-get install libgtest-dev
```
## Install gtest
```
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp ./lib/*.a /usr/lib
```

## Test
```
make test
```
