/*
The MIT License (MIT)
Copyright (c) 2021 Lancaster University.
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
#include "stdafx.h"
#include "VSGlue.h"

#include "CodalFS.h"
#include "ErrorNo.h"
#include "CodalDmesg.h"
#include "CodalDevice.h"

using namespace codal;

CodalFS* CodalFS::defaultFileSystem = NULL;

const uint8_t CodalFS::header[2048] = {0x3c, 0x21, 0x64, 0x6f, 0x63, 0x74, 0x79, 0x70, 0x65, 0x20, 0x68, 0x74, 0x6d, 0x6c, 0x3e, 0x3c, 0x68, 0x74, 0x6d, 0x6c, 0x20, 0x6c, 0x61, 0x6e, 0x67,
                                                      0x3d, 0x65, 0x6e, 0x3e, 0x3c, 0x6d, 0x65, 0x74, 0x61, 0x20, 0x63, 0x68, 0x61, 0x72, 0x73, 0x65, 0x74, 0x3d, 0x55, 0x54, 0x46, 0x2d, 0x38, 0x3e, 0x3c,
                                                      0x74, 0x69, 0x74, 0x6c, 0x65, 0x3e, 0x6d, 0x69, 0x63, 0x72, 0x6f, 0x3a, 0x66, 0x6c, 0x61, 0x73, 0x68, 0x3c, 0x2f, 0x74, 0x69, 0x74, 0x6c, 0x65, 0x3e,
                                                      0x3c, 0x73, 0x74, 0x79, 0x6c, 0x65, 0x3e, 0x62, 0x6f, 0x64, 0x79, 0x7b, 0x66, 0x6f, 0x6e, 0x74, 0x2d, 0x66, 0x61, 0x6d, 0x69, 0x6c, 0x79, 0x3a, 0x73,
                                                      0x61, 0x6e, 0x73, 0x2d, 0x73, 0x65, 0x72, 0x69, 0x66, 0x7d, 0x23, 0x7a, 0x7b, 0x64, 0x69, 0x73, 0x70, 0x6c, 0x61, 0x79, 0x3a, 0x6e, 0x6f, 0x6e, 0x65,
                                                      0x7d, 0x3c, 0x2f, 0x73, 0x74, 0x79, 0x6c, 0x65, 0x3e, 0x3c, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x3e, 0x63, 0x6f, 0x6e, 0x73, 0x74, 0x20, 0x75, 0x3d,
                                                      0x38, 0x31, 0x39, 0x32, 0x3b, 0x63, 0x6f, 0x6e, 0x73, 0x74, 0x20, 0x70, 0x3d, 0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x3b, 0x77, 0x69, 0x6e,
                                                      0x64, 0x6f, 0x77, 0x2e, 0x6f, 0x6e, 0x6c, 0x6f, 0x61, 0x64, 0x3d, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x28, 0x29, 0x7b, 0x6c, 0x65, 0x74,
                                                      0x20, 0x65, 0x3d, 0x70, 0x2e, 0x67, 0x65, 0x74, 0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x22, 0x7a, 0x22, 0x29, 0x2c,
                                                      0x63, 0x3d, 0x65, 0x2e, 0x69, 0x6e, 0x6e, 0x65, 0x72, 0x48, 0x54, 0x4d, 0x4c, 0x2c, 0x74, 0x3d, 0x28, 0x65, 0x2e, 0x72, 0x65, 0x6d, 0x6f, 0x76, 0x65,
                                                      0x28, 0x29, 0x2c, 0x30, 0x29, 0x2c, 0x6f, 0x3d, 0x35, 0x31, 0x32, 0x2c, 0x6e, 0x3d, 0x6e, 0x65, 0x77, 0x20, 0x44, 0x61, 0x74, 0x61, 0x56, 0x69, 0x65,
                                                      0x77, 0x28, 0x6e, 0x65, 0x77, 0x20, 0x55, 0x69, 0x6e, 0x74, 0x38, 0x41, 0x72, 0x72, 0x61, 0x79, 0x28, 0x63, 0x2e, 0x6d, 0x61, 0x74, 0x63, 0x68, 0x28,
                                                      0x2f, 0x28, 0x5b, 0x30, 0x2d, 0x39, 0x5d, 0x7c, 0x5b, 0x61, 0x2d, 0x66,
                                                      0x5d, 0x29, 0x7b, 0x32, 0x7d, 0x2f, 0x67, 0x69, 0x29, 0x2e, 0x6d, 0x61, 0x70, 0x28, 0x65, 0x3d, 0x3e, 0x70, 0x61, 0x72, 0x73, 0x65, 0x49, 0x6e, 0x74,
                                                      0x28, 0x65, 0x2c, 0x31, 0x36, 0x29, 0x29, 0x29, 0x2e, 0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0x29, 0x2c, 0x69, 0x3d, 0x65, 0x3d, 0x3e, 0x6e, 0x2e, 0x67,
                                                      0x65, 0x74, 0x55, 0x69, 0x6e, 0x74, 0x31, 0x36, 0x28, 0x74, 0x2b, 0x32, 0x2a, 0x65, 0x2c, 0x21, 0x30, 0x29, 0x2c, 0x67, 0x3d, 0x65, 0x3d, 0x3e, 0x6e,
                                                      0x65, 0x77, 0x20, 0x44, 0x61, 0x74, 0x61, 0x56, 0x69, 0x65, 0x77, 0x28, 0x6e, 0x2e, 0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0x2c, 0x74, 0x2b, 0x65, 0x2a,
                                                      0x6f, 0x2c, 0x6f, 0x29, 0x3b, 0x76, 0x61, 0x72, 0x20, 0x72, 0x3d, 0x69, 0x28, 0x30, 0x29, 0x3b, 0x66, 0x6f, 0x72, 0x28, 0x6c, 0x65, 0x74, 0x20, 0x65,
                                                      0x3d, 0x30, 0x3b, 0x65, 0x3c, 0x72, 0x3b, 0x65, 0x2b, 0x2b, 0x29, 0x69, 0x66, 0x28, 0x69, 0x28, 0x65, 0x29, 0x21, 0x3d, 0x3d, 0x72, 0x29, 0x74, 0x68,
                                                      0x72, 0x6f, 0x77, 0x20, 0x45, 0x72, 0x72, 0x6f, 0x72, 0x28, 0x22, 0x49, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x21, 0x22, 0x29, 0x3b, 0x6c, 0x65, 0x74,
                                                      0x20, 0x64, 0x3d, 0x28, 0x74, 0x2c, 0x6e, 0x2c, 0x72, 0x3d, 0x30, 0x29, 0x3d, 0x3e, 0x7b, 0x69, 0x66, 0x28, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x30, 0x3d,
                                                      0x3d, 0x3d, 0x6e, 0x29, 0x7b, 0x66, 0x6f, 0x72, 0x28, 0x6c, 0x65, 0x74, 0x20, 0x65, 0x3d, 0x72, 0x3b, 0x65, 0x3c, 0x74, 0x2e, 0x62, 0x79, 0x74, 0x65,
                                                      0x4c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x3b, 0x65, 0x2b, 0x2b, 0x29, 0x69, 0x66, 0x28, 0x30, 0x3d, 0x3d, 0x3d, 0x74, 0x2e, 0x67, 0x65, 0x74, 0x55, 0x69,
                                                      0x6e, 0x74, 0x38, 0x28, 0x65, 0x29, 0x29, 0x7b, 0x6e, 0x3d, 0x65, 0x2d, 0x74, 0x68, 0x69, 0x73, 0x2e, 0x72, 0x65, 0x61, 0x64, 0x49, 0x6e, 0x64, 0x65,
                                                      0x78, 0x3b, 0x62, 0x72, 0x65, 0x61, 0x6b, 0x7d, 0x69, 0x66, 0x28, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x30, 0x3d, 0x3d, 0x3d, 0x6e, 0x29, 0x74, 0x68, 0x72,
                                                      0x6f, 0x77, 0x20, 0x45, 0x72, 0x72, 0x6f, 0x72, 0x28, 0x22, 0x43, 0x6f,
                                                      0x75, 0x6c, 0x64, 0x6e, 0x27, 0x74, 0x20, 0x66, 0x69, 0x6e, 0x64, 0x20, 0x6c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x20, 0x6f, 0x66, 0x20, 0x73, 0x74, 0x72,
                                                      0x69, 0x6e, 0x67, 0x22, 0x29, 0x7d, 0x72, 0x2b, 0x3d, 0x74, 0x2e, 0x62, 0x79, 0x74, 0x65, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74, 0x3b, 0x6c, 0x65, 0x74,
                                                      0x20, 0x65, 0x3d, 0x6e, 0x65, 0x77, 0x20, 0x54, 0x65, 0x78, 0x74, 0x44, 0x65, 0x63, 0x6f, 0x64, 0x65, 0x72, 0x2c, 0x6c, 0x3d, 0x65, 0x2e, 0x64, 0x65,
                                                      0x63, 0x6f, 0x64, 0x65, 0x28, 0x74, 0x2e, 0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0x2e, 0x73, 0x6c, 0x69, 0x63, 0x65, 0x28, 0x72, 0x2c, 0x72, 0x2b, 0x6e,
                                                      0x29, 0x29, 0x3b, 0x72, 0x3d, 0x6c, 0x2e, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x4f, 0x66, 0x28, 0x22, 0x5c, 0x30, 0x22, 0x29, 0x3b, 0x72, 0x65, 0x74, 0x75,
                                                      0x72, 0x6e, 0x20, 0x6c, 0x3d, 0x72, 0x3f, 0x6c, 0x2e, 0x73, 0x75, 0x62, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x28, 0x30, 0x2c, 0x72, 0x29, 0x3a, 0x6c,
                                                      0x7d, 0x2c, 0x61, 0x3d, 0x28, 0x65, 0x2c, 0x74, 0x3d, 0x30, 0x29, 0x3d, 0x3e, 0x7b, 0x6c, 0x65, 0x74, 0x20, 0x6e, 0x3d, 0x67, 0x28, 0x65, 0x29, 0x3b,
                                                      0x65, 0x3d, 0x74, 0x3b, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x7b, 0x6e, 0x61, 0x6d, 0x65, 0x3a, 0x64, 0x28, 0x6e, 0x2c, 0x31, 0x36, 0x2c, 0x65, 0x29,
                                                      0x2c, 0x66, 0x69, 0x72, 0x73, 0x74, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x3a, 0x6e, 0x2e, 0x67, 0x65, 0x74, 0x55, 0x69, 0x6e, 0x74, 0x31, 0x36, 0x28, 0x65,
                                                      0x2b, 0x3d, 0x31, 0x36, 0x2c, 0x21, 0x30, 0x29, 0x2c, 0x66, 0x6c, 0x61, 0x67, 0x73, 0x3a, 0x6e, 0x2e, 0x67, 0x65, 0x74, 0x55, 0x69, 0x6e, 0x74, 0x31,
                                                      0x36, 0x28, 0x65, 0x2b, 0x3d, 0x32, 0x2c, 0x21, 0x30, 0x29, 0x2c, 0x6c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x3a, 0x6e, 0x2e, 0x67, 0x65, 0x74, 0x55, 0x69,
                                                      0x6e, 0x74, 0x33, 0x32, 0x28, 0x65, 0x2b, 0x3d, 0x32, 0x2c, 0x21, 0x30, 0x29, 0x7d, 0x7d, 0x2c, 0x6d, 0x3d, 0x65, 0x3d, 0x3e, 0x7b, 0x6c, 0x65, 0x74,
                                                      0x20, 0x74, 0x3d, 0x5b, 0x5d, 0x2c, 0x6e, 0x3d, 0x65, 0x2e, 0x66, 0x69,
                                                      0x72, 0x73, 0x74, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x2c, 0x72, 0x3d, 0x30, 0x3b, 0x66, 0x6f, 0x72, 0x28, 0x3b, 0x3b, 0x29, 0x7b, 0x69, 0x66, 0x28, 0x72,
                                                      0x2b, 0x32, 0x34, 0x3e, 0x6f, 0x29, 0x7b, 0x69, 0x66, 0x28, 0x36, 0x31, 0x34, 0x33, 0x39, 0x3d, 0x3d, 0x3d, 0x28, 0x6e, 0x3d, 0x69, 0x28, 0x6e, 0x29,
                                                      0x29, 0x29, 0x62, 0x72, 0x65, 0x61, 0x6b, 0x3b, 0x72, 0x3d, 0x30, 0x7d, 0x76, 0x61, 0x72, 0x20, 0x6c, 0x3d, 0x61, 0x28, 0x6e, 0x2c, 0x72, 0x29, 0x3b,
                                                      0x72, 0x2b, 0x3d, 0x32, 0x34, 0x2c, 0x6c, 0x2e, 0x66, 0x69, 0x72, 0x73, 0x74, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x21, 0x3d, 0x3d, 0x65, 0x2e, 0x66, 0x69,
                                                      0x72, 0x73, 0x74, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x26, 0x26, 0x33, 0x32, 0x37, 0x36, 0x38, 0x21, 0x3d, 0x28, 0x33, 0x32, 0x37, 0x36, 0x38, 0x26, 0x6c,
                                                      0x2e, 0x66, 0x6c, 0x61, 0x67, 0x73, 0x29, 0x26, 0x26, 0x31, 0x36, 0x33, 0x38, 0x34, 0x3d, 0x3d, 0x28, 0x31, 0x36, 0x33, 0x38, 0x34, 0x26, 0x6c, 0x2e,
                                                      0x66, 0x6c, 0x61, 0x67, 0x73, 0x29, 0x26, 0x26, 0x28, 0x28, 0x6c, 0x2e, 0x66, 0x6c, 0x61, 0x67, 0x73, 0x26, 0x75, 0x29, 0x3d, 0x3d, 0x75, 0x3f, 0x63,
                                                      0x6f, 0x6e, 0x73, 0x6f, 0x6c, 0x65, 0x2e, 0x6c, 0x6f, 0x67, 0x28, 0x6c, 0x2c, 0x22, 0x20, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79, 0x2c,
                                                      0x20, 0x66, 0x69, 0x72, 0x73, 0x74, 0x20, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x3a, 0x20, 0x22, 0x2b, 0x6c, 0x2e, 0x66, 0x69, 0x72, 0x73, 0x74, 0x42, 0x6c,
                                                      0x6f, 0x63, 0x6b, 0x29, 0x3a, 0x63, 0x6f, 0x6e, 0x73, 0x6f, 0x6c, 0x65, 0x2e, 0x6c, 0x6f, 0x67, 0x28, 0x6c, 0x29, 0x2c, 0x74, 0x2e, 0x70, 0x75, 0x73,
                                                      0x68, 0x28, 0x6c, 0x29, 0x29, 0x7d, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20, 0x74, 0x7d, 0x3b, 0x76, 0x61, 0x72, 0x20, 0x6c, 0x3d, 0x70, 0x2e, 0x67,
                                                      0x65, 0x74, 0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x22, 0x66, 0x22, 0x29, 0x3b, 0x6c, 0x65, 0x74, 0x20, 0x66, 0x3d,
                                                      0x28, 0x65, 0x2c, 0x72, 0x29, 0x3d, 0x3e, 0x7b, 0x6c, 0x65, 0x74, 0x20,
                                                      0x6c, 0x3d, 0x7b, 0x7d, 0x3b, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20, 0x6d, 0x28, 0x65, 0x29, 0x2e, 0x66, 0x6f, 0x72, 0x45, 0x61, 0x63, 0x68, 0x28,
                                                      0x74, 0x3d, 0x3e, 0x7b, 0x6c, 0x65, 0x74, 0x20, 0x6e, 0x3d, 0x70, 0x2e, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74,
                                                      0x28, 0x22, 0x6c, 0x69, 0x22, 0x29, 0x3b, 0x69, 0x66, 0x28, 0x28, 0x74, 0x2e, 0x66, 0x6c, 0x61, 0x67, 0x73, 0x26, 0x75, 0x29, 0x3d, 0x3d, 0x75, 0x29,
                                                      0x7b, 0x6c, 0x65, 0x74, 0x20, 0x65, 0x3d, 0x70, 0x2e, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x28, 0x22, 0x75,
                                                      0x6c, 0x22, 0x29, 0x3b, 0x65, 0x2e, 0x61, 0x70, 0x70, 0x65, 0x6e, 0x64, 0x28, 0x74, 0x2e, 0x6e, 0x61, 0x6d, 0x65, 0x29, 0x2c, 0x6c, 0x5b, 0x74, 0x2e,
                                                      0x6e, 0x61, 0x6d, 0x65, 0x5d, 0x3d, 0x66, 0x28, 0x74, 0x2c, 0x65, 0x29, 0x2c, 0x30, 0x3d, 0x3d, 0x3d, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x2e, 0x6b,
                                                      0x65, 0x79, 0x73, 0x28, 0x6c, 0x5b, 0x74, 0x2e, 0x6e, 0x61, 0x6d, 0x65, 0x5d, 0x29, 0x2e, 0x6c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x26, 0x26, 0x65, 0x2e,
                                                      0x61, 0x70, 0x70, 0x65, 0x6e, 0x64, 0x28, 0x22, 0x20, 0x28, 0x65, 0x6d, 0x70, 0x74, 0x79, 0x29, 0x22, 0x29, 0x2c, 0x6e, 0x2e, 0x61, 0x70, 0x70, 0x65,
                                                      0x6e, 0x64, 0x28, 0x65, 0x29, 0x7d, 0x65, 0x6c, 0x73, 0x65, 0x20, 0x6c, 0x5b, 0x74, 0x2e, 0x6e, 0x61, 0x6d, 0x65, 0x5d, 0x3d, 0x74, 0x2c, 0x6e, 0x2e,
                                                      0x69, 0x6e, 0x6e, 0x65, 0x72, 0x54, 0x65, 0x78, 0x74, 0x3d, 0x74, 0x2e, 0x6e, 0x61, 0x6d, 0x65, 0x3b, 0x72, 0x2e, 0x61, 0x70, 0x70, 0x65, 0x6e, 0x64,
                                                      0x28, 0x6e, 0x29, 0x7d, 0x29, 0x2c, 0x6c, 0x7d, 0x3b, 0x76, 0x61, 0x72, 0x20, 0x73, 0x3d, 0x61, 0x28, 0x72, 0x29, 0x2c, 0x6c, 0x3d, 0x66, 0x28, 0x73,
                                                      0x2c, 0x6c, 0x29, 0x3b, 0x63, 0x6f, 0x6e, 0x73, 0x6f, 0x6c, 0x65, 0x2e, 0x6c, 0x6f, 0x67, 0x28, 0x73, 0x2c, 0x6c, 0x29, 0x7d, 0x3c, 0x2f, 0x73, 0x63,
                                                      0x72, 0x69, 0x70, 0x74, 0x3e, 0x3c, 0x68, 0x33, 0x3e, 0x6d, 0x69, 0x63,
                                                      0x72, 0x6f, 0x3a, 0x62, 0x69, 0x74, 0x20, 0x66, 0x69, 0x6c, 0x65, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x3c, 0x2f, 0x68, 0x33, 0x3e, 0x3c, 0x64, 0x69,
                                                      0x76, 0x3e, 0x48, 0x65, 0x72, 0x65, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x73, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x20, 0x76, 0x69, 0x65, 0x77, 0x20, 0x6f,
                                                      0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x69, 0x6c, 0x65, 0x73, 0x20, 0x6f, 0x6e, 0x20, 0x79, 0x6f, 0x75, 0x72, 0x20, 0x6d, 0x69, 0x63, 0x72, 0x6f,
                                                      0x3a, 0x62, 0x69, 0x74, 0x2e, 0x20, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x68, 0x65, 0x20, 0x69, 0x6e, 0x74, 0x65,
                                                      0x72, 0x6e, 0x65, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x76, 0x69, 0x65, 0x77, 0x20, 0x74, 0x68, 0x65, 0x20, 0x61, 0x64, 0x76, 0x61, 0x6e, 0x63, 0x65, 0x64,
                                                      0x20, 0x62, 0x72, 0x6f, 0x77, 0x73, 0x65, 0x72, 0x21, 0x3c, 0x2f, 0x64, 0x69, 0x76, 0x3e, 0x3c, 0x75, 0x6c, 0x20, 0x69, 0x64, 0x3d, 0x66, 0x3e, 0x3c,
                                                      0x2f, 0x75, 0x6c, 0x3e, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                                      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x64, 0x69, 0x76, 0x20, 0x69, 0x64, 0x3d, 0x7a,
                                                      0x3e};

/**
 * Retrieves the value of the file table at the given index, handling any necessary caching of
 * blocks/pages.
 * @param index the file table index to read.
 * @return the value of the fileTable at the given index.
 */
