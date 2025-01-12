/*
   EDI output.
    This defines a few TAG items as defined ETSI TS 102 821 and
    ETSI TS 102 693

   Copyright (C) 2019
   Matthias P. Braendli, matthias.braendli@mpb.li

    http://www.opendigitalradio.org

   */
/*
   This file is part of ODR-DabMux.

   ODR-DabMux is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMux is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMux.  If not, see <http://www.gnu.org/licenses/>.
   */

#pragma once

#include "config.h"
#include <vector>
#include <array>
#include <chrono>
#include <string>
#include <stdint.h>

namespace edi {

class TagItem
{
    public:
        virtual std::vector<uint8_t> Assemble() = 0;
};

// ETSI TS 102 693, 5.1.1 Protocol type and revision
class TagStarPTR : public TagItem
{
    public:
        std::string protocol = "";
        std::vector<uint8_t> Assemble();
};

// ETSI TS 102 693, 5.1.2 DAB STI-D(LI) Management
class TagDSTI : public TagItem
{
    public:
        std::vector<uint8_t> Assemble();

        // dsti Header
        bool stihf = false;
        bool atstf = false; // presence of atst data
        bool rfadf = false;
        uint16_t dflc = 0; // modulo 5000 frame counter

        // STI Header (optional)
        uint8_t stat = 0;
        uint16_t spid = 0;

        /* UTCO: Offset (in seconds) between UTC and the Seconds value. The
         * value is expressed as an unsigned 8-bit quantity. As of February
         * 2009, the value shall be 2 and shall change as a result of each
         * modification of the number of leap seconds, as proscribed by
         * International Earth Rotation and Reference Systems Service (IERS).
         *
         * According to Annex F
         *  EDI = TAI - 32s (constant)
         *  EDI = UTC + UTCO
         * we derive
         *  UTCO = TAI-UTC - 32
         * where the TAI-UTC offset is given by the USNO bulletin using
         * the ClockTAI module.
         */
        uint8_t utco = 0;

        /* Update the EDI time. t is in UTC */
        void set_edi_time(const std::time_t t, int tai_utc_offset);

        /* The number of SI seconds since 2000-01-01 T 00:00:00 UTC as an
         * unsigned 32-bit quantity. Contrary to POSIX, this value also
         * counts leap seconds.
         */
        uint32_t seconds = 0;

        /* TSTA: Shall be the 24 least significant bits of the Time Stamp
         * (TIST) field from the STI-D(LI) Frame. The full definition for the
         * STI TIST can be found in annex B of EN 300 797 [4]. The most
         * significant 8 bits of the TIST field of the incoming STI-D(LI)
         * frame, if required, may be carried in the RFAD field.
         */
        uint32_t tsta = 0xFFFFFF;

        std::array<uint8_t, 9> rfad;
};

// ETSI TS 102 693, 5.1.4 STI-D Payload Stream <m>
class TagSSm : public TagItem
{
    public:
        std::vector<uint8_t> Assemble();

        // SSTCn
        uint8_t rfa = 0;
        uint8_t tid = 0; // See EN 300 797, 5.4.1.1. Value 0 means "MSC sub-channel"
        uint8_t tidext = 0; // EN 300 797, 5.4.1.3, Value 0 means "MSC audio stream"
        bool crcstf = false;
        uint16_t stid = 0;

        // Pointer to ISTDm data
        const uint8_t *istd_data;
        size_t istd_length; // bytes

        uint16_t id = 0;
};

// ETSI TS 102 821, 5.2.2.2 Dummy padding
class TagStarDMY : public TagItem
{
    public:
        /* length is the TAG value length in bytes */
        TagStarDMY(uint32_t length) : length_(length) {}
        std::vector<uint8_t> Assemble();

    private:
        uint32_t length_;
};

}

