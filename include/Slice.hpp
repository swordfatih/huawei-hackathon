#ifndef SLICE_HPP
#define SLICE_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Packet; // Forward declaration

class Slice
{
public:
    Slice(int id, std::istream& in);

    std::shared_ptr<Packet>               next();
    std::vector<std::shared_ptr<Packet>>& get_packets();
    int                                   get_id() const;
    int                                   get_biggest_delay() const;
    int                                   total_size() const;
    int                                   deadline_time() const;
    int                                   get_max_delay() const;
    float                                 get_bandwidth() const;

private:
    int                                  id;        ///< slice id
    int                                  count;     ///< number of slice packets
    float                                bandwidth; ///< slice bandwidth (SliceBWi)
    int                                  max_delay; ///< maximum slice delay tolerance (UBDi)
    std::vector<std::shared_ptr<Packet>> packets;   ///< slice packets
};

#endif // SLICE_HPP