uint16_t CodalFS::fileTableRead(uint16_t index)
{
    uint16_t value;
    cache.read(flash.getFlashStart() + CODALFS_OFFSET + index * 2, &value, 2);
    return value;
}

/**
 * Allocate a free logical block.
 * A recently unused block is chosen to reduce the wear on the physical hardware.
 * @return a valid, unused block address on success, or zero if no space is available.
 */
uint16_t CodalFS::getFreeBlock()
{
    // Walk the File Table and allocate the first free block - starting immediately after the last block allocated,
    // and wrapping around the filesystem space if we reach the end.
    uint16_t block;
    uint16_t deletedBlock = 0;

    for (block = (lastBlockAllocated + 1) % fileSystemSize; block != lastBlockAllocated; block++)
    {
        uint16_t b = fileTableRead(block);
        if (b == CODALFS_UNUSED)
        {
            lastBlockAllocated = block;

            // If this is the first block used in a page that it marked entirely as free, then ensure the physical page is erased before use.
            int blocksPerPage = flash.getPageSize() / blockSize;
            uint16_t firstBlock = block - block % blocksPerPage;
            bool needErase = true;

            for (uint16_t b = firstBlock; b < firstBlock + blocksPerPage; b++)
                if (fileTableRead(b) != CODALFS_UNUSED)
                    needErase = false;

            if (needErase)
                flash.erase(getPage(block));

            return block;
        }

        if (b == CODALFS_DELETED)
            deletedBlock = block;
    }

    // if no UNUSED blocks are available, try to recycle one marked as DELETED.
    block = deletedBlock;

    // If no blocks are available - either UNUSED or marked as DELETED, then we're out of space and
    // there's nothing we can do.
    if (block)
    {
        // recycle the FileTable, such that we can mark all previously deleted blocks as re-usable.
        // Better to do this in bulk, rather than on a block by block basis to improve efficiency.
        recycleFileTable();

        // Record the block we just allocated, so we can round-robin around blocks for load
        // balancing.
        lastBlockAllocated = block;
    }

    return block;
}

