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
#include <serial/serial_port_intel_x64.h>

#include <map>
static std::map<uint16_t, uint8_t> g_ports;

extern "C" uint8_t
__inb(uint16_t port) noexcept
{
    return g_ports[port];
}

extern "C" void
__outb(uint16_t port, uint8_t val) noexcept
{
    g_ports[port] = val;
}

void
serial_ut::test_serial_null_intrinsics()
{
    EXPECT_NO_EXCEPTION(std::make_unique<serial_port_intel_x64>());
}

void
serial_ut::test_serial_success()
{
    EXPECT_TRUE(serial_port_intel_x64::instance()->port() == DEFAULT_COM_PORT);
    EXPECT_TRUE(serial_port_intel_x64::instance()->baud_rate() == serial_port_intel_x64::DEFAULT_BAUD_RATE);
    EXPECT_TRUE(serial_port_intel_x64::instance()->data_bits() == serial_port_intel_x64::DEFAULT_DATA_BITS);
    EXPECT_TRUE(serial_port_intel_x64::instance()->stop_bits() == serial_port_intel_x64::DEFAULT_STOP_BITS);
    EXPECT_TRUE(serial_port_intel_x64::instance()->parity_bits() == serial_port_intel_x64::DEFAULT_PARITY_BITS);

    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + BAUD_RATE_LO_REG]) == ((serial_port_intel_x64::DEFAULT_BAUD_RATE & 0x00FF) >> 0));
    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + BAUD_RATE_HI_REG]) == ((serial_port_intel_x64::DEFAULT_BAUD_RATE & 0xFF00) >> 8));
    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + FIFO_CONTROL_REG] & FIFO_CONTROL_ENABLE_FIFOS) != 0);
    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + FIFO_CONTROL_REG] & FIFO_CONTROL_CLEAR_RECIEVE_FIFO) != 0);
    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + FIFO_CONTROL_REG] & FIFO_CONTROL_CLEAR_TRANSMIT_FIFO) != 0);
    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + LINE_CONTROL_REG] & LINE_CONTROL_DATA_MASK) == serial_port_intel_x64::DEFAULT_DATA_BITS);
    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + LINE_CONTROL_REG] & LINE_CONTROL_STOP_MASK) == serial_port_intel_x64::DEFAULT_STOP_BITS);
    EXPECT_TRUE((g_ports[DEFAULT_COM_PORT + LINE_CONTROL_REG] & LINE_CONTROL_PARITY_MASK) == serial_port_intel_x64::DEFAULT_PARITY_BITS);
}

void
serial_ut::test_serial_set_baud_rate_success()
{
    auto serial = std::make_unique<serial_port_intel_x64>();

    serial->set_baud_rate(serial_port_intel_x64::baud_rate_50);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_50);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_75);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_75);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_110);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_110);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_150);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_150);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_300);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_300);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_600);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_600);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_1200);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_1200);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_1800);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_1800);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_2000);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_2000);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_2400);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_2400);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_3600);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_3600);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_4800);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_4800);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_7200);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_7200);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_9600);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_9600);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_19200);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_19200);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_38400);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_38400);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_57600);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_57600);
    serial->set_baud_rate(serial_port_intel_x64::baud_rate_115200);
    EXPECT_TRUE(serial->baud_rate() == serial_port_intel_x64::baud_rate_115200);
}

void
serial_ut::test_serial_set_data_bits_success()
{
    auto serial = std::make_unique<serial_port_intel_x64>();

    serial->set_data_bits(serial_port_intel_x64::char_length_5);
    EXPECT_TRUE(serial->data_bits() == serial_port_intel_x64::char_length_5);
    serial->set_data_bits(serial_port_intel_x64::char_length_6);
    EXPECT_TRUE(serial->data_bits() == serial_port_intel_x64::char_length_6);
    serial->set_data_bits(serial_port_intel_x64::char_length_7);
    EXPECT_TRUE(serial->data_bits() == serial_port_intel_x64::char_length_7);
    serial->set_data_bits(serial_port_intel_x64::char_length_8);
    EXPECT_TRUE(serial->data_bits() == serial_port_intel_x64::char_length_8);
}

