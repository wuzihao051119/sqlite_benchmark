## This repository is a benchmark about the efficiency of WHERE IN statement in SQLite

### Test in the following environment
* CPU: Intel(R) Xeon(R) E5-2673 v4 @ 2.30GHz x2 (2 Processors, 40 Cores, 80 Threads)
* Memory: 2133MHz DDR4 128GB
* OS: Windows 11 Pro for WorkStations

### Conclusion
Test 100 cycles and delete 32000 records in each cycle.

When using
```sql
DELETE * FROM test WHERE val = ?;
```

The average time is 140878.007 us.

When using
```sql
DELETE * FROM test WHERE val IN (?,?,?);
```

The average time is 190504.312 us.
