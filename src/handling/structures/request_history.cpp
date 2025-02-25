#include <string>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <jsoncpp/json/json.h>
#include <queue>
#include "rate_structures.h"

namespace handler_structs {


static int fast_atoi(const char * str, int iters = -1)
{
    int val = 0;
    if (iters < 0) {
        while ( *str ) {
            val = val*10 + (*str++ - '0');
        }
        return val;
    }
    for (int i = 0; i < iters; i++ ) {
        val = val*10 + (*str++ - '0');
    }
    return val;
}

static void limitDurationExtraction(std::string_view header_strings, int iters, std::vector<int>& limits, std::vector<int>& durations) {
    int colon_index = header_strings.find(':');
    int limit = fast_atoi(header_strings.data(), colon_index);
    int duration = fast_atoi(header_strings.substr(colon_index+1).data(), iters - colon_index - 1);
    limits.push_back(limit);
    durations.push_back(duration);
}

static std::vector<ScopeHistory> init_method_hierachy(std::string_view method_limits, std::time_t server_time) {
    
    std::vector<ScopeHistory> method_hierachy;
    std::vector<int> limits;
    std::vector<int> durations;

    int comma_index = 0;
    int beginning_index = 0;

    while (comma_index != -1) {
        comma_index = method_limits.find(',', beginning_index);
        limitDurationExtraction(method_limits.substr(beginning_index), comma_index - beginning_index, limits, durations);
        beginning_index = comma_index + 1;
    }

    for (int i = 0; i < limits.size(); i++) {
        method_hierachy.push_back({durations.at(i), limits.at(i)});
        method_hierachy.back().insert_request(server_time);
    }

    return method_hierachy;
}

void ScopeHistory::update_history() {
    
    if (history.size() == 0) {return;};

    bool remove = true;
    const std::time_t c_time = std::time(NULL);
    std::time_t current_t = std::mktime(std::gmtime(&c_time));
    while (remove) {
        if (current_t - history.front() > duration) {
            history.pop();
            remove = !(history.empty());
        } else {
            return;
        }
    }
    return;
}

std::time_t ScopeHistory::validate_request() {
    update_history();

    if (history.size() < limit) {
        return 0;
    } else {
        const std::time_t c_time = std::time(NULL);
        return duration - static_cast<int>(std::mktime(std::gmtime(&c_time)) - history.front());
    }
}

void ScopeHistory::correct_history(int server_counter, int server_limit, int server_duration) {
    update_history();
    int residue = history.size() - server_counter;
    limit = server_limit;
    duration = server_duration;
    
    if (residue == 0) {
        return;
    } else if (residue > 0) {
        for (int i = 0; i < residue; i++) {
            history.pop();
        }
    } else {
        // log unrecorded requests!!
        const std::time_t c_time = std::time(NULL);
        std::time_t current = std::mktime(std::gmtime(&c_time));
        for (int i = 0; i < -residue; i++) {
            insert_request(current);
        }
    }
}

void RegionHistory::update_scopes() {
    for (auto& scope : application_hierachy) {
        scope.update_history();
    }
    for (auto& [method_key, method_queue] : this->method_queues) {
        for (auto& m_queue : method_queue) {
            m_queue.update_history();
        }
    }
}

std::time_t RegionHistory::validate_request(std::string_view method_key) {
    
    std::time_t wait_time = 0;
    bool no_limits = true;
    int hierachy = 0;
    while (no_limits && (hierachy < application_hierachy.size())) {
        wait_time = application_hierachy.at(hierachy).validate_request();
        if (wait_time != 0) {
            no_limits = false;
        }
        hierachy += 1;
    }

    std::time_t method_time;
    try {
        std::time_t method_time = std::accumulate(method_queues.at(method_key).begin(), method_queues.at(method_key).end(), std::time_t(0), [](std::time_t acc, ScopeHistory new_scope){if (std::time_t n = new_scope.validate_request();n > acc) {return n;} else {return acc;}});
        if (method_time > wait_time) {
            return method_time;
        }
    }
    catch (std::out_of_range& exc) {
        wait_time = 0;
    }
    return wait_time;
}

void RegionHistory::insert_request(std::time_t server_time, std::string_view method_key, std::string_view method_limits) {

    update_scopes();

    for (auto& scope : application_hierachy) {
        scope.insert_request(server_time);
    }

    try {
        for (auto& scope : method_queues.at(method_key)) {
            scope.insert_request(server_time);
        }
    }
    catch (std::out_of_range& exc) {// insert new method scope
        std::vector<ScopeHistory> new_method_hierachy = init_method_hierachy(method_limits, server_time);
        method_queues.insert(std::pair<std::string_view, std::vector<ScopeHistory>>(method_key, new_method_hierachy));
    }
}

RegionHistory init_region(std::vector<int> limits, std::vector<int> durations) {

    RegionHistory new_history;

    for (int i = limits.size()-1; i >= 0; i--) {
        new_history.application_hierachy.push_back({.duration = durations[i], .limit = limits[i]});
    }
    
    return new_history;
}

}
