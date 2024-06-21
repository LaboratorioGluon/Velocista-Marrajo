import matplotlib.pyplot as plt
import numpy as np
import sys
from matplotlib.widgets import MultiCursor

filename = sys.argv[1]
bloqnum = int(sys.argv[2])

def calculatePid(inputs, dts):
    kp=0.109
    ki = 0
    kd = 1.009*1000
    out = []
    lasti = 0
    d = 0
    for i in range(len(inputs)):
        p = kp*inputs[i]
        d = kd*(inputs[i]-lasti)/dts[i]
        lasti = inputs[i]
        print(d)
        out.append((p,d, p+d))
    
    return out

with open(filename,"r") as fp:
    fileLines = [f.replace("\n","").strip() for f in fp.readlines()]
    allBloques = []
    for f in fileLines:
        bloques = f.split(" ")
        if f.startswith("Index"):
            print("Nuevo bloque")
            allBloques.append([])
        if len(bloques) != 6:
            continue
        if len(allBloques) == 0:
            continue
        bloques = [ int(b) for b in bloques]
        #print(bloques)
        allBloques[-1].append(bloques.copy())
    
allBloques = [ np.transpose(sorted(a)) for a in allBloques]
for a in allBloques:
    print(len(a))

print(len(allBloques))
#Index | linePos | pidOutpu | motorR | motorL | dt
print(allBloques[bloqnum])

calculado = calculatePid(allBloques[bloqnum][1], allBloques[bloqnum][5])

plt.figure()
ax1 = plt.subplot(2,1,1)
ax1.set_ylim(-1000, 1000)
plt.plot(allBloques[bloqnum][0], allBloques[bloqnum][1], label="linePos")
ax2 = plt.subplot(2,1,2, sharex=ax1)
ax2.set_ylim(-500,500)
plt.plot(allBloques[bloqnum][0], allBloques[bloqnum][2], label="pidOutput")
plt.plot(allBloques[bloqnum][0], calculado, label="calculated")
plt.legend()

fig = plt.figure()
ax3 = fig.gca()
plt.plot(allBloques[bloqnum][0], allBloques[bloqnum][3], label="motorR")
plt.plot(allBloques[bloqnum][0], allBloques[bloqnum][4], label="motorL")
plt.legend()

multi = MultiCursor(None, (ax1, ax2, ax3), color='r', lw=1)
plt.show()

