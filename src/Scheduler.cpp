#include "Scheduler.hpp"
#include "Slice.hpp"
#include "Packet.hpp"

Scheduler::Scheduler(std::istream& in) {
    in >> slice_count >> port_bandwidth;

    for (int i = 0; i < slice_count; ++i) {
        slices.emplace_back(std::make_shared<Slice>(i, in));
    }
}

std::vector<std::shared_ptr<Packet>> Scheduler::get_arrivals() {
    std::vector<std::shared_ptr<Packet>> arrivals;

    for(auto& slice : slices) {
        auto packet = slice->next();

        if(packet) {
            arrivals.emplace_back(packet);
        }
    }

    return arrivals;
}

void Scheduler::prioritize(std::vector<std::shared_ptr<Packet>>& packets) {
    std::sort(packets.begin(), packets.end(), [](const std::shared_ptr<Packet>& a, const std::shared_ptr<Packet>& b) {
        if(a->get_arrival() == b->get_arrival()) {
            if(a->get_slice()->deadline_time() == b->get_slice()->deadline_time()) {
                return a->get_slice()->get_biggest_delay() > b->get_slice()->get_biggest_delay();
            }
            return a->get_slice()->deadline_time() < b->get_slice()->deadline_time();
        }
        return a->get_arrival() < b->get_arrival();
    });
}

void Scheduler::send(int& time, std::vector<std::shared_ptr<Packet>>& packets) {
    time = std::max(time, packets[0]->get_arrival());

    for(auto& packet : packets) {
        if(packet->get_arrival() <= time) {
            packet->set_leave(time);
            time += packet->get_size() / port_bandwidth;
            sequence.emplace_back(packet);
        }
    }
}

std::vector<std::shared_ptr<Packet>> Scheduler::get_all_packets() {
    std::vector<std::shared_ptr<Packet>> packets;

    for(auto& slice : slices) {
        packets.insert(packets.end(), slice->get_packets().begin(), slice->get_packets().end());
    }

    return packets;
}

void Scheduler::schedule() {
    std::vector<std::shared_ptr<Packet>> all = get_all_packets();
    
    int time = 0, count = 0;

    while(sequence.size() < all.size()) {
        auto arrivals = get_arrivals();
        prioritize(arrivals);
        send(time, arrivals);
    }
}

std::string Scheduler::to_string() const {
    std::string str = std::to_string(sequence.size()) + "\n";

    for(auto& packet : sequence) {
        str += std::to_string(packet->get_leave()) + " " + std::to_string(packet->get_slice()->get_id()) + " " + std::to_string(packet->get_id()) + " ";
    }

    str.pop_back();
    return str;
}

//Check if algorithm is correctly sending messages
bool Scheduler::check_sequence() const {
    int last_slice_id = -1;
    int last_leave_time = -1;

    for (const auto& packet : sequence) {
        int current_leave_time = packet->get_leave();
        int current_arrival_time = packet->get_arrival();
        int current_slice_id = packet->get_slice()->get_id();

        // Check if the leaving time is less than the arrival time
        if (current_leave_time < current_arrival_time) {
            std::cerr << "Packet " << packet->get_id() << " from slice " << current_slice_id
                      << " has a leave time less than its arrival time." << std::endl;
            return false;
        }

        // Check if packets from the same slice leave in the order they arrived
        if (current_slice_id == last_slice_id) {
            if (current_leave_time < last_leave_time) {
                std::cerr << "Packet " << packet->get_id() << " from slice " << current_slice_id
                          << " leaves out of order." << std::endl;
                return false;
            }
        }

        // Update the last leave time and last slice ID
        last_leave_time = current_leave_time;
        last_slice_id = current_slice_id;
    }

    return true;  // The sequence is valid
}