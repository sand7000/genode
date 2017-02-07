/*
 * \brief   Pbxa9 specific platform implementation
 * \author  Stefan Kalkowski
 * \date    2016-10-20
 */

/*
 * Copyright (C) 2016 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* bootstrap includes */
#include <platform.h>

Platform::Board::Board()
: early_ram_regions(Memory_region { RAM_0_BASE, RAM_0_SIZE },
                    Memory_region { RAM_1_BASE, RAM_1_SIZE }),
  core_mmio(Memory_region { Board::CORTEX_A9_PRIVATE_MEM_BASE,
                            Board::CORTEX_A9_PRIVATE_MEM_SIZE },
            Memory_region { Board::PL011_0_MMIO_BASE,
                            Board::PL011_0_MMIO_SIZE },
            Memory_region { Board::PL310_MMIO_BASE,
                            Board::PL310_MMIO_SIZE }) { }


bool Cortex_a9::Board::errata(Cortex_a9::Board::Errata err) {
	return false; }


void Cortex_a9::Board::wake_up_all_cpus(void * const ip)
{
	/**
	 * set the entrypoint for the other CPUs via the flags register
	 * of the system control registers. ARMs boot monitor code will
	 * read out this register and jump to it after the cpu received
	 * an interrupt
	 */
	struct System_control : Genode::Mmio
	{
		struct Flagsset : Register<0x30, 32> { };
		struct Flagsclr : Register<0x34, 32> { };

		System_control(void * const ip)
			: Mmio(SYSTEM_CONTROL_MMIO_BASE)
		{
			write<Flagsclr>(~0UL);
			write<Flagsset>(reinterpret_cast<Flagsset::access_t>(ip));
		}
	} sc(ip);
}