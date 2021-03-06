//
// Bareflank Hypervisor
//
// Copyright (C) 2015 Assured Information Security, Inc.
// Author: Rian Quinn        <quinnr@ainfosec.com>
// Author: Brendan Kerrigan  <kerriganb@ainfosec.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <test.h>
#include <intrinsics/portio_x64.h>

using namespace x64;

std::map<uint16_t, uint16_t> g_ports;

extern "C" uint8_t
__inb(uint16_t port) noexcept
{ return gsl::narrow_cast<uint8_t>(g_ports[port]); }

extern "C" uint16_t
__inw(uint16_t port) noexcept
{ return g_ports[port]; }

extern "C" void
__outb(uint16_t port, uint8_t val) noexcept
{ g_ports[port] = val; }

extern "C" void
__outw(uint16_t port, uint16_t val) noexcept
{ g_ports[port] = val; }

void
intrinsics_ut::test_portio_x64_byte()
{
    portio::outb(10, 100U);
    this->expect_true(portio::inb(10) == 100U);
}

void
intrinsics_ut::test_portio_x64_word()
{
    portio::outw(10, 10000U);
    this->expect_true(portio::inw(10) == 10000U);
}
