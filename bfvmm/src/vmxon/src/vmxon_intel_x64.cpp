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

#include <gsl/gsl>

#include <debug.h>
#include <vmxon/vmxon_intel_x64.h>
#include <memory_manager/memory_manager.h>

using namespace x64;
using namespace intel_x64;

vmxon_intel_x64::vmxon_intel_x64(std::shared_ptr<intrinsics_intel_x64> intrinsics) :
    m_intrinsics(std::move(intrinsics)),
    m_vmxon_enabled(false),
    m_vmxon_region_phys(0)
{
    if (!m_intrinsics)
        m_intrinsics = std::make_shared<intrinsics_intel_x64>();
}

void
vmxon_intel_x64::start()
{
    if (this->is_vmx_operation_enabled())
        throw std::logic_error("vmxon already enabled");

    this->check_cpuid_vmx_supported();
    this->check_vmx_capabilities_msr();
    this->check_ia32_vmx_cr0_fixed_msr();
    this->check_ia32_feature_control_msr();
    this->check_v8086_disabled();

    this->create_vmxon_region();

    auto ___ = gsl::on_failure([&]
    { this->release_vmxon_region(); });

    this->enable_vmx_operation();

    auto ___ = gsl::on_failure([&]
    { this->disable_vmx_operation(); });

    if (!this->is_vmx_operation_enabled())
        throw std::logic_error("failed to enable VMXON");

    this->check_ia32_vmx_cr4_fixed_msr();
    this->execute_vmxon();
}

void
vmxon_intel_x64::stop()
{
    this->execute_vmxoff();
    this->disable_vmx_operation();

    if (this->is_vmx_operation_enabled())
        throw std::logic_error("failed to disable VMXON");

    this->release_vmxon_region();
}

void
vmxon_intel_x64::check_cpuid_vmx_supported()
{
    if ((m_intrinsics->cpuid_ecx(1) & (1 << 5)) == 0)
        throw std::logic_error("VMX extensions not supported");
}

void
vmxon_intel_x64::check_vmx_capabilities_msr()
{
    if (msrs::ia32_vmx_basic::physical_address_width::get() != 0)
        throw std::logic_error("invalid physical address width");

    if (msrs::ia32_vmx_basic::memory_type::get() != x64::memory_type::write_back)
        throw std::logic_error("invalid memory type");

    if (msrs::ia32_vmx_basic::true_based_controls::get() == 0)
        throw std::logic_error("invalid vmx true based controls");
}

void
vmxon_intel_x64::check_ia32_vmx_cr0_fixed_msr()
{
    auto cr0 = cr0::get();
    auto ia32_vmx_cr0_fixed0 = msrs::ia32_vmx_cr0_fixed0::get();
    auto ia32_vmx_cr0_fixed1 = msrs::ia32_vmx_cr0_fixed1::get();

    if (0 != ((~cr0 & ia32_vmx_cr0_fixed0) | (cr0 & ~ia32_vmx_cr0_fixed1)))
        throw std::logic_error("invalid cr0");
}

void
vmxon_intel_x64::check_ia32_vmx_cr4_fixed_msr()
{
    auto cr4 = cr4::get();
    auto ia32_vmx_cr4_fixed0 = msrs::ia32_vmx_cr4_fixed0::get();
    auto ia32_vmx_cr4_fixed1 = msrs::ia32_vmx_cr4_fixed1::get();

    if (0 != ((~cr4 & ia32_vmx_cr4_fixed0) | (cr4 & ~ia32_vmx_cr4_fixed1)))
        throw std::logic_error("invalid cr4");
}

void
vmxon_intel_x64::check_ia32_feature_control_msr()
{
    if (msrs::ia32_feature_control::lock_bit::get() == 0)
        throw std::logic_error("vmx lock bit == 0 is unsupported");
}

void
vmxon_intel_x64::check_v8086_disabled()
{
    if (rflags::virtual_8086_mode::get() != 0)
        throw std::logic_error("v8086 mode is not supported");
}

void
vmxon_intel_x64::enable_vmx_operation() noexcept
{
    cr4::vmx_enable_bit::set(1UL);
}

void
vmxon_intel_x64::disable_vmx_operation() noexcept
{
    cr4::vmx_enable_bit::set(0UL);
}

void
vmxon_intel_x64::create_vmxon_region()
{
    auto ___ = gsl::on_failure([&]
    { this->release_vmxon_region(); });

    m_vmxon_region = std::make_unique<uint32_t[]>(1024);
    m_vmxon_region_phys = g_mm->virtptr_to_physint(m_vmxon_region.get());

    if (m_vmxon_region_phys == 0)
        throw std::logic_error("m_vmxon_region_phys == nullptr");

    gsl::span<uint32_t> id{m_vmxon_region.get(), 1024};
    id[0] = gsl::narrow<uint32_t>(msrs::ia32_vmx_basic::revision_id::get());
}

void
vmxon_intel_x64::release_vmxon_region() noexcept
{
    m_vmxon_region.reset();
    m_vmxon_region_phys = 0;
}

void
vmxon_intel_x64::execute_vmxon()
{
    if (m_vmxon_enabled)
        throw std::logic_error("vmxon has already been executed");

    if (!m_intrinsics->vmxon(&m_vmxon_region_phys))
        throw std::logic_error("vmxon failed");

    m_vmxon_enabled = true;
}

void
vmxon_intel_x64::execute_vmxoff()
{
    if (!m_vmxon_enabled)
    {
        bfwarning << "execute_vmxoff: VMX operation already disabled" << bfendl;
        return;
    }

    if (!m_intrinsics->vmxoff())
        throw std::logic_error("vmxoff failed");

    m_vmxon_enabled = false;
}

bool
vmxon_intel_x64::is_vmx_operation_enabled()
{
    return cr4::vmx_enable_bit::get() != 0;
}