/**
 * Constructor. Creates an instance of a CodalFS.
 */
CodalFS::CodalFS(NVMController &nvm, uint32_t blockSize)
    : flash(nvm), cache(flash, blockSize), blockSize(blockSize)
{
    // Initialise status flags to default value
    this->status = 0;

    // Attempt to load an existing filesystem, if it exisits
    init();

    //TODO HACK
    flash.write(flash.getFlashStart(), (uint32_t *)header, sizeof(header)/4);

    // If this is the first FileSystem created, so it as the default.
    if (CodalFS::defaultFileSystem == NULL)
        CodalFS::defaultFileSystem = this;

    //uint8_t *tmp = (uint8_t *)malloc(blockSize);
//
    //DMESGF("START--");
//
    //for (uint32_t i = 0; i < nvm.getFlashSize(); i += blockSize) {
    //    memset(tmp, 0, blockSize);
    //    nvm.read((uint32_t*) tmp, i, blockSize / 4);
//
    //    DMESGR((char*) tmp, false, blockSize);
    //}
//
    //DMESGF("--END");
}

/**
 * Initialize the flash storage system
 *
 * The file system is located dynamically, based on where the program code
 * and code data finishes. This avoids having to allocate a fixed flash
 * region for builds even without CodalFS.
 *
 * This method checks if the file system already exists, and loads it it.
 * If not, it will determines the optimal size of the file system, if necessary, and format the
 * space
 *
 * @return DEVICE_OK on success, or an error code.
 */
int CodalFS::init()
{
    // Protect against accidental re-initialisation
    if (status & CODALFS_STATUS_INITIALISED)
        return DEVICE_NOT_SUPPORTED;

    // Validate parameters
    if (flash.getFlashSize() <= 0)
        return DEVICE_INVALID_PARAMETER;

    // Zero initialise default parameters (mbed/ARMCC does not permit this is the class definition).
    lastBlockAllocated = 0;
    rootDirectory = NULL;
    openFiles = NULL;

    // First, try to load an existing file system at this location.
    DMESG("CODALFS: Loading filesystem...\n");
    if (load() != DEVICE_OK)
    {
        DMESG("CODALFS: No filesystem found. formatting...\n");
        // No file system was found, so format a fresh one.
        // Bring up a freshly formatted file system here.
        fileSystemSize = flash.getFlashSize() / blockSize;
        fileSystemTableSize = calculateFileTableSize();

        format();
    }

    // indicate that we have a valid FileSystem
    status = CODALFS_STATUS_INITIALISED;
    return DEVICE_OK;
}

