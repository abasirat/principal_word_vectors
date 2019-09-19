
#
# This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
# project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
# You are allowed to modify or distribute it if you keep this header part
#
# It can be used to generated an annotated corpus form a conllu format file (e.g., a UD corpus)
#

import sys


infile = sys.argv[1]

AND = "_"
OR = ","
LOWER_CASE=1

with open(infile,'r') as fp :
  line = fp.readline().rstrip() 
  items = []
  while line :
    line = line.lstrip()
    #print(line)

    if len(line) == 0 :
      print("")
      line = fp.readline().rstrip()
      for item in items :
        ID,word,pos,pid,drel = item
        ipid = int(pid)

        feat = pos + OR + drel 
        if ipid > 0 :
          PID,Pword,Ppos,Ppid,Pdrel = items[ipid-1]
          feat += OR + Ppos + OR + pos + AND + drel + AND + Ppos

        if LOWER_CASE:
          word = word.lower()

        print("{0}\t{1}\t{2}\t{3}".format(ID,word,pid,feat))
      items = []
      continue

    if (line[0] == '#'): 
      line = fp.readline().rstrip()
      continue


    line = line.rstrip().lstrip()

    toks = line.split()
    
    ID    = toks[0] ;
    word  = toks[1] ;
    pos   = toks[3] ;
    pid   = toks[6] ;
    drel  = toks[7].split(':')[0] ;

    pos = pos.replace(',',"COMMA")

    feat = pos + '_' + drel
    if ID.isnumeric() :
      #print("{0}\t{1}\t{2}\t{3}".format(ID,word,pid,feat))
      items.append((ID,word,pos,pid,drel))

    line = fp.readline()#.rstrip()
