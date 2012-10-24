#######################################################################################################################
## MCNPXCellParser.py
#######################################################################################################################
##
## Main function for parsing/building the MCNPX file to a Pov Ray file
## It builds only the cells that are given as input in the file cellFile
##
##    arg1: Input MCNPX file
##    arg2: Output Pov Ray file
##    arg3: Input file that specifies the cells to be rendered
##    arg4: Optional input color map to be used in the building (when no color map given, it uses the standard colors)
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################

#######################################################################################################################
## USEFULL LINKS
#######################################################################################################################
##
## POV-RAY
## -----------------------------------
## PRISMS:    http://www.povray.org/documentation/view/3.6.2/62/
## QUADRICS:    http://www.povray.org/documentation/view/3.6.0/299 (for infinite object description)
## BOX:        http://www.povray.org/documentation/view/3.6.1/276/
## SPHERE:    http://www.povray.org/documentation/view/3.6.1/283/
## CYLINDER:    http://www.povray.org/documentation/view/3.6.1/278/
#######################################################################################################################

import sys
import getopt

import povray
import MCNPXParser


# main function
# inputs a mcnpx-file (inputFile)
# input a cellFile specifying the cells to be rendered
# outputs the pov ray builded scene
# optional (but recommended): input color map (link every material to a color)
def parse(inputFile, outputFile, cellFile, colorMapFile):
    
############################&####
#  OPTIONS
################################
    
    buildVoid = True
    
# END OPTIONS

################################
#  INITIALISE
################################    
  
    parser = MCNPXParser.MCNPXParser(inputFile, outputFile, colorMapFile)
    parser.preProcess() # remove unnecessary data out of the mcnpx file (i.e. comments)
    
    file=povray.File(outputFile,"colors.inc","stones.inc")#, "camera.pov","lights.pov")

################################
#  PARSING
################################
    print "START PARSING", inputFile, " to ", outputFile
    
    print "PARSING DATA CARDS"
    parser.parseDataCards()
    print "\t" + str(len(parser.dataCards)) + " DATA CARDS PARSED"
    
    print "PARSING SURFACE CARDS"
    parser.parseSurfaces()
    print "\t" + str(len(parser.surfaceCards)) + " SURFACE CARDS PARSED"

    print "PARSING CELL CARDS"
    parser.parseCells()
    
    print "\tUniverses Found:",
    for uni in parser.universes:
        print str(uni) + "",
    print ""

################################
#  BUILDING
################################
    print "BUILD CELL CARDS"
    
    # write a standard macro for drawing universes in Pov-Ray which can be used for every sort of universe
    file.writeln("//Macro's for drawing")
    file.writeln("#macro Draw_Universe(universe, pos, rot)")
    file.indent()
    file.writeln("object {")
    file.indent()
    file.writeln("universe")
    file.writeln("#if (pos != 0)")
    file.indent()
    file.writeln("translate pos")
    file.dedent()
    file.writeln("#end")
    file.writeln("#if (rot != 0)")
    file.indent()
    file.writeln("rotate rot")
    file.dedent()
    file.writeln("#end")
    file.dedent()    
    file.writeln("}")
    file.dedent()
    file.writeln("#end")

  
    items = [] # this list will contain all the builded cell cards
    cellF = open(cellFile, 'r+')
    for line in cellF:
        if (line != ""):
            povItem = parser.buildCell(cellNumber = int(line), parent = -1, depth = 0,  buildVoid = buildVoid, useColor=True)
            if (povItem):
                items.append(["//" + parser.getGeometryOfCellCard(line), povItem])

    # write the declared macro's for the universes to the povray file
    # it needs a correct sorting so every sub macro that is used in a universe is known
    # every universe is defined with a certain depth (depth first added to the pov ray file)
    file.writeln("// DECLARED UNIVERSES")
    file.writeln("// ***************************************************************************")
    sortedUniverses = sorted(parser.declaredUniverses, key=lambda depth: depth[0], reverse=True)
    for i in range(0, len(sortedUniverses)): 
        file.writeln("// UNIVERSE " + parser.declaredUniverses[i][1])
        file.write(parser.declaredUniverses[i][2])
        
    # After printing all the macro's, the cell cards are added to the file
    file.writeln("//All cells are combined in a big union")
    file.writeln("union {")
    file.indent()    
    for i in range(0,len(items)):
        file.writeln(items[i][0])
        file.write(items[i][1]) 
    file.dedent()
    file.writeln("}")
    file.writeln("")

    # print surface cards to the povray output as debug information
    file.writeln("// LIST OF ALL SURFACES:")
    for card in parser.surfaceCards:
        surfaceCard = parser.surfaceCards[card]
        file.writeln("//" + surfaceCard.getSurfaceLine())

    
    print "\t" + str(len(items)) + " CELL CARDS BUILDED"
    print "\nEND BUILDING CELL CARDS"

    print "\nMCNPX to POV RAY COMPLETED"
    print "TITLE MCNPX: " + parser.title

    #oStrippedFile = open(inputFile + "stripped.txt", 'w')
    #for line in parser.cellBlock:
    #    oStrippedFile.writelines(line + '\n')
    #oStrippedFile.writelines('\n')
    #for line in parser.surfaceBlock:
    #    oStrippedFile.write(line + '\n')

    parser.close()

    

def initialize():
    
    #---------------------------------------------------------
    # TEMP: hard coded arguments
    #---------------------------------------------------------
    #parse("mcnpx/if4", "mcnpx/if4Out.pov")
    #parse("mcnpx/if4b", "mcnpx/if4bOut.pov")
    #parse("mcnpx/if5", "mcnpx/if5Out.pov")
    #parse("mcnpx/geom5b", "mcnpx/geom5bOut.pov")
    #parse("mcnpx/geom5a", "mcnpx/geom5aOut.pov")
    # parse("mcnpx/hex.txt", "mcnpx/hexOut.pov")
    #parse("mcnpx/big.txt", "mcnpx/bigOut.pov")
    #parse("mcnpx/g3.txt", "mcnpx/g3Out.pov")
    #parse("mcnpx/80.0", "mcnpx/80.0Out.pov")
    #parse("mcnpx/graph.txt", "mcnpx/graphOut.pov")
    #parse("mcnpx/graph.txt", "mcnpx/graphOut.pov")
    #parse("mcnpx/graphb.txt", "mcnpx/grapbbOut.pov")
    #parse("mcnpx/graphc.txt", "mcnpx/graphcOut.pov")
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
    except getopt.error, msg:
        print msg
        print "for help use --help"
        sys.exit(2)
    
    if len(args) != 4:
        print "ERROR (MCNPXCellParser.py): not enough arguments"
        return 1
    
    parse(args[0], args[1], args[2], args[3])
    

if __name__ == "__main__":  #startup the parsing
    initialize()