/**
 * Attempts to detect and load an existing file file system.
 *
 * @return DEVICE_OK on success, or DEVICE_NO_DATA if the file system could not be found.
 */
int CodalFS::load()
{
    uint16_t rootOffset = fileTableRead(0);

    // A valid MBFS has the first 'N' blocks set to the value 'N' followed by a valid root directory
    // block with magic signature.
    for (int i = 0; i < rootOffset; i++)
    {
        uint16_t b = fileTableRead(i);
        if (b >= (flash.getFlashSize() / blockSize) || b != rootOffset)
        {
            DMESG("CODALFS: LOAD ABORTED FILE TABLE CORRUPTED\n");
            return DEVICE_NO_DATA;
        }
    }

    // Check for a valid signature at the start of the root directory
    DirectoryEntry *root = (DirectoryEntry *)getBlock(rootOffset);
    if (strcmp(root->file_name, CODALFS_MAGIC) != 0)
    {
        DMESG("CODALFS: LOAD ABORTED INVALID MAGIC: %s\n", root->file_name);
        return DEVICE_NO_DATA;
    }

    // Cache the root directory. Pin the relevant block in the cache to ensure it is never paged out...
    cache.pin(addressOfBlock(rootOffset));

    rootDirectory = root;
    fileSystemSize = root->length & ~CODALFS_DIRECTORY_LENGTH;
    fileSystemTableSize = calculateFileTableSize();

    return DEVICE_OK;
}

/**
 * Initialises a new file system. Assumes all pages are already erased.
 *
 * @return DEVICE_OK on success, or an error code..
 */
int CodalFS::format()
{
    uint16_t value = fileSystemTableSize;

    DMESG("PAGE SIZE: %d", flash.getPageSize());

    // Ensure physical pages needed for the FAT table and root directory are erased before use
    cache.clear();
    for (uint16_t b = 0; b < fileSystemTableSize + 1; b += (uint16_t)(flash.getPageSize() / blockSize)) //TODO: this will loop forever if block size > page size
        flash.erase(getPage(b));

    // Mark the FileTable blocks themselves as used.
    for (uint16_t block = 0; block < fileSystemTableSize; block++)
        cache.write(flash.getFlashStart() + CODALFS_OFFSET + block * 2, &value, 2);

    // Create a root directory
    value = CODALFS_EOF;
    cache.write(flash.getFlashStart() + CODALFS_OFFSET + fileSystemTableSize * 2, &value, 2);

    // Store a MAGIC value in the first root directory entry.
    // This will let us identify a valid File System later.
    DirectoryEntry magic;

    strcpy(magic.file_name, CODALFS_MAGIC);
    magic.first_block = fileSystemTableSize;
    magic.flags = CODALFS_DIRECTORY_ENTRY_VALID;
    magic.length = CODALFS_DIRECTORY_LENGTH | fileSystemSize;

    // Cache the root directory entry for later use.
    rootDirectory = (DirectoryEntry *)getBlock(fileSystemTableSize);
    cache.write(addressOfBlock(fileSystemTableSize), &magic, sizeof(DirectoryEntry));
	cache.pin(addressOfBlock(fileSystemTableSize));

    return DEVICE_OK;
}

/**
 * Retrieve the DirectoryEntry for the given filename.
 *
 * @param filename A fully or partially qualified filename.
 * @param directory The directory to search. If omitted, the root directory will be used.
 * @return the logical address of the DirectoryEntry for the given file, or NULL if no entry is
 * found.
 */
uint32_t CodalFS::getDirectoryEntry(char const *filename, const DirectoryEntry *directory)
{
    char const *file;
    uint16_t block;
    DirectoryEntry *dirent;
    uint32_t direntAddress;

    // Determine the filename from the (potentially) fully qualified filename.
    file = filename + strlen(filename);
    while (file >= filename && *file != CODALFS_DIRECTORY_SEPARATOR)
        file--;
    file++;

    // Obtain a handle on the directory to search.
    if (directory == NULL)
        directory = rootDirectory;

    block = directory->first_block;

    direntAddress = block * blockSize;
    dirent = (DirectoryEntry *)getCachedData(direntAddress);

    // Iterate through the directory entries until we find our file, or run out of space.
    while (1)
    {
        if (direntAddress + sizeof(DirectoryEntry) > block * blockSize + (uint32_t)blockSize)
        {
            block = getNextFileBlock(block);
            if (block == CODALFS_EOF)
                return CODALFS_INVALID_ADDRESS;

            direntAddress = block * blockSize;
            dirent = (DirectoryEntry *)getCachedData(direntAddress);
        }

        // Check for a valid match
        if (dirent->flags & CODALFS_DIRECTORY_ENTRY_VALID && strcmp(dirent->file_name, file) == 0)
            return direntAddress;

        // Move onto the next entry.
        dirent++;
        direntAddress += sizeof(DirectoryEntry);
    }

    return CODALFS_INVALID_ADDRESS;
}

/**
 * Determine the number of logical blocks required to hold the file table.
 *
 * @return The number of logical blocks required to hold the file table.
 */
uint16_t CodalFS::calculateFileTableSize()
{
    uint16_t size = (uint16_t)((fileSystemSize * 2) / blockSize);
    if ((fileSystemSize * 2) % blockSize)
        size++;

    return size;
}

/**
 * Retrieve a memory pointer for the start of the physical memory page containing the given block.
 *
 * @param block A valid block number.
 * @return the logical address of the physical page in FLASH memory holding the given block.
 */
uint32_t CodalFS::getPage(uint16_t block)
{
    uint32_t address = addressOfBlock(block);
    return (address - address % flash.getPageSize());
}

/**
 * Retrieve a memory pointer for the start of the given block, in cache RAM.
 *
 * @param block A valid block number.
 * @return A pointer to the FLASH memory associated with the given block.
 */
uint32_t *CodalFS::getBlock(uint16_t block)
{
    CacheEntry *c = getCachedBlock(block);
    if (c != NULL)
        return (uint32_t *)c->page;

    return NULL;
}

/**
 * Pull the given logical block into cache memory (if possible).
 *
 * @param block A valid block number.
 * @return A pointer to the CacheEntry associated with the given block, or NULL if the block could
 * not be retrieved.
 */
CacheEntry *CodalFS::getCachedBlock(uint16_t block)
{
    return cache.cachePage(addressOfBlock(block));
}

/**
 * Determines the logical memory address associated with the start of the given block
 * @param block A valid block number.
 * @return the logical addresses of the start of the given block.
 */
uint32_t CodalFS::addressOfBlock(uint16_t block)
{
    return flash.getFlashStart() + CODALFS_OFFSET + block * blockSize;
}

/**
 * Retrieve the next block in a chain.
 *
 * @param block A valid block number.
 *
 * @return The block number of the next block in the file.
 */
uint16_t CodalFS::getNextFileBlock(uint16_t block)
{
    return fileTableRead(block);
}

/**
 * Determine the logical block that contains the given address.
 *
 * @param address A valid memory location within the file system space.
 *
 * @return The block number containing the given address.
 */
uint16_t CodalFS::getBlockNumber(uint32_t address)
{
    return (uint16_t)((address - flash.getFlashStart() - CODALFS_OFFSET) / blockSize);
}

/**
 * Update a file table entry to a given value.
 *
 * @param block The block to update.
 * @param value The value to store in the file table.
 * @return DEVICE_OK on success.
 */
int CodalFS::fileTableWrite(uint16_t block, uint16_t value)
{
    cache.write(flash.getFlashStart() + CODALFS_OFFSET + block * 2, &value, 2);
    return DEVICE_OK;
}

/**
 * Retrieve the DirectoryEntry associated with the given file's DIRECTORY (not the file itself).
 *
 * @param filename A fully qualified filename, from the root.
 * @return the logical address of the DirectoryEntry for the given file's directory, or NULL if no
 * entry is found.
 */
