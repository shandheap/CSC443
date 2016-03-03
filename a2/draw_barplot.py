from pylab import *

val = [ 55, 72]
pos = arange(len(val))+.5


barh(pos, val, align='center')
yticks(pos, ("2PMMS", "unix sort"))
xlabel('Running Time (seconds)')
ylabel('Sorting Algorithms')
title('2PMMS vs Unix Sort')
grid(True)

show()