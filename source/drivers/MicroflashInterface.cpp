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

#include <NVMController.h>
#include <CodalFS.h>
#include "MicroflashInterface.h"
#include "CodalDmesg.h"

#define WEBUSB_NO_OP 0x00
#define WEBUSB_OP_PUSH_PATCH 0x01
#define WEBUSB_OP_REMOUNT 0x02
#define WEBUSB_OP_ERASE_PAGE 0x03
#define WEBUSB_OP_FORMAT_FS 0x04
#define WEBUSB_OP_RESPONSE_SUCCESS 0xFF

#define WEBUSB_DEBUG false

using namespace codal;

uint32_t u8_to_u32(const uint8_t *bytes)
{
    uint32_t u32;

    memcpy(&u32, bytes, sizeof u32);

    return u32;
}

static uint16_t u8_to_u16(const uint8_t *bytes)
{
    uint16_t u16;

    memcpy(&u16, bytes, sizeof u16);

    return u16;
}

MicroflashInterface::MicroflashInterface(NVMController &nvm) : nvm(nvm)
{
    memset((void *)&webUsbBuffer, 0, WEBUSB_BUFFER_LENGTH);
    webUsbBuffer[0] = WEBUSB_OP_RESPONSE_SUCCESS;

    this->status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;
}

static bool handlingCommand = false;

static void handleCommand(void *interface)
{
    auto self = (MicroflashInterface *)interface;
    auto webUsbBuffer = self->webUsbBuffer;

    uint8_t cmd = webUsbBuffer[0];

    if (cmd == WEBUSB_NO_OP || cmd == WEBUSB_OP_RESPONSE_SUCCESS)
    {
        handlingCommand = false;
        return; // not modified / no-op
    }

#if WEBUSB_DEBUG
    DMESGF("Found WebUSB command %d", cmd);
#endif

    if (cmd == WEBUSB_OP_PUSH_PATCH)
    {
        uint32_t patchPos = u8_to_u32((uint8_t *)(webUsbBuffer + 1));
        uint8_t patchLength = webUsbBuffer[5];

#if WEBUSB_DEBUG
        DMESG("Beginning patch command: pos %d len %d", patchPos, patchLength);
        DMESG("=== Patch payload:");

        for (int test = 0; test < 256; test++)
        {
            DMESGN("%x ", webUsbBuffer[test]);

            if (test > 0 && test % 16 == 0)
            {
                DMESG(""); // new line
            }
        }

        DMESG("===");
#endif

        if (patchLength > 250)
        {
            DMESG("Patch length is too long! (%d > 250)", patchLength);
            patchLength = 250; // todo handle better?
        }

        auto alignedStart = patchPos - ((patchPos % 4) == 0 ? 0 : (patchPos % 4));
        auto end = patchPos + patchLength;
        auto alignedEnd = end + (end % 4 == 0 ? 0 : end % 4);

        auto alignedLength = alignedEnd - alignedStart;

        auto posOffset = patchPos - alignedStart;

        auto *wordAlignedData = new uint8_t[alignedLength];

#if WEBUSB_DEBUG
        DMESG("word aligned pos %d word aligned len %d", alignedStart,
              alignedLength);
        DMESG("pos with offset %d l %d", alignedStart + CODALFS_OFFSET, alignedLength);
#endif

        CodalFS::defaultFileSystem->cache.read(alignedStart, wordAlignedData,
                                               alignedLength);

        for (auto i = 0; i < patchLength; i++)
        {
            wordAlignedData[posOffset + i] = webUsbBuffer[i + 6];
        }

        CodalFS::defaultFileSystem->cache.write(alignedStart, wordAlignedData,
                                                alignedLength);
#if WEBUSB_DEBUG
        DMESG("Applied patch!");
#endif
    }
    else if (cmd == WEBUSB_OP_REMOUNT)
    {
        self->nvm.remount();
#if WEBUSB_DEBUG
        DMESG("Remounted!");
#endif
    }
    else if (cmd == WEBUSB_OP_ERASE_PAGE)
    {
        uint32_t clearPos = u8_to_u32((uint8_t *)(webUsbBuffer + 1));

        //CodalFS::defaultFileSystem->cache.erase(clearPos);
        CodalFS::defaultFileSystem->cache.clear();
        self->nvm.erase(clearPos + CODALFS_OFFSET);

#if WEBUSB_DEBUG
        DMESG("Cleared page %d!", clearPos);
#endif
    }
    else if (cmd == WEBUSB_OP_FORMAT_FS)
    {
#if WEBUSB_DEBUG
        DMESG("Formatting filesystem...");
#endif
        CodalFS::defaultFileSystem->format();
    }

    for (int i = 1; i < 256; i++)
    {
        webUsbBuffer[i] = 0;
    }

    webUsbBuffer[0] = WEBUSB_OP_RESPONSE_SUCCESS;

#if WEBUSB_DEBUG
    DMESG("Handled command successfully...");
#endif

    handlingCommand = false;
}

void MicroflashInterface::idleCallback()
{
    uint8_t cmd = webUsbBuffer[0];

    if (handlingCommand || cmd == WEBUSB_NO_OP || cmd == WEBUSB_OP_RESPONSE_SUCCESS)
    {
        return; // not modified / no-op
    }

    handlingCommand = true;
    create_fiber(handleCommand, (void *) this);
}

MicroflashInterface::~MicroflashInterface()
{
    status = 0;
    removeComponent();
}
