PROJECT(proto_text)

SET(SRC gen_proto_text_functions_lib.cc gen_proto_text_functions.cc)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# For protobuf
INCLUDE_DIRECTORIES(/Users/chengfu/opensource/protobuf/output/include)
LINK_DIRECTORIES(/Users/chengfu/opensource/protobuf/output/lib)
ADD_EXECUTABLE(proto_text ${SRC})
TARGET_LINK_LIBRARIES(proto_text protobuf)
