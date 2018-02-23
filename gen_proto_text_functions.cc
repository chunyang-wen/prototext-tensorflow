/* Copyright 2016 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <iostream>
#include <set>
#include <stdio.h>
#include <string>

#include "protobuf.h"
#include "types.h"
#include "gen_proto_text_functions_lib.h"

namespace prototext {

const char* kProtobufInt64Typename = "::google::protobuf::int64";
const char* kProtobufUint64Typename = "::google::protobuf::uint64";


static const char kTensorFlowHeaderPrefix[] = "";

static const char kPlaceholderFile[] =
    "placeholder.txt";

bool IsPlaceholderFile(const char* s) {
  std::string ph(kPlaceholderFile);
  std::string str(s);
  return str.size() >= strlen(kPlaceholderFile) &&
         ph == str.substr(str.size() - ph.size());
}

// Main program to take input protos and write output pb_text source files that
// contain generated proto text input and output functions.
//
// Main expects:
// - First argument is output path
// - Second argument is the relative path of the protos to the root. E.g.,
//   for protos built by a rule in tensorflow/core, this will be
//   tensorflow/core.
// - Then any number of source proto file names, plus one source name must be
//   placeholder.txt from this gen tool's package.  placeholder.txt is
//   ignored for proto resolution, but is used to determine the root at which
//   the build tool has placed the source proto files.
//
// Note that this code doesn't use tensorflow's command line parsing, because of
// circular dependencies between libraries if that were done.
//
// This is meant to be invoked by a genrule. See BUILD for more information.
int MainImpl(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Pass output path, relative path, and at least proto file\n";
        return -1;
    }

    const std::string output_root = argv[1];
    const std::string output_relative_path = kTensorFlowHeaderPrefix + std::string(argv[2]);

    std::string src_relative_path;
    bool has_placeholder = false;
    for (int i = 3; i < argc; ++i) {
        if (IsPlaceholderFile(argv[i])) {
            const std::string s(argv[i]);
            src_relative_path = s.substr(0, s.size() - strlen(kPlaceholderFile));
            has_placeholder = true;
        }
    }
    if (!has_placeholder) {
        std::cerr << kPlaceholderFile << " must be passed";
        return -1;
    }

    google::protobuf::compiler::DiskSourceTree source_tree;

    source_tree.MapPath("", src_relative_path.empty() ? "." : src_relative_path);
    google::protobuf::compiler::Importer importer(&source_tree,
            NULL);

    for (int i = 3; i < argc; i++) {
        if (IsPlaceholderFile(argv[i])) continue;
        const std::string proto_path = std::string(argv[i]).substr(src_relative_path.size());

        const google::protobuf::FileDescriptor* fd =
            importer.Import(proto_path);

        const int index = proto_path.find_last_of(".");
        std::string proto_path_no_suffix = proto_path.substr(0, index);

        proto_path_no_suffix =
            proto_path_no_suffix.substr(output_relative_path.size());

        const auto code =
            prototext::GetProtoTextFunctionCode(*fd, kTensorFlowHeaderPrefix);

        // Three passes, one for each output file.
        for (int pass = 0; pass < 3; ++pass) {
            std::string suffix;
            std::string data;
            if (pass == 0) {
                suffix = ".pb_text.h";
                data = code.header;
            } else if (pass == 1) {
                suffix = ".pb_text-impl.h";
                data = code.header_impl;
            } else {
                suffix = ".pb_text.cc";
                data = code.cc;
            }

            const std::string path = output_root + "/" + proto_path_no_suffix + suffix;
            FILE* f = fopen(path.c_str(), "w");
            if (fwrite(data.c_str(), 1, data.size(), f) != data.size()) {
                return -1;
            }
            if (fclose(f) != 0) {
                return -1;
            }
        }
    }
    return 0;
}

}

int main(int argc, char** argv) { return prototext::MainImpl(argc, argv); }
