#include <pci.h>

pci_devlist_t* devlist;

uint16_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    outportl(0xCF8, address);

    return (uint16_t)((inportl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

// uint16_t pci_checkclass(uint16_t bus, uint16_t slot, uint16_t function) {
//     return (pci_config_read(bus, device, function, 0xA) & ~0x00FF) >> 8;
// }
//
// uint16_t pci_checksubclass(uint16_t bus, uint16_t slot, uint16_t function) {
//     return (pci_config_read(bus, device, function, 0xA);
// }

pci_device_t* pci_device_new(uint16_t bus, uint16_t slot, uint16_t function) {
    uint16_t vendor = pci_config_read(bus, slot, function, 0);

    if ( vendor == 0xFFFF ) {
        return 0;
    }

    pci_device_t* dev_obj = (pci_device_t*) malloc(sizeof(pci_device_t));

    uint16_t device = pci_config_read(bus, device, function, 2);

    dev_obj->bus = bus;
    dev_obj->slot = slot;
    dev_obj->function = function;
    dev_obj->vendor = vendor;
    dev_obj->device = device;

    dev_obj->command = pci_config_read(bus, device, function, 4);
    dev_obj->status = pci_config_read(bus, device, function, 6);

    dev_obj->revision = ( pci_config_read(bus, device, function, 8) & 0x00FF );
    dev_obj->prog_if = ( pci_config_read(bus, device, function, 8) & ~0x00FF ) >> 8;
    dev_obj->subclass = ( pci_config_read(bus, device, function, 10) & 0x00FF );
    dev_obj->class = (pci_config_read(bus, device, function, 10) & ~0x00FF) >> 8;

    dev_obj->cacheline_size = ( pci_config_read(bus, device, function, 12) & 0x00FF );
    dev_obj->latency_timer = ( pci_config_read(bus, device, function, 12) & ~0x00FF ) >> 8;
    dev_obj->header_type = pci_config_read(bus, device, function, 14) & 0x00FF;
    dev_obj->bist = (pci_config_read(bus, device, function, 14) & ~0x00FF) >> 8;

    switch ( dev_obj->header_type ) {
        case 0: ;
            // Normal

            pci_device_normal_t* extra = (pci_device_normal_t*) malloc(sizeof(pci_device_normal_t));

            extra->baseaddr_0 = (pci_config_read(bus, device, function, 0x10) << 8) & pci_config_read(bus, device, function, 0x12);
            extra->baseaddr_1 = (pci_config_read(bus, device, function, 0x14) << 8) & pci_config_read(bus, device, function, 0x16);
            extra->baseaddr_2 = (pci_config_read(bus, device, function, 0x18) << 8) & pci_config_read(bus, device, function, 0x18);
            extra->baseaddr_3 = (pci_config_read(bus, device, function, 0x1C) << 8) & pci_config_read(bus, device, function, 0x1E);
            extra->baseaddr_4 = (pci_config_read(bus, device, function, 0x20) << 8) & pci_config_read(bus, device, function, 0x22);
            extra->baseaddr_5 = (pci_config_read(bus, device, function, 0x24) << 8) & pci_config_read(bus, device, function, 0x26);
            extra->cardbus_cis = (pci_config_read(bus, device, function, 0x28) << 8) & pci_config_read(bus, device, function, 0x2A);

            extra->subsystem_vendor = pci_config_read(bus, device, function, 0x2C);
            extra->subsystem = pci_config_read(bus, device, function, 0x2E);

            extra->expansion_rom = (pci_config_read(bus, device, function, 0x30) << 8) & pci_config_read(bus, device, function, 0x32);

            extra->capability = pci_config_read(bus, device, function, 0x34) & 0xFF;

            extra->interrupt_line = pci_config_read(bus, device, function, 0x3C) & 0xFF;
            extra->interrupt_pin = pci_config_read(bus, device, function, 0x3D) & 0xFF;
            extra->min_grant = pci_config_read(bus, device, function, 0x3E) & 0xFF;
            extra->max_latency = pci_config_read(bus, device, function, 0x3F) & 0xFF;

            break;

        case 1: ;
            // PCI-PCI bridge
            break;

        case 2: ;
            // PCI-to-CardBus
            break;

        default: ;
            puts("ERROR: Unable to identify PCI device!\n");

            free(dev_obj);
            return 0;
    }
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

                puts("PCI THINGY ADDDED\n");

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

void pci_install() {
    devlist = pci_devlist_new(PCI_MAXDEVS);

    pci_probe();
}
