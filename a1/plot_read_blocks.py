import matplotlib.pyplot as plt
import re

if __name__ == "__main__":
    rates = []
    programs = ["max_ave_seq_disk", "max_ave_rand_disk", "max_ave_seq_ram", "max_ave_rand_ram"]
    # Read output from bash
    with open("read_blocks_timing.txt") as f:
        lines = f.readlines()
        for l in lines:
            if ("Data rate" in l):
                l = re.sub(r"Data rate: ", r"", l)
                data_rate = l.split(" ")[0]
                rates.append(float(data_rate))
                
    print rates
    
    print "Results"
    print "======="
    for i in xrange(len(rates)):
        fmt_str = "{0} had a write rate of {1} MBps"
        print fmt_str.format(programs[i], rates[i])
        
    raw_input("Press enter to draw graph")
    
    plt.title("Data rate for read_blocks")
    plt.xlabel("Programs")
    plt.ylabel("Data Rate in MBPs")
    plt.bar(range(len(rates)), rates, align='center', width=0.3, alpha=0.5)
    plt.xticks(range(len(programs)), programs)    
    plt.show()