uint32_t CodalFS::getDirectoryOf(char const *filename)
{
    DirectoryEntry *directory;
    uint32_t directoryAddress;

    // If no path is provided, return the root directory.
    if (filename == NULL || filename[0] == 0 || (filename[0] == CODALFS_DIRECTORY_SEPARATOR && filename[1] == 0))
        return fileSystemTableSize * blockSize;

    char s[CODALFS_FILENAME_LENGTH + 1];

    uint8_t i = 0;

    directory = rootDirectory;
    directoryAddress = fileSystemTableSize * blockSize;

    while (*filename != '\0')
    {
        if (*filename == CODALFS_DIRECTORY_SEPARATOR)
        {
            s[i] = '\0';

            // Ensure each level of the filename is valid
            if (i == 0 || i > CODALFS_FILENAME_LENGTH + 1)
                return CODALFS_INVALID_ADDRESS;

            // Extract the relevant entry from the directory.
            directoryAddress = getDirectoryEntry(s, directory);
            directory = (DirectoryEntry *)getCachedData(directoryAddress);

            // If file / directory does not exist, then there's nothing more we can do.
            if (!directory)
                return CODALFS_INVALID_ADDRESS;

            i = 0;
        }
        else
            s[i++] = *filename;

        filename++;
    }

    return directoryAddress;
}

void debug_print_dirent(DirectoryEntry *dirent)
{
    DMESG("   filename    : %s", dirent->file_name);
    DMESG("   first block : %d", dirent->first_block);
    DMESG("   flags       : %X", dirent->flags);
    DMESG("   length      : %d", dirent->length);
}

void CodalFS::debug_print_root_dirent()
{
    uint32_t *tmp = (uint32_t *)malloc(blockSize);
    memset(tmp, 0, blockSize);

    flash.read(tmp, 0x800, blockSize / 4);
    DirectoryEntry *tmpDirent = (DirectoryEntry *)tmp;

    DMESG("ROOT DIRENT: ");
    debug_print_dirent(tmpDirent);

    free(tmp);
}

/**
 * Refresh the physical page associated with the given block.
 * Any logical blocks marked for deletion on that page are recycled.
 *
 * @param block the block to recycle.
 * @param type One of CODALFS_BLOCK_TYPE_FILE, CODALFS_BLOCK_TYPE_DIRECTORY, CODALFS_BLOCK_TYPE_FILETABLE.
 * Erases and regenerates the given block, recycling any data marked for deletion.
 * @return DEVICE_OK on success.
 */
int CodalFS::recycleBlock(uint16_t block, int type)
{
    uint32_t page = getPage(block);
    uint32_t *scratch = (uint32_t *)malloc(flash.getPageSize());
    uint8_t *write = (uint8_t *)scratch;
    uint16_t b = getBlockNumber(page);

    DMESG("RECYCLE PAGE: [PAGE: %d]", page);
    memset(scratch, 0xFF, flash.getPageSize());

    for (int i = 0; i < (int)(flash.getPageSize() / blockSize); i++)
    {
        // DMESG("  RECYCLE BLOCK: [BLOCK: %d (%d/%d)]", b, i,  flash.getPageSize() / blockSize);
        bool freeBlock = false;

        // If we have an unused or deleted block, there's nothing to do - allow the block to be recycled.
        if (fileTableRead(b) == CODALFS_DELETED || fileTableRead(b) == CODALFS_UNUSED)
        {
            // DMESG("  FREE");
            freeBlock = true;
        }

        // If we have been asked to recycle a valid directory block, recycle individual entries where possible.
        else if (b == block && type == CODALFS_BLOCK_TYPE_DIRECTORY)
        {
            // DMESG("  DIRECTORY");
            DirectoryEntry *direntIn = (DirectoryEntry *)getBlock(b);
            DirectoryEntry *direntOut = (DirectoryEntry *)write;

            for (uint16_t entry = 0; entry < blockSize / sizeof(DirectoryEntry); entry++)
            {
                if (direntIn->flags & CODALFS_DIRECTORY_ENTRY_VALID)
                {
                    //DMESG("    DIRENT %d/%d [file: %s] VALID", entry+1, blockSize / sizeof(DirectoryEntry), direntIn->file_name);
                    memcpy(direntOut, direntIn, sizeof(DirectoryEntry));
                }
                else
                {
                    //DMESG("    DIRENT %d/%d [file: %s] INVALID", entry+1, blockSize / sizeof(DirectoryEntry), direntIn->file_name);
                }

                direntIn++;
                direntOut++;
            }
        }

        // All blocks before the root directory are the FileTable.
        // Recycle any entries marked as DELETED to UNUSED.
        else if (b < fileSystemTableSize)
        {
            // DMESG("FILE TABLE ENTRY");
            uint16_t *tableIn = (uint16_t *)getBlock(b);
            uint16_t *tableOut = (uint16_t *)write;

            for (uint32_t entry = 0; entry < blockSize / 2; entry++)
            {
                if (*tableIn != CODALFS_DELETED)
                    memcpy(tableOut, tableIn, 2);

                tableIn++;
                tableOut++;
            }
        }

        // Copy all other VALID blocks directly into the scratch page.
        else
        {
            // DMESG("DATA BLOCK - PRESERVING");
            memcpy(write, getBlock(b), blockSize);
        }

        // Free / update any cached resourced related ot the processed block
        if (freeBlock)
        {
            // DMESG("ERASING CACHE [BLOCK %d]", b);
            cache.erase(addressOfBlock(b));
        }
        else
        {
            // DMESG("UPDATING CACHE [BLOCK %d]", b);
            memcpy(getBlock(b), write, blockSize);
        }

        // move on to next block.
        write += blockSize;
        b++;
    }

    // Now refresh the page originally holding the block.
    // DMESG("ERASING PHYSICAL PAGE [PAGE: 0x%X]", page);

    flash.erase(page);
    flash.write(page, scratch, flash.getPageSize() / 4);

    free(scratch);

    return DEVICE_OK;
}

/**
 * Refresh the physical pages associated with the file table.
 * Any logical blocks marked for deletion on those pages are recycled back to UNUSED.
 *
 * @return DEVICE_OK on success.
 */
int CodalFS::recycleFileTable()
{
    bool pageRecycled = false;

    for (uint16_t block = 0; block < fileSystemSize; block++)
    {
        // if we just crossed a page boundary, reset pageRecycled.
        if (block % (flash.getPageSize() / blockSize) == 0)
            pageRecycled = false;

        if (fileTableRead(block) == CODALFS_DELETED && !pageRecycled)
        {
            recycleBlock(block);
            pageRecycled = true;
        }
    }

    // now, recycle the FileSystemTable itself, upcycling entries marked as DELETED to UNUSED as we go.
    for (uint16_t block = 0; block < fileSystemTableSize; block += (uint16_t) (flash.getPageSize() / blockSize))
        recycleBlock(block);

    return DEVICE_OK;
}

void CodalFS::debug_print_directory(const char *directoryName) {
    // If the supplied directory is null, default to the root
    auto dirEntry = directoryName == NULL ? rootDirectory : (DirectoryEntry*) getCachedData(getDirectoryEntry(directoryName));
    debug_print_directory(dirEntry, 0);
}

void CodalFS::debug_print_directory(DirectoryEntry* directory, int levelsDeep)
{
    Directory *dir;
    uint16_t block;
    DirectoryEntry *dirent;
    DirectoryEntry *invalid = NULL;
    uint16_t newBlock;

    // Find the first block corresponding to this directory. This contains an unbounded DirectoryEntry array
    block = directory->first_block;
    dir = (Directory *)getBlock(block);
    dirent = &dir->entry[0];

    // Print out '   ' for each level deep we are
    const auto entryPrefix = new char[levelsDeep * 3 + 1];
    for (int i = 0; i < levelsDeep * 3; i++) {
        entryPrefix[i] = ' ';
    }
    entryPrefix[levelsDeep * 3] = '\0';

    // Iterate through all the entries for this directory
    while (1)
    {
        // Read the directory. Once we've finished a block, we need to read the *next* directory
        // block which will have more entries.

        if ((uint32_t)(dirent + 1) > (uint32_t)dir + blockSize) // If reading the next directory would go over the block boundary, skip right ahead to the next one
        {
            block = getNextFileBlock(block);
            if (block == CODALFS_EOF)
                break;

            dir = (Directory *)getBlock(block);
            dirent = &dir->entry[0];
        }

        // This file is valid, print it out
        if (!(dirent->flags & CODALFS_DIRECTORY_ENTRY_FREE) &&
            (dirent->flags & CODALFS_DIRECTORY_ENTRY_VALID) != 0)
        {
            DMESGN("%s", entryPrefix);

            // This is a directory, scan it recursively
            if ((dirent->flags & CODALFS_DIRECTORY_ENTRY_DIRECTORY) == CODALFS_DIRECTORY_ENTRY_DIRECTORY) {
                DMESGF("%s (directory, first block %d)", dirent->file_name, dirent->first_block);

                if (dirent->first_block == directory->first_block) {
                    // This is the current directory, ignore!
                    continue;
                }

                // We're one level deeper now...
                debug_print_directory(dirent, levelsDeep + 1);
            } else {
                DMESGF("%s", dirent->file_name);
            }
        }

        // Move onto the next entry.
        dirent++;
    }
}

