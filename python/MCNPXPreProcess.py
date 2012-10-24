#######################################################################################################################
## MCNPXPreProcess.py
#######################################################################################################################
##
## Class to represent a rotation
##
## Part of MCNPX Visualizer
## (c) Simon Vanmaercke (edited by Nick Michiels) for SCK-CEN Mol (2011)
#######################################################################################################################

import re

class MCNPXPreProcess:
 #read input file look for other files to include
 #and separate the blocks (cell, surface, physics)
 def __init__(self, filename):
   self.filedata = []
   self.filedataWithComments = []
   for line in open(filename).readlines():
     #print 'line: ' + line
     line = line.replace('\t', "     ")
     s = re.match(r"read.+file[\s=](\S+)", line, re.IGNORECASE|
re.DOTALL)
     if s:
       self.filedata.extend(open(s.groups()[0]).readlines() );
       self.filedataWithComments.extend(open(s.groups()[0]).readlines() );
       print "Including file %s" % s.groups()[0]
     else:
       self.filedata.append(line);
       self.filedataWithComments.append(line);
   #remove all comments on non-empty lines for easier parsing.
   for i,line in enumerate(self.filedata):
     #print str(i) + " => " + line
     s = re.match("(.*?)(\W\$\s*.*)|(\W[c]\s+.*)|(\W[C]\s+.*)", line, re.IGNORECASE|
re.DOTALL)
     if s:
       
       if len(s.groups()[0]) > 0:
         self.filedata[i] = s.groups()[0]
         self.filedataWithComments[i] = s.groups()[0]
   #filter out all other comments that occupy a complete line
   self.filedata = [ i for i in self.filedata if re.match("^\s*[c$]\s+", i, flags=re.IGNORECASE) is None]
   #print self.filedataWithComments;
   self.filedataWithComments = [ i for i in self.filedataWithComments if re.match("^\s*[$]\s+", i, flags=re.IGNORECASE ) is None]
  
   
   
   #merge multiple line statements to a single string.
   tempfiledata = []
   tempprevious = None
   for line in self.filedata:
     line = line.replace("&"," ")
     s = re.match(r"^\s{0,4}\S+.*", line)
     e = re.match(r"^\s*$", line)
     if s or e :
       #print line
       
       if tempprevious:
         tempfiledata.append(tempprevious)
       tempprevious = re.sub( "\n", " ", line)
     else:
       #strip newline
       tempprevious += re.sub( "\n", " ", line)
   self.filedata = tempfiledata
   
   
   
   tempfiledata = []
   tempprevious = None
   for line in self.filedataWithComments:
     line = line.replace("&"," ")
     s = re.match(r"^\s{0,4}\S+.*", line)
     e = re.match(r"^\s*$", line)
     if s or e :
       #print line
       
       if tempprevious:
         tempfiledata.append(tempprevious)
       tempprevious = re.sub( "\n", " ", line)
     else:
       #strip newline
       tempprevious += re.sub( "\n", " ", line)
   self.filedataWithComments = tempfiledata


   #split into the different blocks
   splitAt = []
   splitAtWithComments = []

   for i, line in enumerate(self.filedata):
       s = re.match(r"^\s*$", line)
       if s:
         splitAt.append(i)
   for i, line in enumerate(self.filedataWithComments):
       s = re.match(r"^\s*$", line)
       if s:
         splitAtWithComments.append(i)
    
   if len(splitAt) == 3:
     self.messageBlock = self.filedata[0:splitAt[0]]
     self.cellBlock = self.filedata[splitAt[0]+1:splitAt[1]]
     self.surfaceBlock = self.filedata[splitAt[1]+1:splitAt[2]]
     self.dataBlock = self.filedata[splitAt[2]+1:]
     self.messageBlockWithComments = self.filedataWithComments[0:splitAtWithComments[0]]
     self.cellBlockWithComments = self.filedataWithComments[splitAtWithComments[0]+1:splitAtWithComments[1]]
     self.surfaceBlockWithComments = self.filedataWithComments[splitAtWithComments[1]+1:splitAtWithComments[2]]
     self.dataBlockWithComments = self.filedataWithComments[splitAtWithComments[2]+1:]
   if len(splitAt) == 2:
     self.messageBlock = []
     self.cellBlock = self.filedata[0:splitAt[0]]
     self.surfaceBlock = self.filedata[splitAt[0]+1:splitAt[1]]
     self.dataBlock = self.filedata[splitAt[1]+1:]
     self.messageBlockWithComments = []
     self.cellBlockWithComments = self.filedataWithComments[0:splitAtWithComments[0]]
     self.surfaceBlockWithComments = self.filedataWithComments[splitAtWithComments[0]+1:splitAtWithComments[1]]
     self.dataBlockWithComments = self.filedataWithComments[splitAtWithComments[1]+1:]
   if len(splitAt) != 2 and len(splitAt) != 3:
     raise NameError("Error. Not enough/too many newlines in input file expected 2 or 3 newlines got %d"%len(splitAt))