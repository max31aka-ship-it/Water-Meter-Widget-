// WaterMeter.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

class Entry
{
    [JsonPropertyName("date")] public string Date { get; set; }
    [JsonPropertyName("value")] public double Value { get; set; }
    [JsonPropertyName("unit")] public string Unit { get; set; }
}

class WaterMeter
{
    private List<Entry> data = new List<Entry>();
    private readonly string dataFile = "water_data.json";
    private readonly JsonSerializerOptions options = new JsonSerializerOptions { WriteIndented = true };

    public WaterMeter() => Load();

    private void Load()
    {
        if (!File.Exists(dataFile)) return;
        string json = File.ReadAllText(dataFile);
        data = JsonSerializer.Deserialize<List<Entry>>(json) ?? new List<Entry>();
    }

    private void Save()
    {
        string json = JsonSerializer.Serialize(data, options);
        File.WriteAllText(dataFile, json);
    }

    public void Add(double value, string unit = "L", string date = null)
    {
        if (string.IsNullOrEmpty(date)) date = DateTime.Now.ToString("yyyy-MM-dd");
        if (string.IsNullOrEmpty(unit)) unit = "L";
        data.Add(new Entry { Date = date, Value = value, Unit = unit });
        Save();
        Console.WriteLine($"✅ Added: {value} {unit} on {date}");
    }

    public void List()
    {
        if (!data.Any()) { Console.WriteLine("No data."); return; }
        Console.WriteLine("\n📋 All readings:");
        foreach (var e in data)
            Console.WriteLine($"  {e.Date}: {e.Value} {e.Unit}");
    }

    public void Stats(int days = 7, string unit = null)
    {
        if (!data.Any()) { Console.WriteLine("No data."); return; }
        var cutoff = DateTime.Now.AddDays(-days);
        var filtered = data.Where(e => DateTime.Parse(e.Date) >= cutoff).ToList();
        if (!filtered.Any())
        {
            Console.WriteLine($"No data in the last {days} days.");
            return;
        }
        string targetUnit = unit ?? "L";
        double total = 0;
        foreach (var e in filtered)
        {
            double val = e.Value;
            if (e.Unit == "m3" && targetUnit == "L") val *= 1000;
            else if (e.Unit == "L" && targetUnit == "m3") val /= 1000;
            total += val;
        }
        double avg = total / filtered.Count;
        Console.WriteLine($"\n📊 Statistics (last {days} days) – unit: {targetUnit}");
        Console.WriteLine($"  Total: {total:F2} {targetUnit}");
        Console.WriteLine($"  Average per day: {avg:F2} {targetUnit}");
    }

    public void Reset()
    {
        data.Clear();
        Save();
        Console.WriteLine("All data cleared.");
    }

    static void Main(string[] args)
    {
        if (args.Length < 1)
        {
            Console.WriteLine("Usage: WaterMeter <command> [options]");
            return;
        }
        var app = new WaterMeter();
        string cmd = args[0];
        switch (cmd)
        {
            case "add":
                double val = double.Parse(args[1]);
                string unit = "L";
                string date = null;
                for (int i=2; i<args.Length; i++)
                {
                    if (args[i] == "--unit" && i+1 < args.Length) unit = args[++i];
                    if (args[i] == "--date" && i+1 < args.Length) date = args[++i];
                }
                app.Add(val, unit, date);
                break;
            case "list":
                app.List();
                break;
            case "stats":
                int days = 7;
                string u = null;
                for (int i=1; i<args.Length; i++)
                {
                    if (args[i] == "--days" && i+1 < args.Length) days = int.Parse(args[++i]);
                    if (args[i] == "--unit" && i+1 < args.Length) u = args[++i];
                }
                app.Stats(days, u);
                break;
            case "reset":
                app.Reset();
                break;
            default:
                Console.WriteLine("Unknown command.");
                break;
        }
    }
}
