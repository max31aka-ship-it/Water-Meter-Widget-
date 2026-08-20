// water_meter.js
#!/usr/bin/env node
const fs = require('fs');
const { program } = require('commander');

const DATA_FILE = 'water_data.json';

class WaterMeter {
    constructor() {
        this.data = [];
        this.load();
    }

    load() {
        if (fs.existsSync(DATA_FILE)) {
            try {
                this.data = JSON.parse(fs.readFileSync(DATA_FILE));
            } catch (e) {
                this.data = [];
            }
        }
    }

    save() {
        fs.writeFileSync(DATA_FILE, JSON.stringify(this.data, null, 2));
    }

    add(value, unit = 'L', date = null) {
        if (!date) {
            date = new Date().toISOString().slice(0,10);
        }
        this.data.push({ date, value, unit });
        this.save();
        console.log(`✅ Added: ${value} ${unit} on ${date}`);
    }

    list() {
        if (this.data.length === 0) {
            console.log('No data.');
            return;
        }
        console.log('\n📋 All readings:');
        for (const e of this.data) {
            console.log(`  ${e.date}: ${e.value} ${e.unit}`);
        }
    }

    stats(days = 7, unit = null) {
        if (this.data.length === 0) {
            console.log('No data.');
            return;
        }
        const cutoff = new Date();
        cutoff.setDate(cutoff.getDate() - days);
        const filtered = this.data.filter(e => new Date(e.date) >= cutoff);
        if (filtered.length === 0) {
            console.log(`No data in the last ${days} days.`);
            return;
        }
        const targetUnit = unit || 'L';
        let total = 0;
        for (const e of filtered) {
            let val = e.value;
            if (e.unit === 'm3' && targetUnit === 'L') val *= 1000;
            else if (e.unit === 'L' && targetUnit === 'm3') val /= 1000;
            total += val;
        }
        const avg = total / filtered.length;
        console.log(`\n📊 Statistics (last ${days} days) – unit: ${targetUnit}`);
        console.log(`  Total: ${total.toFixed(2)} ${targetUnit}`);
        console.log(`  Average per day: ${avg.toFixed(2)} ${targetUnit}`);
    }

    reset() {
        this.data = [];
        this.save();
        console.log('All data cleared.');
    }
}

program
    .command('add <value>')
    .option('--unit <unit>', 'L or m3', 'L')
    .option('--date <date>', 'YYYY-MM-DD')
    .action((value, options) => {
        const meter = new WaterMeter();
        meter.add(parseFloat(value), options.unit, options.date);
    });

program
    .command('list')
    .action(() => {
        const meter = new WaterMeter();
        meter.list();
    });

program
    .command('stats')
    .option('--days <days>', 'Number of days', parseInt, 7)
    .option('--unit <unit>', 'L or m3')
    .action((options) => {
        const meter = new WaterMeter();
        meter.stats(options.days, options.unit);
    });

program
    .command('reset')
    .action(() => {
        const meter = new WaterMeter();
        meter.reset();
    });

program.parse(process.argv);