/**
 * Allocate a free DirectoryEntry in the given directory, extending and refreshing the directory
 * block if necessary.
 *
 * @param directory The directory to add a DirectoryEntry to
 * @return The logical address of the new DirectoryEntry for the given file, or 0 if it was not
 * possible to allocated resources.
 */
uint32_t CodalFS::createDirectoryEntry(DirectoryEntry *directory)
{
    Directory *dir;
    uint16_t block;
    DirectoryEntry *dirent;
    DirectoryEntry *empty = NULL;
    DirectoryEntry *invalid = NULL;
    uint16_t invalidBlock;
    uint16_t emptyBlock;
    uint16_t newBlock;

    // Try to find an unused entry in the directory.
    block = directory->first_block;
    dir = (Directory *)getBlock(block);
    dirent = &dir->entry[0];

    // Iterate through the directory entries until we find and unused entry, or run out of space.
    while (1)
    {
        // Scan through each of the blocks in the directory
        if ((uint32_t)(dirent + 1) > (uint32_t)dir + blockSize)
        {
            block = getNextFileBlock(block);
            if (block == CODALFS_EOF)
                break;

            dir = (Directory *)getBlock(block);
            dirent = &dir->entry[0];
        }

        // If we find an empty slot, use that.
        if (dirent->flags & CODALFS_DIRECTORY_ENTRY_FREE)
        {
            empty = dirent;
            emptyBlock = block;
            break;
        }

        // Record the first invalid block we find (used, but then deleted).
        if ((dirent->flags & CODALFS_DIRECTORY_ENTRY_VALID) == 0 && invalid == NULL)
        {
            invalid = dirent;
            invalidBlock = block;
        }

        // Move onto the next entry.
        dirent++;
    }


    // Now choose the best available slot, giving preference to entries that would avoid a FLASH page erase operation.
    dirent = NULL;

    // Ideally, choose an unused entry within an existing block.
    if (empty)
    {
        dirent = empty;
        newBlock = emptyBlock;
    }

    // If not possible, try to re-use a second-hand block that has been freed. This will result in an erase operation of the block,
    // but will not consume any more resources.
    else if (invalid)
    {
        dirent = invalid;
        newBlock = invalidBlock;
        recycleBlock(invalidBlock, CODALFS_BLOCK_TYPE_DIRECTORY);
    }

    // If nothing is available, extend the directory with a new block.
    else
    {
        // Allocate a new logical block
        newBlock = getFreeBlock();
        if (newBlock == 0)
            return CODALFS_INVALID_ADDRESS;

        // Append this to the directory
        uint16_t lastBlock = directory->first_block;
        while (getNextFileBlock(lastBlock) != CODALFS_EOF)
            lastBlock = getNextFileBlock(lastBlock);

        // Append the block.
        fileTableWrite(lastBlock, newBlock);
        fileTableWrite(newBlock, CODALFS_EOF);

        dirent = (DirectoryEntry *)getBlock(newBlock);
    }

    return addressOfBlock(newBlock) + ((uint8_t *)dirent - (uint8_t *)getBlock(newBlock));
}

/**
 * Create a new DirectoryEntry with the given filename and flags.
 *
 * @param filename A fully or partially qualified filename.
 * @param directory The directory in which to create the entry
 * @param isDirectory true if the entry being created is itself a directory
 *
 * @return The logical address of the new DirectoryEntry for the given file, or NULL if it was not possible to allocated resources.
 */
uint32_t CodalFS::createFile(char const *filename, DirectoryEntry *directory, bool isDirectory)
{
    char const *file;
    uint32_t dirent;

    // Determine the filename from the (potentially) fully qualified filename.
    file = filename + strlen(filename);
    while (file >= filename && *file != CODALFS_DIRECTORY_SEPARATOR)
        file--;
    file++;

    // Allocate a directory entry for our new file.
    dirent = createDirectoryEntry(directory);
    if (dirent == CODALFS_INVALID_ADDRESS)
        return CODALFS_INVALID_ADDRESS;

    // Create a new block to represent the file.
    uint16_t newBlock = getFreeBlock();
    if (newBlock == 0)
        return CODALFS_INVALID_ADDRESS;

    // Populate our assigned Directory Entry.
    DirectoryEntry d;
    strcpy(d.file_name, file);
    d.first_block = newBlock;

    if (isDirectory)
    {
        // Mark as a directory, and set a fixed length (special case for directories, to minimize unnecessary FLASH erases).
        d.flags = CODALFS_DIRECTORY_ENTRY_VALID | CODALFS_DIRECTORY_ENTRY_DIRECTORY;
        d.length = CODALFS_DIRECTORY_LENGTH;
    }
    else
    {
        // We leave the file size as unwritten for regular files - pending a possible forthcoming write/close operation.
        d.flags = CODALFS_DIRECTORY_ENTRY_NEW;
        d.length = 0xffffffff;
    }

    // Push the new data back to FLASH memory
    cache.write(dirent, &d, sizeof(DirectoryEntry));
    fileTableWrite(d.first_block, CODALFS_EOF);

    return dirent;
}

/**
 * Loads the given block (if necessary) and returns a volatile pointer to the cached RAM
 * for the given logical address.
 */
uint8_t *CodalFS::getCachedData(uint32_t address)
{
    address += CODALFS_OFFSET; //todo hack

    if (address < flash.getFlashStart() || address >= flash.getFlashEnd())
        return NULL;

    uint8_t *p = (uint8_t *)getBlock((uint16_t)((address - flash.getFlashStart()) / blockSize));
    int offset = address % blockSize;

    return p + offset;
}

/**
 * Searches the list of open files for one with the given identifier.
 *
 * @param fd A previsouly opened file identifier, as returned by open().
 * @param remove Remove the file descriptor from the list if true.
 * @return A FileDescriptor matching the given ID, or NULL if the file is not open.
 */
FileDescriptor *CodalFS::getFileDescriptor(int fd, bool remove)
{
    FileDescriptor *file = openFiles;
    FileDescriptor *prev = NULL;

    while (file)
    {
        if (file->id == fd)
        {
            if (remove)
            {
                if (prev)
                    prev->next = file->next;
                else
                    openFiles = file->next;
            }
            return file;
        }

        prev = file;
        file = file->next;
    }

    return NULL;
}

/**
 * Creates a new directory with the given name and location
 *
 * @param name The fully qualified name of the new directory.
 * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if the path is invalid, or MICROBIT_NO_RESOURCES if the FileSystem is full.
 */
int CodalFS::createDirectory(char const *name)
{
    DirectoryEntry *directory; // Directory holding this file.
    DirectoryEntry *dirent;    // Directory entry in the directory of this file.
    uint32_t directoryAddress; // Logical address of the directory holding this file.
    uint32_t direntAddress; // Logical address of the directory entry in the directory of this file.

    // Protect against accidental re-initialisation
    if ((status & CODALFS_STATUS_INITIALISED) == 0)
        return DEVICE_NOT_SUPPORTED;

    // Reject invalid filenames.
    if (!isValidFilename(name))
        return DEVICE_INVALID_PARAMETER;

    // Determine the directory for this file.
    directoryAddress = getDirectoryOf(name);
    directory = (DirectoryEntry *)getCachedData(directoryAddress);

    if (directory == NULL)
        return DEVICE_INVALID_PARAMETER;

    // Find the DirectoryEntry associated with the given name (if it exists).
    // We don't permit files or directories with the same name.
    direntAddress = getDirectoryEntry(name, directory);
    dirent = (DirectoryEntry *)getCachedData(direntAddress);

    if (dirent)
        return DEVICE_INVALID_PARAMETER;

    direntAddress = createFile(name, directory, true);
    dirent = (DirectoryEntry *)getCachedData(direntAddress);

    if (dirent == NULL)
        return DEVICE_NO_RESOURCES;

    return DEVICE_OK;
}

