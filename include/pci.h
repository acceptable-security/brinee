#include <stdlib.h>

// Struct to hold the PCI device information.

// These ones are for the special information for each type.
struct pci_device_normal {
    uint32_t baseaddr_0;
    uint32_t baseaddr_1;
    uint32_t baseaddr_2;
    uint32_t baseaddr_3;
    uint32_t baseaddr_4;
    uint32_t baseaddr_5;

    uint32_t cardbus_cis;

    uint16_t subsystem;
    uint16_t subsystem_vendor;

    uint32_t expansion_rom;

    uint8_t capability;

    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
};

typedef struct pci_device_normal pci_device_normal_t;

struct pci_device_pcipci {
    uint32_t baseaddr_0;
    uint32_t baseaddr_1;

    uint8_t primary_bus;
    uint8_t secondary_bus;
    uint8_t subordinate_bus;
    uint8_t secondary_latency;

    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;

    uint16_t memory_base;
    uint16_t memory_limit;

    uint16_t prefetch_mem_base;
    uint16_t prefetch_mem_limit;

    uint16_t prefetch_base;
    uint16_t prefetch_limit;

    uint16_t io_upperbase;
    uint16_t io_upperlimit;

    uint16_t capability;
    uint16_t expansion_rom;

    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
};

typedef struct pci_device_pcipci pci_device_pcipci_t;

struct pci_device {
    // For storage. idk why but just keep it I guess :^)
    uint16_t bus;
    uint16_t slot;
    uint16_t function;

    // Universal for all headertypes.
    uint16_t vendor;
    uint16_t device;

    uint16_t status;
    uint16_t command;

    uint8_t class;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t revision;

    uint8_t bist;
    uint8_t header_type;
    uint8_t latency_timer;
    uint8_t cacheline_size;

    void* extra; // For special devices. Look above.

    // Asigned by device list.
    int dev_id;
};

typedef struct pci_device pci_device_t;

// Struct to hold a list of PCI devices.

#define PCI_MAXDEVS 64 // Temporary solution to a problem.

struct pci_devlist {
    pci_device_t** devices;
    int size;
    int max;
};

typedef struct pci_devlist pci_devlist_t;

void pci_install();
