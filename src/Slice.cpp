#include "Slice.hpp"
#include "Packet.hpp"

Slice::Slice(int id, std::istream& in) : id(id)
{
    in >> count >> bandwidth >> max_delay;

    for(int i = 0; i < count; ++i)
    {
        packets.emplace_back(std::make_shared<Packet>(i, this, in));
    }
}

std::shared_ptr<Packet> Slice::next()
{
    for(auto& packet: packets)
    {
        if(packet->get_leave() == -1)
        {
            return packet;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Packet>>& Slice::get_packets()
{
    return packets;
}

int Slice::get_id() const
{
    return id;
}

int Slice::get_biggest_delay() const
{
    int biggest_delay = 0;

    for(auto& packet: packets)
    {
        if(packet->get_delay() > biggest_delay)
        {
            biggest_delay = packet->get_delay();
        }
    }

    return biggest_delay;
}

int Slice::total_size() const
{
    int total = 0;
    for(auto& packet: packets)
    {
        total += packet->get_size();
    }
    return total;
}

int Slice::deadline_time() const
{
    return total_size() / (0.95 * bandwidth) - packets[0]->get_arrival();
}

int Slice::get_max_delay() const
{
    return max_delay;
}

float Slice::get_bandwidth() const
{
    return bandwidth;
}