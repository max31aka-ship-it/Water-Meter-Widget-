// water_meter.go
package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"
)

type Entry struct {
	Date  string  `json:"date"`
	Value float64 `json:"value"`
	Unit  string  `json:"unit"`
}

type WaterMeter struct {
	Data []Entry `json:"data"`
	File string
}

func NewWaterMeter(file string) *WaterMeter {
	w := &WaterMeter{File: file}
	w.load()
	return w
}

func (w *WaterMeter) load() {
	data, err := os.ReadFile(w.File)
	if err != nil {
		return
	}
	json.Unmarshal(data, w)
}

func (w *WaterMeter) save() {
	data, _ := json.MarshalIndent(w, "", "  ")
	os.WriteFile(w.File, data, 0644)
}

func (w *WaterMeter) Add(value float64, unit string, date string) {
	if date == "" {
		date = time.Now().Format("2006-01-02")
	}
	if unit == "" {
		unit = "L"
	}
	w.Data = append(w.Data, Entry{Date: date, Value: value, Unit: unit})
	w.save()
	fmt.Printf("✅ Added: %.2f %s on %s\n", value, unit, date)
}

func (w *WaterMeter) List() {
	if len(w.Data) == 0 {
		fmt.Println("No data.")
		return
	}
	fmt.Println("\n📋 All readings:")
	for _, e := range w.Data {
		fmt.Printf("  %s: %.2f %s\n", e.Date, e.Value, e.Unit)
	}
}

func (w *WaterMeter) Stats(days int, unit string) {
	if len(w.Data) == 0 {
		fmt.Println("No data.")
		return
	}
	cutoff := time.Now().AddDate(0, 0, -days)
	var filtered []Entry
	for _, e := range w.Data {
		t, _ := time.Parse("2006-01-02", e.Date)
		if t.After(cutoff) || t.Equal(cutoff) {
			filtered = append(filtered, e)
		}
	}
	if len(filtered) == 0 {
		fmt.Printf("No data in the last %d days.\n", days)
		return
	}
	targetUnit := "L"
	if unit != "" {
		targetUnit = unit
	}
	total := 0.0
	for _, e := range filtered {
		val := e.Value
		if e.Unit == "m3" && targetUnit == "L" {
			val *= 1000
		} else if e.Unit == "L" && targetUnit == "m3" {
			val /= 1000
		}
		total += val
	}
	avg := total / float64(len(filtered))
	fmt.Printf("\n📊 Statistics (last %d days) – unit: %s\n", days, targetUnit)
	fmt.Printf("  Total: %.2f %s\n", total, targetUnit)
	fmt.Printf("  Average per day: %.2f %s\n", avg, targetUnit)
}

func (w *WaterMeter) Reset() {
	w.Data = []Entry{}
	w.save()
	fmt.Println("All data cleared.")
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: water_meter <command> [options]")
		return
	}
	w := NewWaterMeter("water_data.json")
	cmd := os.Args[1]
	switch cmd {
	case "add":
		addCmd := flag.NewFlagSet("add", flag.ExitOnError)
		valStr := addCmd.String("value", "", "")
		unit := addCmd.String("unit", "L", "")
		date := addCmd.String("date", "", "")
		addCmd.Parse(os.Args[2:])
		if *valStr == "" && len(addCmd.Args()) > 0 {
			*valStr = addCmd.Args()[0]
		}
		val, _ := strconv.ParseFloat(*valStr, 64)
		w.Add(val, *unit, *date)
	case "list":
		w.List()
	case "stats":
		statsCmd := flag.NewFlagSet("stats", flag.ExitOnError)
		days := statsCmd.Int("days", 7)
		unit := statsCmd.String("unit", "", "")
		statsCmd.Parse(os.Args[2:])
		w.Stats(*days, *unit)
	case "reset":
		w.Reset()
	default:
		fmt.Println("Unknown command. Use add, list, stats, reset.")
	}
}
