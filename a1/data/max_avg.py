def find_max(d, value):
    
    for key in d.keys():
        if d[key] == 1781:
            return key

if __name__ == "__main__":
	
    f = open("g_plusAnonymized.csv");
    d = {}
    while 1:
        line = f.readline()
        if not line:
            break
        pair = line.strip().split(",")
        if d.has_key(int(pair[0])):
            d[int(pair[0])] += 1
        else:
            d[int(pair[0])] = 1
    
    max_key =  find_max(d, 1781)
    avg_val = sum(d.values()) * 1.0/ len(d)
    max_val = max(d.values())
    
    print("Avg: " + str(avg_val) + ", MaxVal: " + str(max_val) + " MaxValKey: "+ str(max_key))
    
    
