import math
tab = []
for i in [x*0.01 for x in range(1001)]:
    tab.append(math.floor(math.pow(2, i)))
    print(math.floor(math.pow(2, i)))
print(tab)