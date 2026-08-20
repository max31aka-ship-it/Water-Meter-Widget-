# water_meter.rb
#!/usr/bin/env ruby
require 'json'
require 'date'
require 'optparse'

DATA_FILE = 'water_data.json'

class WaterMeter
  attr_reader :data

  def initialize
    @data = []
    load
  end

  def load
    if File.exist?(DATA_FILE)
      begin
        @data = JSON.parse(File.read(DATA_FILE))
      rescue
        @data = []
      end
    end
  end

  def save
    File.write(DATA_FILE, JSON.pretty_generate(@data))
  end

  def add(value, unit = 'L', date = nil)
    date ||= Date.today.to_s
    @data << { 'date' => date, 'value' => value, 'unit' => unit }
    save
    puts "✅ Added: #{value} #{unit} on #{date}"
  end

  def list
    if @data.empty?
      puts "No data."
      return
    end
    puts "\n📋 All readings:"
    @data.each do |e|
      puts "  #{e['date']}: #{e['value']} #{e['unit']}"
    end
  end

  def stats(days = 7, unit = nil)
    if @data.empty?
      puts "No data."
      return
    end
    cutoff = Date.today - days
    filtered = @data.select { |e| Date.parse(e['date']) >= cutoff }
    if filtered.empty?
      puts "No data in the last #{days} days."
      return
    end
    target_unit = unit || 'L'
    total = 0
    filtered.each do |e|
      val = e['value']
      if e['unit'] == 'm3' && target_unit == 'L'
        val *= 1000
      elsif e['unit'] == 'L' && target_unit == 'm3'
        val /= 1000
      end
      total += val
    end
    avg = total / filtered.length
    puts "\n📊 Statistics (last #{days} days) – unit: #{target_unit}"
    puts "  Total: %.2f #{target_unit}" % total
    puts "  Average per day: %.2f #{target_unit}" % avg
  end

  def reset
    @data = []
    save
    puts "All data cleared."
  end
end

options = {}
OptionParser.new do |opts|
  opts.banner = "Usage: water_meter.rb <command> [options]"
  opts.on("add VALUE", Float, "Add a reading (default unit L)") { |v| options[:add_value] = v }
  opts.on("--unit UNIT", "L or m3") { |v| options[:unit] = v }
  opts.on("--date DATE", "YYYY-MM-DD") { |v| options[:date] = v }
  opts.on("list", "List all readings") { options[:cmd] = :list }
  opts.on("stats", "Show statistics") { options[:cmd] = :stats }
  opts.on("--days N", Integer, "Days for stats") { |v| options[:days] = v }
  opts.on("--stats-unit UNIT", "L or m3") { |v| options[:stats_unit] = v }
  opts.on("reset", "Clear all data") { options[:cmd] = :reset }
end.parse!

app = WaterMeter.new

if options[:add_value]
  app.add(options[:add_value], options[:unit] || 'L', options[:date])
elsif options[:cmd] == :list
  app.list
elsif options[:cmd] == :stats
  app.stats(options[:days] || 7, options[:stats_unit])
elsif options[:cmd] == :reset
  app.reset
else
  puts "Unknown command. Use add, list, stats, reset."
end
