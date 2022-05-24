/*
The MIT License (MIT)
Copyright (c) 2022 Lancaster University.
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

#ifndef CODAL_MICROFLASHINTERFACE_H
#define CODAL_MICROFLASHINTERFACE_H

#define WEBUSB_BUFFER_LENGTH 256

#include "CodalDevice.h"
#include "NVMController.h"

using namespace codal;

class MicroflashInterface : public CodalComponent
{
private:

public:
    MicroflashInterface(NVMController &nvm);

    volatile uint8_t webUsbBuffer[WEBUSB_BUFFER_LENGTH];

    void idleCallback() override;

    ~MicroflashInterface() override;

    NVMController &nvm;
};

#endif // CODAL_MICROFLASHINTERFACE_H
