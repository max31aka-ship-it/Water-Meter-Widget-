# water_meter.php
#!/usr/bin/env php
<?php

$dataFile = 'water_data.json';

class WaterMeter {
    private $data = [];
    private $file;

    function __construct($file) {
        $this->file = $file;
        $this->load();
    }

    function load() {
        if (file_exists($this->file)) {
            $content = file_get_contents($this->file);
            $this->data = json_decode($content, true) ?? [];
        }
    }

    function save() {
        file_put_contents($this->file, json_encode($this->data, JSON_PRETTY_PRINT));
    }

    function add($value, $unit = 'L', $date = null) {
        if ($date === null) $date = date('Y-m-d');
        $this->data[] = ['date' => $date, 'value' => $value, 'unit' => $unit];
        $this->save();
        echo "✅ Added: $value $unit on $date\n";
    }

    function list() {
        if (empty($this->data)) {
            echo "No data.\n";
            return;
        }
        echo "\n📋 All readings:\n";
        foreach ($this->data as $e) {
            echo "  {$e['date']}: {$e['value']} {$e['unit']}\n";
        }
    }

    function stats($days = 7, $unit = null) {
        if (empty($this->data)) {
            echo "No data.\n";
            return;
        }
        $cutoff = new DateTime();
        $cutoff->modify("-$days days");
        $filtered = array_filter($this->data, function($e) use ($cutoff) {
            return new DateTime($e['date']) >= $cutoff;
        });
        if (empty($filtered)) {
            echo "No data in the last $days days.\n";
            return;
        }
        $targetUnit = $unit ?? 'L';
        $total = 0;
        foreach ($filtered as $e) {
            $val = $e['value'];
            if ($e['unit'] == 'm3' && $targetUnit == 'L') $val *= 1000;
            elseif ($e['unit'] == 'L' && $targetUnit == 'm3') $val /= 1000;
            $total += $val;
        }
        $avg = $total / count($filtered);
        echo "\n📊 Statistics (last $days days) – unit: $targetUnit\n";
        echo "  Total: " . number_format($total, 2) . " $targetUnit\n";
        echo "  Average per day: " . number_format($avg, 2) . " $targetUnit\n";
    }

    function reset() {
        $this->data = [];
        $this->save();
        echo "All data cleared.\n";
    }
}

$app = new WaterMeter($dataFile);

$cmd = $argv[1] ?? null;
if ($cmd === 'add') {
    $value = (float)($argv[2] ?? 0);
    $unit = $argv[3] ?? 'L';
    $date = null;
    for ($i=4; $i<$argc; $i++) {
        if ($argv[$i] == '--date' && isset($argv[$i+1])) {
            $date = $argv[$i+1];
            $i++;
        }
    }
    $app->add($value, $unit, $date);
} elseif ($cmd === 'list') {
    $app->list();
} elseif ($cmd === 'stats') {
    $days = 7;
    $unit = null;
    for ($i=2; $i<$argc; $i++) {
        if ($argv[$i] == '--days' && isset($argv[$i+1])) {
            $days = (int)$argv[$i+1];
            $i++;
        }
        if ($argv[$i] == '--unit' && isset($argv[$i+1])) {
            $unit = $argv[$i+1];
            $i++;
        }
    }
    $app->stats($days, $unit);
} elseif ($cmd === 'reset') {
    $app->reset();
} else {
    echo "Usage: water_meter.php add <value> [unit] [--date YYYY-MM-DD] | list | stats [--days N] [--unit L|m3] | reset\n";
}
?>
