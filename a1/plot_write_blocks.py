import matplotlib.pyplot as plt
import re


KB = 1024
MB = 1024 * 1024
sizes = [
    512,
    1 * KB,
    4 * KB,
    8 * KB,
    16 * KB,
    32 * KB,
    1 * MB,
    2 * MB,
    4 * MB
]

if __name__ == "__main__":
    rates = []

    # Read output from bash
    with open("write_blocks_timing.txt") as f:
        lines = f.readlines()
        for l in lines:
            l = re.sub(r"Data rate: ", r"", l)
            data_rate = l.split(" ")[0]
            rates.append(float(data_rate))

    print "Results"
    print "======="
    for i in xrange(len(sizes)):
        fmt_str = "{0} block size had a write rate of {1} BPS"
        print fmt_str.format(sizes[i], rates[i])

    raw_input("Press enter to draw graph")
    plt.title("Data rate for write_blocks")
    plt.xlabel("Size in Bytes")
    plt.ylabel("Data Rate in BPS")
    plt.plot(sizes, rates)
    plt.show()