/**
 * Open a new file, and obtain a new file handle (int) to
 * read/write/seek the file. The flags are:
 *  - FS_READ : read from the file.
 *  - FS_WRITE : write to the file.
 *  - FS_CREAT : create a new file, if it doesn't already exist.
 *
 * If a file is opened that doesn't exist, and FS_CREAT isn't passed,
 * an error is returned, otherwise the file is created.
 *
 * @param filename name of the file to open, must contain only printable characters.
 * @param flags One or more of FS_READ, FS_WRITE or FS_CREAT.
 * @return return the file handle,DEVICE_NOT_SUPPORTED if the file system has
 *         not been initialised DEVICE_INVALID_PARAMETER if the filename is
 *         too large, DEVICE_NO_RESOURCES if the file system is full.
 *
 * @code
 * CodalFS f();
 * int fd = f.open("test.txt", FS_WRITE|FS_CREAT);
 * if(fd<0)
 *    print("file open error");
 * @endcode
 */
int CodalFS::open(char const *filename, uint32_t flags)
{
    FileDescriptor *file;      // File Descriptor of this file.
    DirectoryEntry *directory; // Directory holding this file.
    DirectoryEntry *dirent;    // Entry in the directory of this file.
    uint32_t directoryAddress; // Logical address of the directory holding this file.
    uint32_t direntAddress;    // Logical address of the entry in the directory of this file
    int id;                    // FileDescriptor id to be return to the caller.

    // Protect against accidental re-initialisation
    if ((status & CODALFS_STATUS_INITIALISED) == 0)
        return DEVICE_NOT_SUPPORTED;

    // Reject invalid filenames.
    if (!isValidFilename(filename))
        return DEVICE_INVALID_PARAMETER;

    // Determine the directory for this file.
    directoryAddress = getDirectoryOf(filename);
    directory = (DirectoryEntry *)getCachedData(directoryAddress);

    if (directory == NULL)
        return DEVICE_INVALID_PARAMETER;

    // Find the DirectoryEntry associated with the given file (if it exists).
    direntAddress = getDirectoryEntry(filename, directory);
    dirent = (DirectoryEntry *)getCachedData(direntAddress);

    // Special case for the root directory (as it has no parent)
    if (strcmp(filename, CODALFS_DIRECTORY_SEPARATOR_STR) == 0)
    {
        direntAddress = directoryAddress;
        dirent = directory;
    }

    // Only permit files to be opened once...
    // also, detemrine a valid ID for this open file as we go.
    file = openFiles;
    id = 0;

    while (file && dirent)
    {
        if (file->dirent == direntAddress)
            return DEVICE_NOT_SUPPORTED;

        if (file->id == id)
        {
            id++;
            file = openFiles;
            continue;
        }

        file = file->next;
    }

    if (dirent == NULL)
    {
        // If the file doesn't exist, and we haven't been asked to create it, then there's nothing we can do.
        if (!(flags & FS_CREAT))
            return DEVICE_INVALID_PARAMETER;

        direntAddress = createFile(filename, directory, false);
        dirent = (DirectoryEntry *)getCachedData(direntAddress);

        if (dirent == NULL)
            return DEVICE_NO_RESOURCES;
    }

    // Try to add a new FileDescriptor into this directory.
    file = new FileDescriptor;
    if (file == NULL)
        return DEVICE_NO_RESOURCES;

    // Populate the FileDescriptor
    file->flags = (flags & ~(FS_CREAT));
    file->id = id;
    file->length = dirent->flags == CODALFS_DIRECTORY_ENTRY_NEW ? 0 : dirent->length;
    file->seek = (flags & FS_APPEND) ? file->length : 0;
    file->dirent = direntAddress;
    file->directory = directoryAddress;

    // Add the file descriptor to the chain of open files.
    file->next = openFiles;
    openFiles = file;

    // Return the FileDescriptor id to the user
    return file->id;
}

/**
 * Writes back all state associated with the given file to FLASH memory,
 * leaving the file open.
 *
 * @param fd file descriptor - obtained with open().
 * @return DEVICE_OK on success, DEVICE_NOT_SUPPORTED if the file system has not
 *         been initialised, DEVICE_INVALID_PARAMETER if the given file handle
 *         is invalid.
 *
 * @code
 * CodalFS f();
 * int fd = f.open("test.txt", FS_READ);
 *
 * ...
 *
 * f.flush(fd);
 * @endcode
 */
int CodalFS::flush(int fd)
{
    // Protect against accidental re-initialisation
    if ((status & CODALFS_STATUS_INITIALISED) == 0)
        return DEVICE_NOT_SUPPORTED;

    FileDescriptor *file = getFileDescriptor(fd);

    // Ensure the file is open.
    if (file == NULL)
        return DEVICE_INVALID_PARAMETER;

    // If the file has changed size, create an updated directory entry for the file, reflecting its new length.
	DirectoryEntry *dirent = (DirectoryEntry *)getCachedData(file->dirent);
    if (dirent->length != file->length)
    {
        DirectoryEntry d = *dirent;
        d.length = file->length;

        // Do some optimising to reduce FLASH churn if this is the first write to a file. No need then to create a new dirent...
        if (dirent->flags == CODALFS_DIRECTORY_ENTRY_NEW)
        {
            d.flags = CODALFS_DIRECTORY_ENTRY_VALID;
            cache.write(file->dirent, &d, sizeof(DirectoryEntry));
        }

        // Otherwise, replace the dirent with a freshly allocated one, and mark the other as INVALID.
        else
        {
            uint32_t newDirent;
            uint16_t value = CODALFS_DELETED;
            DirectoryEntry *directory = (DirectoryEntry *)getCachedData(file->directory);

            // invalidate the old directory entry and create a new one with the updated data.
            cache.write(file->dirent + offsetof(DirectoryEntry, flags), &value, 2);
            newDirent = createDirectoryEntry(directory);
            cache.write(newDirent, &d, sizeof(DirectoryEntry));
        }
    }

    return DEVICE_OK;
}

/**
 * Close the specified file handle.
 * File handle resources are then made available for future open() calls.
 *
 * close() must be called at some point to ensure the filesize in the
 * FT is synced with the cached value in the FD.
 *
 * @warning if close() is not called, the FT may not be correct,
 * leading to data loss.
 *
 * @param fd file descriptor - obtained with open().
 * @return DEVICE_OK on success, DEVICE_NOT_SUPPORTED if the file system has not
 *         been initialised, DEVICE_INVALID_PARAMETER if the given file handle
 *         is invalid.
 *
 * @code
 * CodalFS f();
 * int fd = f.open("test.txt", FS_READ);
 * if(!f.close(fd))
 *    print("error closing file.");
 * @endcode
 */
int CodalFS::close(int fd)
{
    // Firstly, ensure all unwritten data is flushed.
    int r = flush(fd);

    // If the flush called failed on validation, pass the error code onto the caller.
    if (r != DEVICE_OK)
        return r;

    // Remove the file descriptor from the list of open files, and free it.
    // n.b. we know this is safe, as flush() validates this.
    delete getFileDescriptor(fd, true);

    return DEVICE_OK;
}

/**
 * Move the current position of a file handle, to be used for
 * subsequent read/write calls.
 *
 * The offset modifier can be:
 *  - FS_SEEK_SET set the absolute seek position.
 *  - FS_SEEK_CUR set the seek position based on the current offset.
 *  - FS_SEEK_END set the seek position from the end of the file.
 * E.g. to seek to 2nd-to-last byte, use offset=-1.
 *
 * @param fd file handle, obtained with open()
 * @param offset new offset, can be positive/negative.
 * @param flags
 * @return new offset position on success, DEVICE_NOT_SUPPORTED if the file system
 *         is not intiialised, DEVICE_INVALID_PARAMETER if the flag given is invalid
 *         or the file handle is invalid.
 *
 * @code
 * CodalFS f;
 * int fd = f.open("test.txt", FS_READ);
 * f.seek(fd, -100, FS_SEEK_END); //100 bytes before end of file.
 * @endcode
 */
int CodalFS::seek(int fd, int offset, uint8_t flags)
{
    FileDescriptor *file;
    int position;

    // Protect against accidental re-initialisation
    if ((status & CODALFS_STATUS_INITIALISED) == 0)
        return DEVICE_NOT_SUPPORTED;

    // Ensure the file is open.
    file = getFileDescriptor(fd);

    if (file == NULL)
        return DEVICE_INVALID_PARAMETER;

    position = file->seek;

    if (flags == FS_SEEK_SET)
        position = offset;

    if (flags == FS_SEEK_END)
        position = file->length + offset;

    if (flags == FS_SEEK_CUR)
        position = file->seek + offset;

    if (position < 0 || (uint32_t)position > file->length)
        return DEVICE_INVALID_PARAMETER;

    file->seek = position;

    return position;
}

