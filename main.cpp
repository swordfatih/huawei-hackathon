#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Packet
{
public:
    Packet(int id, std::istream& in) : id(id)
    {
        in >> size >> arrival;
    }

    std::string to_string() const
    {
        return "[Packet #" + std::to_string(id) + "] size: " + std::to_string(size) + ", arrival: " + std::to_string(arrival);
    }

private:
    int id;      ///< packet id
    int size;    ///< packet size (PktSize)
    int arrival; ///< arrival of the last bit of the packet (ts)
    int leave;   ///< leave time of the first bit of the packet (te)
};

class Slice
{
public:
    Slice(int id, std::istream& in) : id(id)
    {
        in >> count >> bandwidth >> max_delay;

        for(int i = 0; i < count; ++i)
        {
            sequence.emplace_back(i, in);
        }
    }

    std::string to_string() const
    {
        std::string str = "[Slice #" + std::to_string(id) + "] count: " + std::to_string(count) + ", bandwidth: " + std::to_string(bandwidth) + ", max_delay: " + std::to_string(max_delay) + "\n";
        for(const Packet& packet: sequence)
        {
            str += packet.to_string() + "\n";
        }
        return str;
    }

private:
    int                 id;        ///< slice id
    int                 count;     ///< number of slice packets
    int                 bandwidth; ///< slice bandwidth (SliceBWi)
    int                 max_delay; ///< maximum slice delay tolerance (UBDi)
    std::vector<Packet> sequence;  ///< slice packets
};

class Scheduler
{
public:
    Scheduler(std::istream& in)
    {
        in >> users_count >> port_bandwidth;

        for(int i = 0; i < users_count; ++i)
        {
            slices.emplace_back(i, in);
        }
    }

    void schedule(std::ostream& out)
    {
        out << "[Scheduler] scheduling\n";
    }

    std::string to_string() const
    {
        std::string str = "[Scheduler] users_count: " + std::to_string(users_count) + ", port_bandwidth: " + std::to_string(port_bandwidth) + "\n\n";
        for(const Slice& slice: slices)
        {
            str += slice.to_string() + "\n";
        }
        return str;
    }

private:
    int                users_count = 0;    ///< number of slice users
    int                port_bandwidth = 0; ///< port bandwidth (PortBW)
    std::vector<Slice> slices;             ///< slice sequence
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

    std::istream& in = input_file ? input_file : std::cin;
    std::ostream& out = output_file ? output_file : std::cout;

    Scheduler scheduler(in);

    out << scheduler.to_string();

    scheduler.schedule(out);

    return 0;
}