#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Packet
{
public:
    Packet(int id, int slice, std::istream& in) : id(id), slice(slice)
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

    int get_slice() const
    {
        return slice;
    }

private:
    int slice;     ///< slice id
    int id;        ///< packet id
    int size;      ///< packet size (PktSize)
    int arrival;   ///< arrival of the last bit of the packet (ts)
    int leave = 0; ///< leave time of the first bit of the packet (te)
};

class Slice
{
public:
    Slice(int id, std::istream& in) : id(id)
    {
        in >> count >> bandwidth >> max_delay;

        for(int i = 0; i < count; ++i)
        {
            packets.emplace_back(std::make_shared<Packet>(i, id, in));
        }
    }

    int get_bandwidth() const
    {
        return bandwidth;
    }

    std::shared_ptr<Packet> get_packet(int i)
    {
        return packets[i];
    }

    std::shared_ptr<Packet> next(int round, int time)
    {
        if(packets.empty() || round >= packets.size() || time < packets[round]->get_arrival())
        {
            return nullptr;
        }

        return packets[round];
    }

    int get_id() const
    {
        return id;
    }

private:
    int                                  id;        ///< slice id
    int                                  count;     ///< number of slice packets
    int                                  bandwidth; ///< slice bandwidth (SliceBWi)
    int                                  max_delay; ///< maximum slice delay tolerance (UBDi)
    std::vector<std::shared_ptr<Packet>> packets;   ///< slice packets
};

class Scheduler
{
public:
    Scheduler(std::istream& in)
    {
        in >> slice_count >> port_bandwidth;

        for(int i = 0; i < slice_count; ++i)
        {
            slices.emplace_back(i, in);
        }
    }

    void schedule()
    {
        int time = 0, round = 0, count = 0;

        while(true)
        {
            bool added = false;
            for(auto& slice: slices)
            {
                auto packet = slice.next(round, time);

                if(packet)
                {
                    auto duration = packet->get_size() / port_bandwidth;

                    packet->set_leave(time);
                    time += duration;

                    sequence.emplace_back(packet);
                    added = true;
                }
            }

            if(!added)
            {
                break;
            }

            round++;
        }
    }

    int get_bandwidth() const
    {
        return port_bandwidth;
    }

    std::string to_string() const
    {
        std::string str = std::to_string(sequence.size()) + "\n";

        for(auto& packet: sequence)
        {
            str += std::to_string(packet->get_leave()) + " " + std::to_string(slices[packet->get_slice()].get_id()) + " " + std::to_string(packet->get_id()) + " ";
        }

        str.pop_back();

        return str;
    }

private:
    int                                  slice_count = 0;    ///< number of slice users
    int                                  port_bandwidth = 0; ///< port bandwidth (PortBW)
    std::vector<Slice>                   slices;             ///< slices
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