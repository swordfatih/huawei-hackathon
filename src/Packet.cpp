#include "Packet.hpp"
#include "Slice.hpp" // Include Slice for the constructor

Packet::Packet(int id, Slice* slice, std::istream& in) : id(id), slice(slice)
{
    in >> arrival >> size;
}

int Packet::get_arrival() const
{
    return arrival;
}

void Packet::set_leave(int leave)
{
    this->leave = leave;
}

int Packet::get_size() const
{
    return size;
}

int Packet::get_leave() const
{
    return leave;
}

int Packet::get_id() const
{
    return id;
}

Slice* Packet::get_slice()
{
    return slice;
}

int Packet::get_delay() const
{
    return leave == -1 ? -1 : leave - arrival;
}
