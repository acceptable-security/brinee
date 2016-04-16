#include <kernel/pci.h>
#include <stdio.h>
#include <stdlib.h>

pci_devlist_t* devlist;

uint16_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint64_t address;
    uint64_t lbus = (uint64_t)bus;
    uint64_t lslot = (uint64_t)slot;
    uint64_t lfunc = (uint64_t)func;
    uint16_t tmp = 0;
    address = (uint64_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | (0x80000000));
    outportl (0xCF8, address);
    tmp = (uint16_t)((inportl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

pci_device_t* pci_device_new(uint16_t bus, uint16_t slot, uint16_t function) {
    uint16_t vendor = pci_config_read(bus, slot, function, 0);

    if ( vendor == 0xFFFF ) {
        return 0;
    }

    pci_device_t* dev_obj = (pci_device_t*) malloc(sizeof(pci_device_t));

    dev_obj->bus = bus;
    dev_obj->slot = slot;
    dev_obj->function = function;
    dev_obj->vendor = vendor;
    dev_obj->device = pci_config_read(bus, slot, function, 2);

    dev_obj->command = pci_config_read(bus, slot, function, 4);
    dev_obj->status = pci_config_read(bus, slot, function, 6);

    dev_obj->revision = ( pci_config_read(bus, slot, function, 8) & 0x00FF );
    dev_obj->prog_if = ( pci_config_read(bus, slot, function, 8) & ~0x00FF ) >> 8;
    dev_obj->subclass = ( pci_config_read(bus, slot, function, 10) & 0x00FF );
    dev_obj->class = (pci_config_read(bus, slot, function, 10) & ~0x00FF) >> 8;

    dev_obj->cacheline_size = ( pci_config_read(bus, slot, function, 12) & 0x00FF );
    dev_obj->latency_timer = ( pci_config_read(bus, slot, function, 12) & ~0x00FF ) >> 8;
    dev_obj->header_type = pci_config_read(bus, slot, function, 14) & 0x00FF;
    dev_obj->bist = (pci_config_read(bus, slot, function, 14) & ~0x00FF) >> 8;

    if ( dev_obj->header_type == 0 ) {
            pci_device_normal_t* extra = (pci_device_normal_t*) malloc(sizeof(pci_device_normal_t));

            extra->baseaddr_0 = (pci_config_read(bus, slot, function, 0x10)) | (pci_config_read(bus, slot, function, 0x12) << 16);
            extra->baseaddr_1 = (pci_config_read(bus, slot, function, 0x14)) | (pci_config_read(bus, slot, function, 0x16) << 16);
            extra->baseaddr_2 = (pci_config_read(bus, slot, function, 0x18)) | (pci_config_read(bus, slot, function, 0x18) << 16);
            extra->baseaddr_3 = (pci_config_read(bus, slot, function, 0x1C)) | (pci_config_read(bus, slot, function, 0x1E) << 16);
            extra->baseaddr_4 = (pci_config_read(bus, slot, function, 0x20)) | (pci_config_read(bus, slot, function, 0x22) << 16);
            extra->baseaddr_5 = (pci_config_read(bus, slot, function, 0x24)) | (pci_config_read(bus, slot, function, 0x26) << 16);
            extra->cardbus_cis = (pci_config_read(bus, slot, function, 0x28)) | (pci_config_read(bus, slot, function, 0x2A) << 16);

            extra->subsystem_vendor = pci_config_read(bus, slot, function, 0x2C);
            extra->subsystem = pci_config_read(bus, slot, function, 0x2E);

            extra->expansion_rom = (pci_config_read(bus, slot, function, 0x30) << 8) & pci_config_read(bus, slot, function, 0x32);

            extra->capability = pci_config_read(bus, slot, function, 0x34) & 0xFF;

            extra->interrupt_line = pci_config_read(bus, slot, function, 0x3C) & 0xFF;
            extra->interrupt_pin = pci_config_read(bus, slot, function, 0x3D) & 0xFF;
            extra->min_grant = pci_config_read(bus, slot, function, 0x3E) & 0xFF;
            extra->max_latency = pci_config_read(bus, slot, function, 0x3F) & 0xFF;

            dev_obj->extra = (void*) extra;

    }
    else if ( dev_obj->header_type == 1 ) {
            pci_device_pcipci_t* extra1 = (pci_device_pcipci_t*) malloc(sizeof(pci_device_pcipci_t));

            extra1->baseaddr_0 = (pci_config_read(bus, slot, function, 0x10)) & (pci_config_read(bus, slot, function, 0x12) << 8);
            extra1->baseaddr_1 = (pci_config_read(bus, slot, function, 0x14)) & (pci_config_read(bus, slot, function, 0x16) << 8);

            extra1->primary_bus = pci_config_read(bus, slot, function, 0x18) & 0x00FF;
            extra1->secondary_bus = (pci_config_read(bus, slot, function, 0x18) & ~0x00FF) >> 8;
            extra1->subordinate_bus = pci_config_read(bus, slot, function, 0x1A) & 0x00FF;
            extra1->secondary_latency = (pci_config_read(bus, slot, function, 0x1A) & ~0x00FF) >> 8;

            extra1->io_base = pci_config_read(bus, slot, function, 0x1C) & 0x00FF;
            extra1->io_limit = (pci_config_read(bus, slot, function, 0x1C) & ~0x00FF) >> 8;
            extra1->secondary_status = pci_config_read(bus, slot, function, 0x1E);

            extra1->prefetch_mem_base = pci_config_read(bus, slot, function, 0x24);
            extra1->prefetch_mem_limit = pci_config_read(bus, slot, function, 0x26);

            extra1->prefetch_base = (pci_config_read(bus, slot, function, 0x28)) & (pci_config_read(bus, slot, function, 0x2A) << 8);
            extra1->prefetch_limit = (pci_config_read(bus, slot, function, 0x2C)) & (pci_config_read(bus, slot, function, 0x2E) << 8);

            extra1->io_upperbase = pci_config_read(bus, slot, function, 0x30);
            extra1->io_upperlimit = pci_config_read(bus, slot, function, 0x32);

            extra1->capability = pci_config_read(bus, slot, function, 0x34) & 0x00FF;

            extra1->expansion_rom = (pci_config_read(bus, slot, function, 0x38)) & (pci_config_read(bus, slot, function, 0x3A) << 8);

            extra1->interrupt_line = pci_config_read(bus, slot, function, 0x3C) & 0x00FF;
            extra1->interrupt_pin = (pci_config_read(bus, slot, function, 0x3C) & ~0x00FF) >> 8;
            extra1->bridge_control = pci_config_read(bus, slot, function, 0x3E);

            dev_obj->extra = (void*) extra1;
    }
    else {

            // printf("Unable to deal with device 0x%X\n", dev_obj->header_type);

            free(dev_obj);
            return 0;
    }

    return dev_obj;
}

void pci_probe() {
    uint32_t bus, slot, function;

    for ( bus = 0; bus < 256; bus++ ) {
        for ( slot = 0; slot < 32; slot++ ) {
            for ( function = 0; function < 8; function++ ) {
                pci_device_t* dev_obj = pci_device_new(bus, slot, function);

                if ( dev_obj == 0 ) {
                    continue;
                }

                printf("PCI Device %x %x %d %x\n", dev_obj->vendor, dev_obj->device, dev_obj->header_type, ((pci_device_normal_t*)dev_obj->extra)->baseaddr_0);


                pci_devlist_add(devlist, dev_obj);
            }
        }
    }
}

pci_devlist_t* pci_devlist_new(int max) {
    pci_devlist_t* list = (pci_devlist_t*) malloc(sizeof(pci_devlist_t));

    list->devices = (pci_device_t**) malloc(sizeof(pci_device_t*) * max);
    list->size = 0;
    list->max = max;

    return list;
}

void pci_devlist_add(pci_devlist_t* list, pci_device_t* device) {
    if ( list->size >= list->max ) {
        puts("ERROR: Max PCI devices hit!");
        return;
    }

    list->devices[list->size] = device;
    list->size++;

    device->dev_id = list->size - 1;
}

void pci_run_driver(int class, int subclass, void (*handle)(pci_device_t*)) {
    for ( int i = 0; i < devlist->size; i++ ) {
        pci_device_t* dev = devlist->devices[i];

        if ( dev->class == class && dev->subclass == subclass ) {
            handle(dev);
        }
    }
}

void pci_install() {
    devlist = pci_devlist_new(PCI_MAXDEVS);
    pci_probe();
}
