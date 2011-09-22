import sys

c = 16 #steps per round
n = 89 #steps per rotation

rot1len = 6 # 6*c > n
rot2len = 12 # 12*c > 2*n
rot3len = 17 # 17*c > 3*n


def descend(pathLen, path, tpLen, paths):
  if pathLen <= 0 :
    paths.append(path)
    return 

  if len(path) > 0  and (path[-1] == '1' or path[-1] == '2'):
    descend(pathLen - tpLen, path+"T",  tpLen, paths)
  else:
    descend(pathLen - tpLen, path+"T",  tpLen, paths)

    #1 represents 6 rounds of an ntp path, 2 = 12 rounds and so on
    descend(pathLen - rot1len, path+"1",  tpLen, paths)
    descend(pathLen - rot2len, path+"2",  tpLen, paths)
    descend(pathLen - rot3len, path+"3",  tpLen, paths)


def costOfPath(path, length, aag, tpresult, ntpresult, tpLen):
  if len(path) == 0 or length < 6:
    return aag

  #we are at the end of a path so, path sections may be incomplete
  if len(path) == 1:
    if path[0] == 'T':
      return tpresult[length]+aag
    if path[0] == '1' or path[0] == '2' or path[0] == '3':
      return ntpresult[length]+aag


  if path[0] == 'T':
    return costOfPath(path[1:], length-tpLen, aag+tpresult[tpLen], tpresult, ntpresult, tpLen)

  if path[0] == '1': #one section of ntp
    return costOfPath(path[1:], length-rot1len, aag+ntpresult[rot1len], tpresult, ntpresult, tpLen)

  if path[0] == '2': #two contiguous sections of ntp
    return costOfPath(path[1:], length-rot2len, aag+ntpresult[rot2len], tpresult, ntpresult, tpLen)

  if path[0] == '3': #three contiguous sections of ntp
    return costOfPath(path[1:], length-rot3len, aag+ntpresult[rot3len], tpresult, ntpresult, tpLen)


def findCheapestPath(paths, length, tpdata, ntpdata):  
  tpLen =  max(tpdata.keys())

  solutions = min ([ ( costOfPath(path, length, 0, tpdata, ntpdata, tpLen), path ) for path in paths ])
  print "Minimum Pattern %s, Cost %d " % solutions[::-1]

def readfile(filename):
  return "".join([line for line in open(filename)])


#to test run: $python src/lb.py 153 <(python src/parse_diffp.py output/tp_0_1120_16__2.txt) <(python src/parse_diffp.py output/ntp_0_192_16__2.txt output/ntp_96_288__e4_r__2.txt)
def main():

  if len(sys.argv) < 4:
    print "python computeLB [# of rounds] [tp-parsed-output] [ntp-parsed-output]"
    return 1

  md6NumOfRounds = int(sys.argv[1])
  tpdata = eval(readfile(sys.argv[2]))
  ntpdata = eval(readfile(sys.argv[3]))

  paths = []
  descend(md6NumOfRounds, "", max(tpdata.keys()), paths)
  findCheapestPath(paths, md6NumOfRounds, tpdata, ntpdata)

if __name__ == "__main__":
  main()
