import time
import os

def test():
    t = time.time()
    os.system('build/Turtle examples/test.ty > /dev/null')
    return time.time() - t

k = list()
for i in range(0,100):
    a = test()
    print(a)
    k.append(a)

print("AVG: ", sum(k)/len(k))