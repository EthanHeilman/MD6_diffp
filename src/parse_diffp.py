import sys

c = 16 #steps per round

def mergeData(tables):
  mergedT = {}

  for t in tables:
    for key in t:
      value = t[key]  
      
      if mergedT.has_key(key) and mergedT[key] > t[key] :
        continue
      mergedT[key] = t[key]

  return mergedT


def printTable(table):
  keys = table.keys()
  keys.sort()
  print "{"

  strs = []
  for key in keys:
    strs.append(str(key)+": "+str(table[key]))
  print ",\n".join(strs)
  print "}"


def parseFile(f):
  resultsTable = {}

  for line in f:
    line = line.strip()
    if len(line) == 0:
      continue    

    if line[0] == "{":
      result = eval(line)

      #make sure that we are only incrementing in values of 16 (1 round)
      steps = int( result['stepLimit'] )
      aagCount =  int( result['AAG'] )
      rounds = int( result['rounds'] )

      if result['Success'] == 'false':
        aagCount += 1

      assert( steps % c == 0 )
      
      # we rely on the fact that highest aag search for a round always comes last
      resultsTable[ rounds ] = aagCount

  return resultsTable


#python src/lb.py <(python src/parse_diffp.py output/tp_0_1120_16__2.txt) <(python src/parse_diffp.py output/ntp_0_192_16__2.txt output/ntp_96_288__e4_r__2.txt)
def main():
  if len(sys.argv) < 2:
    print "python computeLB [output file 1] [output file 2] ..."
    return 1

  data = []
  for i  in xrange(1, len( sys.argv )):
    data.append(parseFile(open(sys.argv[i])))


  mergedData = mergeData(data)

  printTable(mergedData)


if __name__ == "__main__":
	main()
