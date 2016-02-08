import matplotlib.pyplot as plt
import re
import math

if __name__ == "__main__":
    rates = []
    programs = ["max_ave_seq_disk", "max_ave_seq_ram"]
    max_avg = []
    # Read output from bash
    with open("read_blocks_timing.txt") as f:
        lines = f.readlines()
        for l in lines:
            if ("Data rate" in l):
                l = re.sub(r"Data rate: ", r"", l)
                data_rate = l.split(" ")[0]
                rates.append(math.log10(float(data_rate)))
            elif max_avg == [] and("Max" in l):
                max_avg.append(l.split()[-1])
            elif len(max_avg) == 1 and("Average" in l):
                max_avg.append(l.split()[-1])            
                
                
    print rates
    
    print "Results"
    print "======="
    print "Max follower count is " + max_avg[0]
    print "Average follower count is " + max_avg[1]
    for i in xrange(len(rates)):
        fmt_str = "{0} had a write rate of {1} Log of BPS"
        print fmt_str.format(programs[i], rates[i])
        
    raw_input("Press enter to draw graph")
    
    plt.title("Data rate for read_blocks")
    plt.xlabel("Programs")
    plt.ylabel("Data Rate in Logarithm of BPS")
    plt.bar(range(len(rates)), rates, align='center', width=0.25, alpha=0.5)
    plt.xticks(range(len(programs)), programs)    
    plt.show()