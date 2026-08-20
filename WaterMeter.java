// WaterMeter.java
import java.io.*;
import java.nio.file.*;
import java.time.*;
import java.time.format.*;
import java.util.*;
import com.google.gson.*;

class Entry {
    String date;
    double value;
    String unit;
}

public class WaterMeter {
    private List<Entry> data = new ArrayList<>();
    private final String dataFile = "water_data.json";
    private final Gson gson = new GsonBuilder().setPrettyPrinting().create();

    public WaterMeter() { load(); }

    private void load() {
        try {
            Path path = Paths.get(dataFile);
            if (Files.exists(path)) {
                String json = new String(Files.readAllBytes(path));
                Entry[] arr = gson.fromJson(json, Entry[].class);
                data = Arrays.asList(arr);
            }
        } catch (Exception e) {}
    }

    private void save() {
        try {
            Files.write(Paths.get(dataFile), gson.toJson(data).getBytes());
        } catch (Exception e) {}
    }

    public void add(double value, String unit, String date) {
        if (date == null) date = LocalDate.now().toString();
        if (unit == null) unit = "L";
        Entry e = new Entry();
        e.date = date;
        e.value = value;
        e.unit = unit;
        data.add(e);
        save();
        System.out.printf("✅ Added: %.2f %s on %s%n", value, unit, date);
    }

    public void list() {
        if (data.isEmpty()) { System.out.println("No data."); return; }
        System.out.println("\n📋 All readings:");
        for (Entry e : data) {
            System.out.printf("  %s: %.2f %s%n", e.date, e.value, e.unit);
        }
    }

    public void stats(int days, String unit) {
        if (data.isEmpty()) { System.out.println("No data."); return; }
        LocalDate cutoff = LocalDate.now().minusDays(days);
        List<Entry> filtered = new ArrayList<>();
        for (Entry e : data) {
            LocalDate d = LocalDate.parse(e.date);
            if (d.isAfter(cutoff) || d.isEqual(cutoff)) filtered.add(e);
        }
        if (filtered.isEmpty()) {
            System.out.printf("No data in the last %d days.%n", days);
            return;
        }
        String targetUnit = unit != null ? unit : "L";
        double total = 0;
        for (Entry e : filtered) {
            double val = e.value;
            if (e.unit.equals("m3") && targetUnit.equals("L")) val *= 1000;
            else if (e.unit.equals("L") && targetUnit.equals("m3")) val /= 1000;
            total += val;
        }
        double avg = total / filtered.size();
        System.out.printf("%n📊 Statistics (last %d days) – unit: %s%n", days, targetUnit);
        System.out.printf("  Total: %.2f %s%n", total, targetUnit);
        System.out.printf("  Average per day: %.2f %s%n", avg, targetUnit);
    }

    public void reset() {
        data.clear();
        save();
        System.out.println("All data cleared.");
    }

    public static void main(String[] args) throws Exception {
        if (args.length < 1) {
            System.out.println("Usage: WaterMeter <command> [options]");
            return;
        }
        WaterMeter app = new WaterMeter();
        String cmd = args[0];
        switch (cmd) {
            case "add": {
                double value = Double.parseDouble(args[1]);
                String unit = "L";
                String date = null;
                for (int i=2; i<args.length; i++) {
                    if (args[i].equals("--unit") && i+1<args.length) {
                        unit = args[++i];
                    }
                    if (args[i].equals("--date") && i+1<args.length) {
                        date = args[++i];
                    }
                }
                app.add(value, unit, date);
                break;
            }
            case "list":
                app.list();
                break;
            case "stats": {
                int days = 7;
                String unit = null;
                for (int i=1; i<args.length; i++) {
                    if (args[i].equals("--days") && i+1<args.length) {
                        days = Integer.parseInt(args[++i]);
                    }
                    if (args[i].equals("--unit") && i+1<args.length) {
                        unit = args[++i];
                    }
                }
                app.stats(days, unit);
                break;
            }
            case "reset":
                app.reset();
                break;
            default:
                System.out.println("Unknown command.");
        }
    }
}
