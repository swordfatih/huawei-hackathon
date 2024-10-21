#ifndef PACKET_HPP
#define PACKET_HPP

#include <iostream>
#include <memory>

class Slice; 

class Packet {
public:
    Packet(int id, Slice* slice, std::istream& in);

    int get_arrival() const;
    void set_leave(int leave);
    int get_size() const;
    int get_leave() const;
    int get_id() const;
    Slice* get_slice();
    int get_delay() const;

private:
    Slice* slice;      ///< slice
    int id;           ///< packet id
    int size;         ///< packet size (PktSize)
    int arrival;      ///< arrival of the last bit of the packet (ts)
    int leave = -1;   ///< leave time of the first bit of the packet (te)
};

#endif // PACKET_HPP
