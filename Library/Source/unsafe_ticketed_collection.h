#pragma once

#include <map>

namespace Babylon
{
    template<typename T>
    class unsafe_ticketed_collection
    {
        using MapT = std::map<bool*, T>;

    public:
        class ticket
        {
        public:
            ~ticket()
            {
                if (m_isCollectionStillAlive != nullptr && *m_isCollectionStillAlive)
                {
                    m_collection.erase(m_isCollectionStillAlive);
                }

                delete m_isCollectionStillAlive;
            }

            ticket(const ticket&) = delete;
            
            ticket(ticket&& other)
                : m_isCollectionStillAlive{ other.m_isCollectionStillAlive }
                , m_collection{ other.m_collection }
            {
                other.m_isCollectionStillAlive = nullptr;
            }

        private:
            friend class unsafe_ticketed_collection;

            ticket(T&& value, MapT& collection)
                : m_isCollectionStillAlive{ new bool{true} }
                , m_collection{ collection }
            {
                m_collection[m_isCollectionStillAlive] = std::move(value);
            }

            bool* m_isCollectionStillAlive{};
            MapT& m_collection;
        };

        ~unsafe_ticketed_collection()
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
            for (auto& [ptr, T] : m_map)
            {
                callable(T);
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