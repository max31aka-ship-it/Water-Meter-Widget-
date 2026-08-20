// water_meter.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct Entry {
    string date;
    double value;
    string unit;
};

class WaterMeter {
private:
    vector<Entry> data;
    string dataFile = "water_data.json";

    string currentDate() {
        time_t t = time(nullptr);
        tm* now = localtime(&t);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", now);
        return string(buf);
    }

    void load() {
        ifstream f(dataFile);
        if (!f.is_open()) return;
        json j;
        f >> j;
        for (auto& item : j) {
            Entry e;
            e.date = item["date"];
            e.value = item["value"];
            e.unit = item["unit"];
            data.push_back(e);
        }
    }

    void save() {
        json j = json::array();
        for (auto& e : data) {
            j.push_back({{"date", e.date}, {"value", e.value}, {"unit", e.unit}});
        }
        ofstream f(dataFile);
        f << setw(2) << j << endl;
    }

public:
    WaterMeter() { load(); }

    void add(double value, const string& unit, const string& date) {
        string d = date.empty() ? currentDate() : date;
        string u = unit.empty() ? "L" : unit;
        data.push_back({d, value, u});
        save();
        cout << "✅ Added: " << value << " " << u << " on " << d << endl;
    }

    void list() {
        if (data.empty()) { cout << "No data." << endl; return; }
        cout << "\n📋 All readings:" << endl;
        for (auto& e : data) {
            cout << "  " << e.date << ": " << e.value << " " << e.unit << endl;
        }
    }

    void stats(int days, const string& unit) {
        if (data.empty()) { cout << "No data." << endl; return; }
        time_t now = time(nullptr);
        tm* now_tm = localtime(&now);
        time_t cutoff = now - days * 24 * 3600;
        vector<Entry> filtered;
        for (auto& e : data) {
            struct tm tm = {};
            strptime(e.date.c_str(), "%Y-%m-%d", &tm);
            time_t t = mktime(&tm);
            if (t >= cutoff) filtered.push_back(e);
        }
        if (filtered.empty()) {
            cout << "No data in the last " << days << " days." << endl;
            return;
        }
        string targetUnit = unit.empty() ? "L" : unit;
        double total = 0;
        for (auto& e : filtered) {
            double val = e.value;
            if (e.unit == "m3" && targetUnit == "L") val *= 1000;
            else if (e.unit == "L" && targetUnit == "m3") val /= 1000;
            total += val;
        }
        double avg = total / filtered.size();
        cout << "\n📊 Statistics (last " << days << " days) – unit: " << targetUnit << endl;
        cout << "  Total: " << fixed << setprecision(2) << total << " " << targetUnit << endl;
        cout << "  Average per day: " << fixed << setprecision(2) << avg << " " << targetUnit << endl;
    }

    void reset() {
        data.clear();
        save();
        cout << "All data cleared." << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: water_meter <command> [options]" << endl;
        return 1;
    }
    WaterMeter app;
    string cmd = argv[1];
    if (cmd == "add") {
        if (argc < 3) { cerr << "add <value> [unit] [--date YYYY-MM-DD]" << endl; return 1; }
        double value = stod(argv[2]);
        string unit = "L";
        string date;
        for (int i=3; i<argc; i++) {
            if (string(argv[i]) == "--date" && i+1 < argc) {
                date = argv[++i];
            } else if (string(argv[i]) != "--date") {
                unit = argv[i];
            }
        }
        app.add(value, unit, date);
    } else if (cmd == "list") {
        app.list();
    } else if (cmd == "stats") {
        int days = 7;
        string unit;
        for (int i=2; i<argc; i++) {
            if (string(argv[i]) == "--days" && i+1 < argc) {
                days = stoi(argv[++i]);
            }
            if (string(argv[i]) == "--unit" && i+1 < argc) {
                unit = argv[++i];
            }
        }
        app.stats(days, unit);
    } else if (cmd == "reset") {
        app.reset();
    } else {
        cerr << "Unknown command. Use add, list, stats, reset." << endl;
        return 1;
    }
    return 0;
}
