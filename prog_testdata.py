import re
import matplotlib.pyplot as plt
import math
import numpy as np

filvag = r"C:\Users\super\OneDrive - Chalmers\Kandidatarbete\TestData.txt"

with open(filvag, "r", encoding="utf-8") as fil:
    #data = fil.read()
    rader = fil.readlines()
    for rad in fil:
        print(repr(rad))  

#print(data)

# Listor för att spara data
tider, foot1_x, foot1_y, foot1_z, foot2_x, foot2_y, foot2_z, dist = [], [], [], [], [], [], [], []

# Regex med extra mellanrumshantering
mönster = re.compile(
    r"(\d+:\d+:\d+\.\d+)\s+Foot_1:\s*(\d+)\s+(\d+)\s+(\d+)\s+Foot_2:\s*(\d+)\s+(\d+)\s+(\d+)\s+Distance:\s*([\d.]+)"
)

for rad in rader:
    rad = rad.strip()  # Tar bort onödiga mellanslag
    if "Foot_1" not in rad:  
        print(" Skipping malformed line: {rad}") 
        continue

for rad in rader:
    match = mönster.search(rad)
    if match:
        tider.append(match.group(1))
        foot1_x.append(int(match.group(2)))
        foot1_y.append(int(match.group(3)))
        foot1_z.append(int(match.group(4)))
        foot2_x.append(int(match.group(5)))
        foot2_y.append(int(match.group(6)))
        foot2_z.append(int(match.group(7)))
        dist.append(float(match.group(8)))

# Kontrollutskrift (felsökning)
print(f"Antal datapunkter: {len(tider)}")
if not tider:
    print(" Inga data extraherade! Kolla filens format.")
    exit()

# Konvertera tid till numeriskt index
tidssteg = range(len(tider))

# Plotta värdena
plt.figure(figsize=(12, 6))
plt.plot(tidssteg, foot1_x, label="Foot_1 X", marker="o", markersize=1)
plt.plot(tidssteg, foot1_y, label="Foot_1 Y", marker="o", markersize=1)
plt.plot(tidssteg, foot1_z, label="Foot_1 Z", marker="o", markersize=1)
plt.plot(tidssteg, foot2_x, label="Foot_2 X", marker="s", markersize=1)
plt.plot(tidssteg, foot2_y, label="Foot_2 Y", marker="s", markersize=1)
plt.plot(tidssteg, foot2_z, label="Foot_2 Z", marker="s", markersize=1)
plt.plot(tidssteg, dist, label="Distance", linestyle="dashed", linewidth=2)

plt.xlabel("Tid (index)")
plt.ylabel("Värde")
plt.title("Plott av fotpositioner och distans")
plt.legend()
plt.grid()
plt.show()

# Exempel på sensordata för varje fot (ersätt med dina faktiska data)
# Summera komponenterna för varje fot (summan av motsvarande element i listorna)
foot1 = [x + y + z for x, y, z in zip(foot1_x, foot1_y, foot1_z)]
foot2 = [x + y + z for x, y, z in zip(foot2_x, foot2_y, foot2_z)]

# Beräkna medelvärdet för varje fot
foot_balance = abs(np.mean(foot1) - np.mean(foot2)) #detta borde man vilja ha så nära 0 som möjligt ???
print("Foot balance:", foot_balance)

################################################################################################################################################### 
# behöver egentligen dela upp olika datan efter de olika faserna och sedan kolla hur trycket varierar i fram/bakfot beroende på
# definera upp faserna på något smart sätt
foot1_fram = [x + y for x, y in zip(foot1_x, foot1_y)]
foot1_bak = [z for z in foot1_z]  # Ingen zip behövs för bakfoten, det är en lista
foot2_fram = [x + y for x, y in zip(foot2_x, foot2_y)]
foot2_bak = [z for z in foot2_z]  # Ingen zip behövs för bakfoten

# Total tryck på varje fot
total_foot1 = [fram + bak for fram, bak in zip(foot1_fram, foot1_bak)]
total_foot2 = [fram + bak for fram, bak in zip(foot2_fram, foot2_bak)]

# Beräkna andelen tryck på framfoten för varje fot
foot1_fram_ratio = [fram / total if total != 0 else 0 for fram, total in zip(foot1_fram, total_foot1)]
foot2_fram_ratio = [fram / total if total != 0 else 0 for fram, total in zip(foot2_fram, total_foot2)]
foot1_bak_ratio = [bak / total if total != 0 else 0 for bak, total in zip(foot1_bak, total_foot1) ]
foot2_bak_ratio = [bak / total if total != 0 else 0 for bak, total in zip(foot2_bak, total_foot2) ]

# Utskrift av resultat
#print("Andel tryck på vänster framfot:", foot1_fram_ratio)
#print("Andel tryck på höger framfot:", foot2_fram_ratio)
#print("Andel tryck på vänster bakfot:", foot1_bak_ratio)
#print("Andel tryck på höger bakfot:", foot2_bak_ratio)

plt.figure(figsize=(12,6))
plt.plot(tidssteg, foot1_fram_ratio, label = 'andel_fot1_fram')
plt.plot(tidssteg, foot2_fram_ratio, label = 'andel_fot2_fram')
plt.plot(tidssteg, foot1_bak_ratio, label = 'andel_fot1_bak')
plt.plot(tidssteg, foot2_bak_ratio, label = 'andel_fot2_bak')

plt.xlabel("Tid (index)")
plt.ylabel("Andel")
plt.title("Plot av tryckfördelning - varje fot för sig")
plt.legend()
plt.grid()
plt.show()
###############################################################################################################################
# andel höger respektive vänster fot, nu är de inte kallibrerade men en bra skidåkare vill ju kunna ha 100% vänster och sen 100% höger
total_both_feet = [right + left for left,right in zip(foot1, foot2)]
ratio_foot1 = [left / total2 if total2 != 0 else 0 for left,total2 in zip(foot1, total_both_feet)]
ratio_foot2 = [right / total2 if total2 != 0 else 0 for right,total2 in zip(foot2, total_both_feet)]

plt.figure(figsize=(12,6))
plt.plot(tidssteg, ratio_foot1, label = 'andel_fot1')
plt.plot(tidssteg, ratio_foot2, label = 'andel_fot2')


plt.xlabel("Tid (index)")
plt.ylabel("Andel")
plt.title("Andel tryck från höger respektive vänster fot")
plt.legend()
plt.grid()
plt.show()