def computeChecksum(nmea):
  checksum = 0
  for s in nmea:
    checksum = checksum ^ bytes(s, "ascii")[0]
  return "{:02x}".format(checksum)

data = [
  (57.697635,12.013142),
  (57.697474,12.014043),
  (57.696878,12.015502),
  (57.696213,12.016575),
  (57.695135,12.017734),
  (57.693828,12.019365),
  (57.693002,12.016962),
  (57.693736,12.014945),
  (57.695112,12.013056),
  (57.696832,12.011597),
  (57.697933,12.012413)
]

result = []

print(data)

for i, d in enumerate(data):
  minutesN = (d[0] % 1)*60
  degreesN = str(int(d[0]))
  minutesE = (d[1] % 1)*60
  degreesE = str(int(d[1]))
  t = 162445.11+i
  nmea = "GNGGA,"+str(t)+","+degreesN+"{0:.5f}".format(minutesN)+",N,"+degreesE+"{0:.5f}".format(minutesE)+",E,1,12,1.83,106.5,M,34.1,M,,"
  print("$"+nmea+"*"+computeChecksum(nmea))
