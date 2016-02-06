import matplotlib.pyplot as plt
import subprocess
import time
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

    for s in sizes:
        # Program and args to pass to write_blocks
        cmd = ["./write_blocks", "g_plusAnonymized.csv",  str(s)]

        # Parse data rate from output
        result = subprocess.check_output(cmd)
        result = re.sub(r"Data rate: ", r"", result)
        data_rate = result.split(" ")[0]
        rates.append(float(data_rate))

    print "Data rates for specified sizes are as follows:"
    print rates
    raw_input("Press enter to draw graph")
    plt.title("Data rate for write_blocks")
    plt.xlabel("Size in Bytes")
    plt.ylabel("Data Rate in MBPs")
    plt.plot(sizes, rates)
    plt.show()
