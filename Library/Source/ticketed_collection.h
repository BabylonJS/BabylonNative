#pragma once

#include <map>

namespace Babylon
{
    // NOTE: This type is not thread-safe.
    template<typename T>
    class ticketed_collection
    {
        using MapT = std::map<bool*, T>;

    public:
        class ticket
        {
        public:
            ticket(const ticket&) = delete;

            ticket(ticket&& other)
                : m_isCollectionStillAlive{ other.m_isCollectionStillAlive }
                , m_collection{ other.m_collection }
            {
                other.m_isCollectionStillAlive = nullptr;
            }

            ~ticket()
            {
                if (m_isCollectionStillAlive != nullptr)
                {
                    if (*m_isCollectionStillAlive)
                    {
                        m_collection.erase(m_isCollectionStillAlive);
                    }

                    delete m_isCollectionStillAlive;
                }
            }

        private:
            friend class ticketed_collection;

            ticket(T&& value, MapT& collection)
                : m_isCollectionStillAlive{ new bool{true} }
                , m_collection{ collection }
            {
                m_collection[m_isCollectionStillAlive] = std::move(value);
            }

            bool* m_isCollectionStillAlive{};
            MapT& m_collection;
        };

        ticketed_collection() = default;
        ticketed_collection(const ticketed_collection&) = delete;
        ticketed_collection(ticketed_collection&&) = delete;

        ~ticketed_collection()
        {
            clear();
        }

        ticket insert(T&& value)
        {
            return{ std::move(value), m_map };
        }

        template<typename CallableT>
        void apply_to_all(CallableT callable)
        {
            for (auto& [ptr, value] : m_map)
            {
                callable(value);
            }
        }

        void clear()
        {
            for (auto& [ptr, value] : m_map)
            {
                *ptr = false;
            }

            m_map.clear();
        }

    private:
        MapT m_map{};
    };
}