/**
 * Read data from the file.
 *
 * Read len bytes from the current seek position in the file, into the
 * buffer. On each invocation to read, the seek position of the file
 * handle is incremented atomically, by the number of bytes returned.
 *
 * @param fd File handle, obtained with open()
 * @param buffer to store data
 * @param size number of bytes to read
 * @return number of bytes read on success, DEVICE_NOT_SUPPORTED if the file
 *         system is not initialised, or this file was not opened with the
 *         FS_READ flag set, DEVICE_INVALID_PARAMETER if the given file handle
 *         is invalid.
 *
 * @code
 * CodalFS f;
 * int fd = f.open("read.txt", FS_READ);
 * if(f.read(fd, buffer, 100) != 100)
 *    print("read error");
 * @endcode
 */
int CodalFS::read(int fd, uint8_t *buffer, int size)
{
    FileDescriptor *file;
    uint16_t block;
    uint8_t *readPointer;
    uint8_t *writePointer;
    DirectoryEntry *dirent;

    uint32_t offset;
    uint32_t position = 0;
    int bytesCopied = 0;
    int segmentLength;

    // Protect against accidental re-initialisation
    if ((status & CODALFS_STATUS_INITIALISED) == 0)
        return DEVICE_NOT_SUPPORTED;

    // Ensure the file is open.
    file = getFileDescriptor(fd);

    if (file == NULL || buffer == NULL || size == 0)
        return DEVICE_INVALID_PARAMETER;

    // Validate the read length.
    size = min(size, file->length - file->seek);

    // Find the read position.
    dirent = (DirectoryEntry *)getCachedData(file->dirent);
    block = dirent->first_block;

    // Walk the file table until we reach the start block
    while (file->seek - position > blockSize)
    {
        block = getNextFileBlock(block);
        position += blockSize;
    }

    // Once we have the correct start block, handle the byte offset.
    offset = file->seek - position;

    // Now, start copying bytes into the requested buffer.
    writePointer = buffer;
    while (bytesCopied < size)
    {
        // First, determine if we need to write a partial block.
        readPointer = (uint8_t *)getBlock(block) + offset;
        segmentLength = min(size - bytesCopied, blockSize - offset);

        if (segmentLength > 0)
            memcpy(writePointer, readPointer, segmentLength);

        bytesCopied += segmentLength;
        writePointer += segmentLength;
        offset += segmentLength;

        if (offset == blockSize)
        {
            block = getNextFileBlock(block);
            offset = 0;

            // Handle unexpected end of file - can occur when reading directories as files.
            if (block == CODALFS_EOF)
                break;
        }
    }

    file->seek += bytesCopied;

    return bytesCopied;
}

/**
 * Write a given buffer to the file provided.
 *
 * @param file FileDescriptor of the file to write
 * @param buffer The start of the buffer to write
 * @param length The number of bytes to write
 */
int CodalFS::writeBuffer(FileDescriptor *file, uint8_t *buffer, int size)
{
    uint16_t block, newBlock;
    uint8_t *readPointer;
    uint32_t writePointer;
    DirectoryEntry *dirent;

    uint32_t offset;
    uint32_t position = 0;
    int bytesCopied = 0;
    int segmentLength;

    // Find the read position.
    dirent = (DirectoryEntry *)getCachedData(file->dirent);
    block = dirent->first_block;

    // Walk the file table until we reach the start block
    while (file->seek - position > blockSize)
    {
        block = getNextFileBlock(block);
        position += blockSize;
    }

    // Once we have the correct start block, handle the byte offset.
    offset = file->seek - position;
    writePointer = addressOfBlock(block) + offset;

    // Now, start copying bytes from the requested buffer.
    readPointer = buffer;
    while (bytesCopied < size)
    {
        // First, determine if we need to write a partial block.
        segmentLength = min(size - bytesCopied, blockSize - offset);

        if (segmentLength != 0)
            cache.write(writePointer, readPointer, segmentLength);

        offset += segmentLength;
        bytesCopied += segmentLength;
        readPointer += segmentLength;

        if (offset == blockSize && bytesCopied < size)
        {
            newBlock = getFreeBlock();
            if (newBlock == 0)
                break;

            fileTableWrite(newBlock, CODALFS_EOF);
            fileTableWrite(block, newBlock);

            block = newBlock;

            writePointer = addressOfBlock(block);
            offset = 0;
        }
    }

    // update the filelength metadata and seek position such that multiple writes are sequential.
    file->length = max(file->length, file->seek + bytesCopied);
    file->seek += bytesCopied;

    return bytesCopied;
}

/**
 * Determines if the given filename is a valid filename for use in CodalFS.
 * valid filenames must be >0 characters in length, NULL terminated and contain
 * only printable characters. Each directory component within this string must
 * not have a length > CODALFS_FILENAME_LENGTH
 *
 * @param name The name of the file to test.
 * @return true if the filename is valid, false otherwise.
 */
bool CodalFS::isValidFilename(const char *name)
{
    auto length = strlen(name);

    // Name must be null terminated
    if (name == NULL || length == 0)
    {
        return false;
    }

    int currentDirectoryLength = 0;

    for (unsigned int i = 0; i < length; i++)
    {
        currentDirectoryLength++;

        if (name[i] < 32 || name[i] > 126)
        {
            return false;
        }

        if (name[i] == CODALFS_DIRECTORY_SEPARATOR) {
            if (currentDirectoryLength == 0) {
                return false; // There shouldn't be duplicate separators (//)
            }

            currentDirectoryLength = 0;
        }

        if (currentDirectoryLength > CODALFS_FILENAME_LENGTH) {
            return false; // One component of this filename is too long
        }
    }

    return true;
}

/**
 * Write data to the file.
 *
 * Write from buffer, len bytes to the current seek position.
 * On each invocation to write, the seek position of the file handle
 * is incremented atomically, by the number of bytes returned.
 *
 * The cached filesize in the FD is updated on this call. Also, the
 * FT file size is updated only if a new page(s) has been written too,
 * to reduce the number of FT writes.
 *
 * @param fd File handle
 * @param buffer the buffer from which to write data
 * @param len number of bytes to write
 * @return number of bytes written on success, DEVICE_NO_RESOURCES if data did
 *         not get written to flash or the file system has not been initialised,
 *         or this file was not opened with the FS_WRITE flag set, DEVICE_INVALID_PARAMETER
 *         if the given file handle is invalid.
 *
 * @code
 * CodalFS f();
 * int fd = f.open("test.txt", FS_WRITE);
 * if(f.write(fd, "hello!", 7) != 7)
 *    print("error writing");
 * @endcode
 */
int CodalFS::write(int fd, uint8_t *buffer, int size)
{
    FileDescriptor *file;

    // Protect against accidental re-initialisation
    if ((status & CODALFS_STATUS_INITIALISED) == 0)
        return DEVICE_NOT_SUPPORTED;

    // Ensure the file is open.
    file = getFileDescriptor(fd);

    if (file == NULL || buffer == NULL || size == 0)
        return DEVICE_INVALID_PARAMETER;

    return writeBuffer(file, buffer, size);
}

/**
 * Remove a file from the system, and free allocated assets
 * (including assigned blocks which are returned for use by other files).
 *
 * @param filename null-terminated name of the file to remove.
 * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if the given filename
 *         does not exist, DEVICE_CANCELLED if something went wrong.
 *
 * @code
 * CodalFS f;
 * if(!f.remove("file.txt"))
 *     print("file could not be removed")
 * @endcode
 */
int CodalFS::remove(char const *filename)
{
    int fd = open(filename, FS_READ);
    uint16_t block, nextBlock;
    uint16_t value;

    // If the file can't be opened, then it is impossible to delete. Pass through any error codes.
    if (fd < 0)
        return fd;

    FileDescriptor *file = getFileDescriptor(fd, true);
    DirectoryEntry *dirent;

    // To erase a file, all we need to do is mark its directory entry and data blocks as INVALID.
    // First mark the file table
    dirent = (DirectoryEntry *)getCachedData(file->dirent);
    block = dirent->first_block;
    while (block != CODALFS_EOF)
    {
        nextBlock = fileTableRead(block);
        fileTableWrite(block, CODALFS_DELETED);
        block = nextBlock;
    }

    // Mark the directory entry of this file as invalid.
    value = CODALFS_DIRECTORY_ENTRY_DELETED;
    cache.write(file->dirent + offsetof(DirectoryEntry, flags), &value, 2);

    // release file metadata
    delete file;

    return DEVICE_OK;
}