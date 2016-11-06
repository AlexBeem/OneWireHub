// (0x0F) 64kbit EEPROM, Add Only Memory

#ifndef ONEWIRE_DS2506_H
#define ONEWIRE_DS2506_H

#include "OneWireItem.h"

class DS2506 : public OneWireItem
{
private:

    // Problem: atmega has 2kb RAM, this IC offers 8kb
    // Solution: out-of-bound-memory will be constant 0xFF, same for the depending status-registers
    static constexpr uint16_t MEM_SIZE_PROPOSE  = 256; // TUNE HERE! Give this device as much RAM as your CPU can spare

    static constexpr uint8_t  PAGE_SIZE         = 32;
    static constexpr uint16_t PAGE_COUNT        = MEM_SIZE_PROPOSE / PAGE_SIZE;
    static constexpr uint8_t  PAGE_MASK         = 0b00011111;

    static constexpr uint16_t MEM_SIZE          = PAGE_COUNT * PAGE_SIZE;
    static constexpr uint16_t MEM_MASK          = MEM_SIZE - 1;

    static constexpr uint8_t  STATUS_SEGMENT    = (PAGE_COUNT/8);
    static constexpr uint16_t STATUS_SIZE       = PAGE_COUNT + (3*STATUS_SEGMENT);
    static constexpr uint16_t STATUS_SIZE_DEV   = 0x200; // device specific "real" size

    static constexpr uint8_t  STATUS_WP_PAGES_BEG   {0x00};  // 32  bytes -> Page write protection
    static constexpr uint8_t  STATUS_WP_REDIR_BEG   {0x20};  // 32  bytes -> Redirection write protection
    static constexpr uint8_t  STATUS_PG_WRITN_BEG   {0x40};  // 32  bytes -> Page used status (written ones)
    static constexpr uint8_t  STATUS_UNDEF_B1_BEG   {0x60};
    static constexpr uint16_t STATUS_PG_REDIR_BEG   {0x100}; // 256 bytes -> Redirection to page, 0xFF if valid, ones complement (xFD is page 2)
    static constexpr uint16_t STATUS_UNDEF_B2_BEG   {0x200};

    static_assert(MEM_SIZE > 255,       "REAL MEM SIZE IS TOO SMALL");
    static_assert(STATUS_SEGMENT > 0,   "REAL MEM SIZE IS TOO SMALL");
    static_assert(MEM_SIZE <= 8192,     "REAL MEM SIZE IS TOO BIG, MAX IS 8291 bytes");

    uint8_t     memory[MEM_SIZE];    // 4 pages of 32 bytes
    uint8_t     status[STATUS_SIZE]; // eprom status bytes

    uint16_t    sizeof_memory;              // device specific "real" size
    uint16_t    page_count, status_segment; // device specific "real" size


    uint16_t translateRedirection(const uint16_t source_address) const; // react to redirection in status and not available memory

public:
    static constexpr uint8_t family_code = 0x0F; // the ds2506

    DS2506(uint8_t ID1, uint8_t ID2, uint8_t ID3, uint8_t ID4, uint8_t ID5, uint8_t ID6, uint8_t ID7);

    void    duty(OneWireHub * const hub);

    void    clearMemory(void);
    void    clearStatus(void);

    bool    writeMemory(const uint8_t* const source, const uint16_t length, const uint16_t position = 0);
    bool    readMemory(uint8_t* const destination, const uint16_t length, const uint16_t position = 0) const;

    uint8_t writeStatus(const uint16_t address, const uint8_t value);
    uint8_t readStatus(const uint16_t address) const;

    void    setPageProtection(const uint8_t page);
    bool    getPageProtection(const uint8_t page) const;

    void    setRedirectionProtection(const uint8_t page);
    bool    getRedirectionProtection(const uint8_t page) const;

    void    setPageUsed(const uint8_t page);
    bool    getPageUsed(const uint8_t page) const;

    bool    setPageRedirection(const uint8_t page_source, const uint8_t page_destin);
    uint8_t getPageRedirection(const uint8_t page) const;
};

#endif
