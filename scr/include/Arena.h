#pragma once

class ArenaAllocator{

    public:

        inline explicit ArenaAllocator(size_t bytes) : size(bytes)
        {
            buffer = static_cast<std::byte*>(malloc(size));
            offset = buffer;
        }

        template<typename T>
        inline T* alloc()
        {
            void* p_offset = offset;
        
            offset += sizeof(T);

            return p_offset;
        }

        inline ArenaAllocator(const ArenaAllocator& other) = delete;
        inline ArenaAllocator operator=(const ArenaAllocator& other) = delete;

        inline ~ArenaAllocator()
        {
            free(buffer);
        }

    private:

        size_t size;
        std::byte* buffer;
        std::byte* offset;

};