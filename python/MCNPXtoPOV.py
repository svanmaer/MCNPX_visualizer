#######################################################################################################################
## MCNPXtoPOV.py
#######################################################################################################################
##
## Main function for parsing/building the MCNPX file to a Pov Ray file
## First pass: parses all the cell, surface and materials cards to a proper Python representations
## Second pass: builds all the cells available to a Pov Ray representation
##
##	arg1: Input MCNPX file
##	arg2: Output Pov Ray file
##	arg3: Optional input color map to be used in the building (when no color map given, it uses the standard colors)
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
## PRISMS:	http://www.povray.org/documentation/view/3.6.2/62/
## QUADRICS:	http://www.povray.org/documentation/view/3.6.0/299 (for infinite object description)
## BOX:		http://www.povray.org/documentation/view/3.6.1/276/
## SPHERE:	http://www.povray.org/documentation/view/3.6.1/283/
## CYLINDER:	http://www.povray.org/documentation/view/3.6.1/278/
## FRAMEWORK FOR SOLVING EQUATIONS: http://code.google.com/p/sympy/
#######################################################################################################################

import sys
import getopt

import povray
import MCNPXParser

# main function
# inputs a mcnpx-file (inputFile)
# outputs the pov ray builded scene
# optional (but recommended): input color map (link every material to a color)
def parse(inputFile, outputFile, colorMapFile):
	
############################&####
#  OPTIONS
################################

	buildVoid = True

# END OPTIONS

################################
#  INITIALISE
################################	
	
	parser = MCNPXParser.MCNPXParser(inputFile, outputFile, colorMapFile)
	parser.preProcess()	# remove unnecessary data out of the mcnpx file (i.e. comments)
	
	file=povray.File(outputFile,"colors.inc","stones.inc")
	fileImp=povray.File(outputFile+"_imp0.pov");
	
################################
#  PARSING
################################

	print "START PARSING", inputFile, " to ", outputFile
	
	print "\nPARSING DATA CARDS"
	parser.parseDataCards()
	print "\t" + str(len(parser.dataCards)) + " DATA CARDS PARSED"
	
	print "\nPARSING SURFACE CARDS"
	parser.parseSurfaces()
	print "\t" + str(len(parser.surfaceCards)) + " SURFACE CARDS PARSED"

	print "\nPARSING CELL CARDS"
	parser.parseCells()
	print "\t" + str(len(parser.cellCards)) + " SURFACE CARDS PARSED"
	
	print "\t"  + str(len(parser.universes)) + " Universes Found:",
	for uni in parser.universes:
		print str(uni) + "",
	print ""

################################
#  BUILDING
################################

	print "\nBUILDING CELL CARDS"
	
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
	imp0Items = []
	for i, card in  (enumerate(parser.cellCards)): # use enumerate to sort the cell cards to be builded
		cellCard = parser.getCellCard(card)
		if (cellCard and cellCard.params.has_key("IMP")):
			# cellcard with imp=0 doesn't need to be builded with colors and to the main pov-ray output file
			# it will be build without colors and a smaller scale to the fileImp output
			# this will be used to limit the rendered scene to the imp=1 space
			if (cellCard.params["IMP"] == "n=0" or cellCard.params["IMP"] == "N=0"):
				povItem = parser.buildCell(cellNumber = card, parent = 0, depth = 0,  buildVoid = buildVoid, useColor=False, scale=0.99999)
				if povItem:
					imp0Items.append(povItem)
				continue
			else:
				povItem = parser.buildCell(cellNumber = card, parent = 0, depth = 0,  buildVoid = buildVoid, useColor=True)
				if (povItem):
					items.append(["//" + parser.getGeometryOfCellCard(card), povItem])
		elif (cellCard):
			povItem = parser.buildCell(cellNumber = card, parent = 0, depth = 0,  buildVoid = buildVoid, useColor=True)
			if (povItem):
				items.append(["//" + parser.getGeometryOfCellCard(card), povItem])
	
	imp0Cells = parser.getImpZeroCellCard()
	imp0Items= []
	if (imp0Cells):
		for cell in imp0Cells:
			povItem = parser.buildCell(cellNumber = cell.number, parent = 0, depth = 0,  buildVoid = buildVoid, useColor=False, scale=0.99999)
			if povItem:
				imp0Items.append(povItem)
		if (len(imp0Items) == 1):
			fileImp.write(imp0Items[0])
		elif (len(imp0Items) > 1):
			fileImp.writeln("union {")
			fileImp.indent()
			for item in imp0Items:	
				fileImp.write(item)
			fileImp.dedent()
			fileImp.writeln(" }")
	
				
			
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
	file.writeln("// All cells are combined in a big union")
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

	print "\t" + str(len(parser.cellCards)) + " CELL CARDS BUILDED"
	print "\nEND BUILDING CELL CARDS"

	print "\nMCNPX to POV RAY COMPLETED"
	print "TITLE MCNPX: " + parser.title

	#oStrippedFile = open(inputFile + "stripped.txt", 'w')
	#for line in parser.cellBlock:
	#	oStrippedFile.writelines(line + '\n')
	#oStrippedFile.writelines('\n')
	#for line in parser.surfaceBlock:
	#	oStrippedFile.write(line + '\n')

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
	#parse("mcnpx/hex.txt", "mcnpx/hexOut.pov")
	#parse("mcnpx/big.txt", "mcnpx/bigOut.pov")
	#parse("mcnpx/g3.txt", "mcnpx/g3Out.pov", "mcnpx/g3_colorMap")
	#parse("mcnpx/refcore_c", "mcnpx/refcore_cOut.pov")
	#parse("mcnpx/80.0", "mcnpx/80.0Out.pov")
	#parse("mcnpx/graph.txt", "mcnpx/graphOut.pov")
	#parse("mcnpx/graph.txt", "mcnpx/graphOut.pov")
	#parse("mcnpx/graphb.txt", "mcnpx/grapbbOut.pov")
	#parse("mcnpx/graphc.txt", "mcnpx/graphcOut.pov")
	
	#---------------------------------------------------------
	# TEMP: TEST FILES
	#---------------------------------------------------------
	#parse("mcnpx/testNesting.txt", "mcnpx/testNestingOut.pov")
	#parse("mcnpx/testRotation.txt", "mcnpx/testRotationOut.pov")
	
	
	try:
		opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
	except getopt.error, msg:
		print msg
		print "for help use --help"
		sys.exit(2)
	

	
	if len(args) != 3:
		print "ERROR (MCNPXtoPOV.py): not enough arguments"
		return 1
	
	parse(args[0], args[1], args[2])
	

if __name__ == "__main__":
	initialize()

