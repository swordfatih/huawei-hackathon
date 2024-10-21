#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <iostream>
#include <memory>
#include <vector>

class Packet; 
class Slice; 

class Scheduler {
public:
    Scheduler(std::istream& in);
    
    std::vector<std::shared_ptr<Packet>> get_arrivals();
    void prioritize(std::vector<std::shared_ptr<Packet>>& packets);
    void send(int& time, std::vector<std::shared_ptr<Packet>>& packets);
    std::vector<std::shared_ptr<Packet>> get_all_packets();
    void schedule();
    std::string to_string() const;
    bool check_sequence() const;  // Add this line

private:
    int slice_count = 0;                     ///< number of slice users
    int port_bandwidth = 0;                  ///< port bandwidth (PortBW)
    std::vector<std::shared_ptr<Slice>> slices; ///< slices
    std::vector<std::shared_ptr<Packet>> sequence; ///< packet sequence
};

#endif // SCHEDULER_HPP
