#include "Scheduler.hpp"
#include "Packet.hpp"
#include "Slice.hpp"

Scheduler::Scheduler(std::istream& in)
{
    in >> slice_count >> port_bandwidth;

    for(int i = 0; i < slice_count; ++i)
    {
        slices.emplace_back(std::make_shared<Slice>(i, in));
    }
}

std::vector<std::shared_ptr<Packet>> Scheduler::get_arrivals(std::queue<std::shared_ptr<Packet>>& input, int time)
{
    std::vector<std::shared_ptr<Packet>> arrivals;

    while(!input.empty() && input.front()->get_arrival() <= time)
    {
        arrivals.emplace_back(input.front());
        input.pop();
    }

    return arrivals;
}

void Scheduler::prioritize(std::vector<std::shared_ptr<Packet>>& packets)
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

void Scheduler::send(int& time, std::vector<std::shared_ptr<Packet>>& packets)
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

std::vector<std::shared_ptr<Packet>> Scheduler::get_all_packets()
{
    std::vector<std::shared_ptr<Packet>> packets;

    for(auto& slice: slices)
    {
        packets.insert(packets.end(), slice->get_packets().begin(), slice->get_packets().end());
    }

    std::sort(packets.begin(), packets.end(), [](const std::shared_ptr<Packet>& a, const std::shared_ptr<Packet>& b)
    {
        return a->get_arrival() < b->get_arrival();
    });

    return packets;
}

void Scheduler::schedule()
{
    std::queue<std::shared_ptr<Packet>> input;

    for(auto& packet: get_all_packets())
    {
        input.push(packet);
    }

    int time = 0;
    while(!input.empty())
    {
        if(auto arrivals = get_arrivals(input, time); arrivals.empty())
        {
            time = input.front()->get_arrival();
        }
        else
        {
            prioritize(arrivals);
            send(time, arrivals);
        }
    }
}

std::string Scheduler::to_string() const
{
    std::string str = std::to_string(sequence.size()) + "\n";

    for(auto& packet: sequence)
    {
        str += std::to_string(packet->get_leave()) + " " + std::to_string(packet->get_slice()->get_id()) + " " + std::to_string(packet->get_id()) + " ";
    }

    str.pop_back();
    return str;
}

// Check if algorithm is correctly sending messages
bool Scheduler::check_sequence() const
{
    int last_slice_id = -1;
    int last_leave_time = -1;

    for(const auto& packet: sequence)
    {
        int current_leave_time = packet->get_leave();
        int current_arrival_time = packet->get_arrival();
        int current_slice_id = packet->get_slice()->get_id();

        // Check if the leaving time is less than the arrival time
        if(current_leave_time < current_arrival_time)
        {
            std::cerr << "\033[1;33m" << "\t[Early leave] Packet " << packet->get_id() << " from slice " << current_slice_id
                      << " has a leave time less than its arrival time." << "\033[0m" << std::endl;

            return false;
        }

        // Check if packets from the same slice leave in the order they arrived
        if(current_slice_id == last_slice_id)
        {
            if(current_leave_time < last_leave_time)
            {
                std::cerr << "\033[1;33m" << "\t[Order] Packet " << packet->get_id() << " from slice " << current_slice_id
                          << " leaves out of order." << "\033[0m" << std::endl;
                return false;
            }
        }

        // Update the last leave time and last slice ID
        last_leave_time = current_leave_time;
        last_slice_id = current_slice_id;
    }

    return true; // The sequence is valid
}

bool Scheduler::check_bandwidth() const
{
    for(const auto& slice: slices)
    {
        int   slice_size = slice->total_size();
        int   first_arrival = slice->get_packets()[0]->get_arrival();
        int   last_leave = slice->get_packets().back()->get_leave();
        float slice_bandwidth = slice->get_bandwidth();
        float delay = last_leave - first_arrival;

        if(slice_size / delay < (0.95 * slice_bandwidth))
        {
            std::cerr << "\033[1;33m" << "\t[Bandwidth] Slice " << slice->get_id() << " has a bandwidth violation." << "\033[0m" << std::endl;
            return false;
        }
    }

    return true;
}

float Scheduler::calculate_score() const
{
    float score = 0.0f;
    float biggest_delay = 0.0f;

    for(const auto& slice: slices)
    {
        biggest_delay = std::max(biggest_delay, static_cast<float>(slice->get_biggest_delay()));
    }

    for(const auto& slice: slices)
    {
        float f = slice->get_biggest_delay() <= slice->get_max_delay() ? 1.0f : 0.0f;
        score += f / slice_count;
    }

    score += 10000.f / biggest_delay;

    return score;
}