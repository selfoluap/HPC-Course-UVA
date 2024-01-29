with open("test.txt", "r") as f:
    #read lines and remove { and }
    lines = [line.strip("{}") for line in f.readlines()]
    count = 0
    for line in lines:
        #count number of occurrences of 1
        count += line.count("1")
    print(count)