void
serial_ut::test_serial_set_data_bits_success_extra_bits()
{
    auto serial = std::make_unique<serial_port_intel_x64>();

    auto bits = serial_port_intel_x64::DEFAULT_DATA_BITS | ~LINE_CONTROL_DATA_MASK;
    serial->set_data_bits(static_cast<serial_port_intel_x64::data_bits_t>(bits));

    EXPECT_TRUE(serial->data_bits() == serial_port_intel_x64::DEFAULT_DATA_BITS);
    EXPECT_TRUE(serial->stop_bits() == serial_port_intel_x64::DEFAULT_STOP_BITS);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::DEFAULT_PARITY_BITS);
}

void
serial_ut::test_serial_set_stop_bits_success()
{
    auto serial = std::make_unique<serial_port_intel_x64>();

    serial->set_stop_bits(serial_port_intel_x64::stop_bits_1);
    EXPECT_TRUE(serial->stop_bits() == serial_port_intel_x64::stop_bits_1);
    serial->set_stop_bits(serial_port_intel_x64::stop_bits_2);
    EXPECT_TRUE(serial->stop_bits() == serial_port_intel_x64::stop_bits_2);
}

void
serial_ut::test_serial_set_stop_bits_success_extra_bits()
{
    auto serial = std::make_unique<serial_port_intel_x64>();

    auto bits = serial_port_intel_x64::DEFAULT_STOP_BITS | ~LINE_CONTROL_STOP_MASK;
    serial->set_stop_bits(static_cast<serial_port_intel_x64::stop_bits_t>(bits));

    EXPECT_TRUE(serial->data_bits() == serial_port_intel_x64::DEFAULT_DATA_BITS);
    EXPECT_TRUE(serial->stop_bits() == serial_port_intel_x64::DEFAULT_STOP_BITS);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::DEFAULT_PARITY_BITS);
}

void
serial_ut::test_serial_set_parity_bits_success()
{
    auto serial = std::make_unique<serial_port_intel_x64>();

    serial->set_parity_bits(serial_port_intel_x64::parity_none);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::parity_none);
    serial->set_parity_bits(serial_port_intel_x64::parity_odd);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::parity_odd);
    serial->set_parity_bits(serial_port_intel_x64::parity_even);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::parity_even);
    serial->set_parity_bits(serial_port_intel_x64::parity_mark);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::parity_mark);
    serial->set_parity_bits(serial_port_intel_x64::parity_space);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::parity_space);
}

void
serial_ut::test_serial_set_parity_bits_success_extra_bits()
{
    auto serial = std::make_unique<serial_port_intel_x64>();

    auto bits = serial_port_intel_x64::DEFAULT_PARITY_BITS | ~LINE_CONTROL_PARITY_MASK;
    serial->set_parity_bits(static_cast<serial_port_intel_x64::parity_bits_t>(bits));

    EXPECT_TRUE(serial->data_bits() == serial_port_intel_x64::DEFAULT_DATA_BITS);
    EXPECT_TRUE(serial->stop_bits() == serial_port_intel_x64::DEFAULT_STOP_BITS);
    EXPECT_TRUE(serial->parity_bits() == serial_port_intel_x64::DEFAULT_PARITY_BITS);
}

void
serial_ut::test_serial_write_character()
{
    g_ports[DEFAULT_COM_PORT + LINE_STATUS_REG] = 0xFF;

    auto serial = std::make_unique<serial_port_intel_x64>();
    serial->write('c');
}

void
serial_ut::test_serial_write_string()
{
    g_ports[DEFAULT_COM_PORT + LINE_STATUS_REG] = 0xFF;

    auto serial = std::make_unique<serial_port_intel_x64>();
    serial->write("hello world");
}
