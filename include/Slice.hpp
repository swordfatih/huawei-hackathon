#ifndef SLICE_HPP
#define SLICE_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

class Packet; // Forward declaration

class Slice {
public:
    Slice(int id, std::istream& in);

    std::shared_ptr<Packet> next();
    std::vector<std::shared_ptr<Packet>>& get_packets();
    int get_id() const;
    int get_biggest_delay() const;
    int total_size() const;
    int deadline_time() const;

private:
    int id;                                  ///< slice id
    int count;                               ///< number of slice packets
    float bandwidth;                         ///< slice bandwidth (SliceBWi)
    int max_delay;                           ///< maximum slice delay tolerance (UBDi)
    std::vector<std::shared_ptr<Packet>> packets; ///< slice packets
};

#endif // SLICE_HPP
