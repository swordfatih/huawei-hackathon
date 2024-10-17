#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Slice;

class Packet
{
public:
    Packet(int id, Slice* slice, std::istream& in) : id(id), slice(slice)
    {
        in >> arrival >> size;
    }

    int get_arrival() const
    {
        return arrival;
    }

    void set_leave(int leave)
    {
        this->leave = leave;
    }

    int get_size() const
    {
        return size;
    }

    int get_leave() const
    {
        return leave;
    }

    int get_id() const
    {
        return id;
    }

    Slice* get_slice()
    {
        return slice;
    }

    int get_delay() const
    {
        return leave == -1 ? -1 : leave - arrival;
    }

private:
    Slice* slice;      ///< slice
    int    id;         ///< packet id
    int    size;       ///< packet size (PktSize)
    int    arrival;    ///< arrival of the last bit of the packet (ts)
    int    leave = -1; ///< leave time of the first bit of the packet (te)
};

class Slice
{
public:
    Slice(int id, std::istream& in) : id(id)
    {
        in >> count >> bandwidth >> max_delay;

        for(int i = 0; i < count; ++i)
        {
            packets.emplace_back(std::make_shared<Packet>(i, this, in));
        }
    }

    std::shared_ptr<Packet> next()
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

    std::vector<std::shared_ptr<Packet>>& get_packets()
    {
        return packets;
    }

    int get_id() const
    {
        return id;
    }

    int get_biggest_delay() const
    {
        auto element = std::max_element(packets.begin(), packets.end(), [](const std::shared_ptr<Packet>& a, const std::shared_ptr<Packet>& b)
        {
            return a->get_delay() < b->get_delay();
        });

        return (*element)->get_delay();
    }

    int total_size() const
    {
        int total = 0;
        for(auto& packet: packets)
        {
            total += packet->get_size();
        }

        return total;
    }

    int deadline_time() const
    {
        return total_size() / (0.95 * bandwidth) - packets[0]->get_arrival();
    }

private:
    int                                  id;             ///< slice id
    int                                  count;          ///< number of slice packets
    float                                bandwidth;      ///< slice bandwidth (SliceBWi)
    int                                  max_delay;      ///< maximum slice delay tolerance (UBDi)
    std::vector<std::shared_ptr<Packet>> packets;        ///< slice packets
};

class Scheduler
{
public:
    Scheduler(std::istream& in)
    {
        in >> slice_count >> port_bandwidth;

        for(int i = 0; i < slice_count; ++i)
        {
            slices.emplace_back(std::make_shared<Slice>(i, in));
        }
    }

    std::vector<std::shared_ptr<Packet>> get_arrivals()
    {
        std::vector<std::shared_ptr<Packet>> arrivals;

        for(auto& slice: slices)
        {
            auto packet = slice->next();

            if(packet)
            {
                arrivals.emplace_back(packet);
            }
        }

        return arrivals;
    }

    void prioritize(std::vector<std::shared_ptr<Packet>>& packets)
    {
        std::sort(packets.begin(), packets.end(), [](const std::shared_ptr<Packet>& a, const std::shared_ptr<Packet>& b)
        {
            if(a->get_arrival() == b->get_arrival())
            {
                if(a->get_slice()->deadline_time() == b->get_slice()->deadline_time())
                {
                    return a->get_slice()->get_biggest_delay() > b->get_slice()->get_biggest_delay();
                }

                return a->get_slice()->deadline_time() < b->get_slice()->deadline_time();
            }

            return a->get_arrival() < b->get_arrival();
        });
    }

    void send(int& time, std::vector<std::shared_ptr<Packet>>& packets)
    {
        time = std::max(time, packets[0]->get_arrival());

        for(auto& packet: packets)
        {
            if(packet->get_arrival() <= time)
            {
                packet->set_leave(time);

                time += packet->get_size() / port_bandwidth;

                sequence.emplace_back(packet);
            }
        }
    }

    std::vector<std::shared_ptr<Packet>> get_all_packets()
    {
        std::vector<std::shared_ptr<Packet>> packets;

        for(auto& slice: slices)
        {
            packets.insert(packets.end(), slice->get_packets().begin(), slice->get_packets().end());
        }

        return packets;
    }

    void schedule()
    {
        std::vector<std::shared_ptr<Packet>> all = get_all_packets();

        int time = 0, count = 0;

        while(sequence.size() < all.size())
        {
            auto arrivals = get_arrivals();
            prioritize(arrivals);
            send(time, arrivals);
        }
    }

    std::string to_string() const
    {
        std::string str = std::to_string(sequence.size()) + "\n";

        for(auto& packet: sequence)
        {
            str += std::to_string(packet->get_leave()) + " " + std::to_string(packet->get_slice()->get_id()) + " " + std::to_string(packet->get_id()) + " ";
        }

        str.pop_back();

        return str;
    }

private:
    int                                  slice_count = 0;    ///< number of slice users
    int                                  port_bandwidth = 0; ///< port bandwidth (PortBW)
    std::vector<std::shared_ptr<Slice>>  slices;             ///< slices
    std::vector<std::shared_ptr<Packet>> sequence;           ///< packet sequence
};

int main(int argc, char* argv[])
{
    // options : -i input_path -o output_path (standard io by default)
    std::ifstream input_file;
    std::ofstream output_file;
    for(int i = 1; i < argc; i += 2)
    {
        if(std::string(argv[i]) == "-i")
        {
            input_file.open(argv[i + 1]);
        }
        else if(std::string(argv[i]) == "-o")
        {
            output_file.open(argv[i + 1]);
        }
    }

    std::istream& in = input_file.is_open() ? input_file : std::cin;
    std::ostream& out = output_file.is_open() ? output_file : std::cout;

    Scheduler scheduler(in);
    scheduler.schedule();

    out << scheduler.to_string();

    return 0;
}