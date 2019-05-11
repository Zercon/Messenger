#pragma once
#include <cstdint>
#include <memory>

class Packet
{
public:
    Packet()
    {
        buffer_ = (uint8_t*)malloc(1024);
        length_ = 0;
        size_ = 1024;
    }

    Packet(uint8_t cod, uint32_t length, uint16_t ident, uint16_t item_count = 0)
    {
        buffer_ = (uint8_t*)malloc(1024);
        buffer_[0] = cod;
        *((uint32_t*)(buffer_ + 1)) = length;
        *((uint16_t*)(buffer_ + 5)) = ident;
        *((uint16_t*)(buffer_ + 7)) = item_count;
        length_ = 9;
        size_ = 1024;
    }

    ~Packet()
    {
        if (buffer_ != NULL)
        {
            delete[] buffer_;
        }
    }

    void init(uint8_t cod, uint32_t length, uint16_t ident, uint16_t item_count = 0)
    {
        buffer_[0] = cod;
        *((uint32_t*)(buffer_ + 1)) = length;
        *((uint16_t*)(buffer_ + 5)) = ident;
        *((uint16_t*)(buffer_ + 7)) = item_count;
        length_ = 9;
    }

    uint8_t* data() const { return buffer_; }

    uint8_t* payload() const { return buffer_ + length_; }

    bool resize(uint32_t size)
    {
        if (size < 7 || length_ < size)
        {
            return false;
        }
        uint8_t* new_buffer = (uint8_t*)realloc(buffer_, size);
        if (new_buffer != NULL)
        {
            buffer_ = new_buffer;
            size_ = size_;
            printf("DEBUG PACKET: Packet was resized to %d bytes.\n", size_);
            return false;
        }
        else
        {
            printf("DEBUG PACKET: Packet was not resized!\n");
            return false;
        }
        return false;
    }

    uint32_t getLength() const { return length_; }

    uint32_t getSize() const { return size_; }

    uint8_t getCod() const { return *buffer_; }

    uint16_t getItemCount() const { return *((uint16_t*)(buffer_ + 7)); }

    void setItemCount(uint16_t item_count)
    {
        if (item_count > 0 && size_ > 7)
        {
            *((uint16_t*)(buffer_ + 7)) = item_count;
            length_ += 2;
        }
    }

    void makeStringItem(uint8_t cod, const char* data, uint32_t len)
    {
        if (size_ > length_ + len + 5)
        {
            if (length_ < 9)
            {
                printf("ERROR PACKET: are you sure that item count was setted?\n");
                return;
            }
            *((uint8_t*)(buffer_ + length_)) = cod;
            length_ += 1;
            *((uint32_t*)(buffer_ + length_)) = len + 5;
            length_ += 4;
            std::memcpy(buffer_ + length_, data, len);
            length_ += len;
            incrementItemCount();
        }
        else
        {
            printf("ERROR PACKET: not enough space for make string item! Length: %d, size: %d\n", length_, size_);
        }
    }

    void makeCodItem(uint8_t cod, uint8_t codItem)
    {
        if (size_ > length_ + 6)
        {
            if (length_ < 9)
            {
                printf("ERROR PACKET: are you sure that item count was setted?\n");
                return;
            }
            *((uint8_t*)(buffer_ + length_)) = cod;
            length_ += 1;
            *((uint32_t*)(buffer_ + length_)) = 6;
            length_ += 4;
            *((uint8_t*)(buffer_ + length_)) = codItem;
            length_ += 1;
            incrementItemCount();
        }
        else
        {
            printf("ERROR PACKET: not enough space for make cod item! Length: %d, size: %d\n", length_, size_);
        }
    }

private:

    void incrementItemCount()
    {
        *((uint16_t*)(buffer_ + 7)) += 1;
    }

    uint32_t length_;
    uint32_t size_;
    uint8_t* buffer_;
};