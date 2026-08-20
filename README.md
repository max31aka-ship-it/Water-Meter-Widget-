💧 Water Meter Widget — Multi‑Language Consumption Tracker
8 languages, one simple water usage widget – track your daily water consumption, view statistics, and stay hydrated with data.

✨ Features
📝 Add readings – record consumption (litres or m³) with optional date

📊 View statistics – daily, weekly, monthly averages and totals

📋 List all entries – see your full history

🔄 Persistent storage – all data saved in a JSON file

🎛️ Unit selection – switch between litres (L) and cubic metres (m³)

⏳ No external dependencies – runs in any environment

🧰 Supported Languages & Files
Language	File
Python	water_meter.py
Go	water_meter.go
JavaScript (Node)	water_meter.js
Ruby	water_meter.rb
PHP	water_meter.php
Java	WaterMeter.java
C#	WaterMeter.cs
C++	water_meter.cpp
🚀 Common Usage
All implementations follow the same CLI pattern:

bash
# Add a reading (default unit = litres)
<command> add 120
<command> add 0.5 m3

# Add a reading for a specific date
<command> add 100 --date 2025-12-25

# Show statistics (last 7 days by default)
<command> stats
<command> stats --days 30
<command> stats --unit m3

# List all readings
<command> list

# Reset all data
<command> reset
Arguments:

add <value> [unit] [--date YYYY-MM-DD] – record consumption

stats [--days N] [--unit L|m3] – show average and total

list – display all entries

reset – clear all data

📸 Example Output
text
💧 Water Meter Widget
Total consumption (last 7 days): 245 L
Daily average: 35.0 L
Entries:
2025-12-19: 30 L
2025-12-20: 40 L
2025-12-21: 35 L
2025-12-22: 50 L
2025-12-23: 45 L
2025-12-24: 25 L
2025-12-25: 20 L
📁 Repository Structure
text
.
├── README.md
├── python/
│   └── water_meter.py
├── go/
│   └── water_meter.go
├── javascript/
│   └── water_meter.js
├── ruby/
│   └── water_meter.rb
├── php/
│   └── water_meter.php
├── java/
│   └── WaterMeter.java
├── csharp/
│   └── WaterMeter.cs
└── cpp/
    └── water_meter.cpp
