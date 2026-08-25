#pragma once

#include <cstdint>
#include <string>
#include <vector>

class SFile;

// Common lifetime and raw-row storage shared by the generated record types.
class ClientDBRecord {
public:
    virtual ~ClientDBRecord() = default;

protected:
    unsigned char ReadRecord(SFile*, const char* stringBuffer) {
        m_stringBuffer = stringBuffer;
        return 1;
    }

    const char* m_stringBuffer = nullptr;
    std::vector<std::uint8_t> m_rawRow;
};
