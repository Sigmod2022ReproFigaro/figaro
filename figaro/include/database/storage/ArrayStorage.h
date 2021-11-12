#ifndef _FIGARO_ARRAY_STORAGE_H_
#define _FIGARO_ARRAY_STORAGE_H_

#include <boost/align/aligned_alloc.hpp>

namespace Figaro
{
    template <typename T>
    class ArrayStorage
    {
        uint64_t m_size = 0;
        T* m_data = nullptr;
        void destroyData(void)
        {
            if (nullptr != m_data)
            {
                boost::alignment::aligned_free(m_data);
            }
        }
    public:

        static constexpr uint64_t CACHE_LINE_SIZE = 64;
        ArrayStorage(uint64_t size): m_size(size)
        {
            if (size > 0)
            {
                m_data = (T*)boost::alignment::aligned_alloc(
                    CACHE_LINE_SIZE, size * sizeof(T));
            }
        }

        T& operator[](uint64_t idx)
        {
            FIGARO_LOG_ASSERT(idx < m_size);
            return m_data[idx];
        }

        const T& operator[](uint64_t idx) const
        {
            FIGARO_LOG_ASSERT(idx < m_size);
            return m_data[idx];
        }

        ~ArrayStorage()
        {
            destroyData();
        }

        uint64_t getSize(void) const
        {
            return m_size;
        }

        T* getData(void)
        {
            return m_data;
        }

        void resize(uint64_t newSize)
        {
            T* newData = nullptr;
            if (newSize > 0)
            {
                newData = (T*)boost::alignment::aligned_alloc(
                    CACHE_LINE_SIZE, newSize * sizeof(T));
            }

            if ((m_size > 0) && (newSize > 0))
            {
                memcpy(newData, m_data, sizeof(T) * std::min(newSize, m_size));
            }

            destroyData();
            m_size = newSize;
            m_data = newData;
        }

        void setToZeros(void)
        {
            memset(m_data, 0, sizeof(T) * m_size);
        }
    };
}

#endif