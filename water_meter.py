# water_meter.py
import json
import os
import sys
import argparse
from datetime import datetime, timedelta

DATA_FILE = "water_data.json"
DEFAULT_UNIT = "L"

class WaterMeter:
    def __init__(self):
        self.data = []
        self.load()

    def load(self):
        if os.path.exists(DATA_FILE):
            with open(DATA_FILE, "r") as f:
                try:
                    self.data = json.load(f)
                except:
                    self.data = []

    def save(self):
        with open(DATA_FILE, "w") as f:
            json.dump(self.data, f, indent=2)

    def add(self, value, unit="L", date=None):
        if date is None:
            date = datetime.now().strftime("%Y-%m-%d")
        entry = {"date": date, "value": value, "unit": unit}
        self.data.append(entry)
        self.save()
        print(f"✅ Added: {value} {unit} on {date}")

    def list(self):
        if not self.data:
            print("No data.")
            return
        print("\n📋 All readings:")
        for e in self.data:
            print(f"  {e['date']}: {e['value']} {e['unit']}")

    def stats(self, days=7, unit=None):
        if not self.data:
            print("No data.")
            return
        cutoff = datetime.now() - timedelta(days=days)
        filtered = [e for e in self.data if datetime.strptime(e["date"], "%Y-%m-%d") >= cutoff]
        if not filtered:
            print(f"No data in the last {days} days.")
            return
        # Convert all to the requested unit
        target_unit = unit if unit else DEFAULT_UNIT
        total = 0
        for e in filtered:
            val = e["value"]
            if e["unit"] == "m3" and target_unit == "L":
                val *= 1000
            elif e["unit"] == "L" and target_unit == "m3":
                val /= 1000
            total += val
        avg = total / len(filtered)
        print(f"\n📊 Statistics (last {days} days) – unit: {target_unit}")
        print(f"  Total: {total:.2f} {target_unit}")
        print(f"  Average per day: {avg:.2f} {target_unit}")

    def reset(self):
        self.data = []
        self.save()
        print("All data cleared.")

def main():
    parser = argparse.ArgumentParser(description="Water Meter Widget")
    subparsers = parser.add_subparsers(dest="cmd", required=True)

    add_parser = subparsers.add_parser("add")
    add_parser.add_argument("value", type=float)
    add_parser.add_argument("unit", nargs="?", default="L", choices=["L", "m3"])
    add_parser.add_argument("--date", help="YYYY-MM-DD", default=None)

    stats_parser = subparsers.add_parser("stats")
    stats_parser.add_argument("--days", type=int, default=7)
    stats_parser.add_argument("--unit", choices=["L", "m3"], default=None)

    subparsers.add_parser("list")
    subparsers.add_parser("reset")

    args = parser.parse_args()
    meter = WaterMeter()

    if args.cmd == "add":
        meter.add(args.value, args.unit, args.date)
    elif args.cmd == "stats":
        meter.stats(args.days, args.unit)
    elif args.cmd == "list":
        meter.list()
    elif args.cmd == "reset":
        meter.reset()

if __name__ == "__main__":
    main()
