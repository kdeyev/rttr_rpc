# rrtRpc


  _____ _______ _______ _____        _____  _____   _____ 
 |  __ \__   __|__   __|  __ \      |  __ \|  __ \ / ____|
 | |__) | | |     | |  | |__) |_____| |__) | |__) | |     
 |  _  /  | |     | |  |  _  /______|  _  /|  ___/| |     
 | | \ \  | |     | |  | | \ \      | | \ \| |    | |____ 
 |_|  \_\ |_|     |_|  |_|  \_\     |_|  \_\_|     \_____|
                                                          
                                                          

## Build

### Build RTTR 
2. cd rttr
3. mkdir build && cd mkdir
4. cmake -DCMAKE_INSTALL_PREFIX:PATH=../install -G "Visual Studio 15 2017 Win64" ..
5. cmake --build . --target install

### Build JsonRpc++
1. cd jsonrpcpp
3. mkdir build && cd mkdir
2. cmake -G "Visual Studio 15 2017 Win64" ..
5. cmake --build . --target install

