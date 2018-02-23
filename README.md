Tensorflow has a tool called `prototext`.

It is bundled with Tensorflow so tightly. This repo extracts essential code.

### Install google protobuf

You have to install protobuf and then change the locations in CMakeLists.txt

```bash
# cd to protobuf directory
mkdir output
./autogen.sh
./configure --prefix={path-to-output}
make && make install
```

### Modify CMakeLists.txt

```CMake
INCLUDE_DIRECTORIES()
TARGET_LINK_LIBRARIES()
```

### Execute command

```bash
mkdir build
cd build && cmake ..
```
