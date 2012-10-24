#######################################################################################################################
## MCNPXPreParser.py
#######################################################################################################################
##
## Parses the most important information out of a mcnpx-file and divides it over the different output files
## It can be used as an information coupling between the mcnpx-file and the GUI
##
##	arg1: Input MCNPX file
##	arg2: Output surfaces file
##	arg3: Output cells file
##	arg4: Output universes file
##	arg5: Output importance file
##	arg6: Output materials file
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
#######################################################################################################################

import sys
import getopt

import povray
import MCNPXParser

# main function
# inputs a mcnpx-file (inputFile)
# outputs the mncpx information of surfaces, cells, universes, importance and materials
def parse(inputFile, surfacesFile, cellsFile, universesFile, importanceFile, materialsFile, cellTreeFile):
	
############################&####
#  OPTIONS
################################

	# buildVoid = True

# END OPTIONS

################################
#  INITIALISE
################################	
	parser = MCNPXParser.MCNPXParser(inputFile, surfacesFile)
	parser.preProcess() # remove unnecessary data out of the mcnpx file (i.e. comments)
	
	fileSurfaces=povray.File(surfacesFile)
	fileCells=povray.File(cellsFile)
	fileUniverses=povray.File(universesFile)
	fileImportance=povray.File(importanceFile)
	fileMaterials=povray.File(materialsFile)
	fileCellTree=povray.File(cellTreeFile)


################################
#  PARSING
################################
	print "\nSTART PREPARSING", inputFile
	
	print "\nPREPARSING DATACARDS"
	parser.parseDataCards()
	print "\t" + str(len(parser.dataCards)) + " DATA CARDS PARSED"
	
	# write all materials defined in the mcnpx data block to fileMaterials
	# including the optional name for the material
	print parser.materialCards
	for mat in parser.materialCards.keys(): 
		fileMaterials.writeln(str(mat) + " " + str(parser.materialCardsName[int(mat)]))
	
	print "\nPREPARSING SURFACE CARDS"
	parser.parseSurfaces()
	print "\t" + str(len(parser.surfaceCards)) + " SURFACE CARDS PARSED"
	for surface in parser.surfaceCards:
		fileSurfaces.writeln(str(surface) + "&" + str(parser.surfaceCards[surface].mnemonic) + "&" + str(parser.surfaceCards[surface].data)) 
	
	print "\nPREPARSING CELL CARDS"
	parser.parseCells()
	
	# write the cellcard with importance 0 to fileImportance
	imp0 = parser.getImpZeroCellCard()
	if (imp0):
		for cCard in imp0:
			if (cCard):
				parser.writeOuterCaseToFile(cCard, fileImportance)
	for cell in parser.cellCards:
		fileCells.writeln(str(cell) + "&" + str(parser.cellCards[cell].material) + "&" + str(parser.cellCards[cell].d) + "&" + str(parser.cellCards[cell].geometry) + "&" + str(parser.cellCards[cell].params)) 
	
	# write all universes defined in the mcnpx file to fileUniverses
	for uni in parser.universes:
		uniLine = uni
		for uniCell in parser.universes[uni]:
			uniLine = uniLine + "&" + str(uniCell)
		fileUniverses.writeln(uniLine) 
		
	print "\t" + str(len(parser.cellCards)) + " CELL CARDS PARSED"
	
	# write the tree structure of the cells and universes to file
	fileCellTree.writeln(parser.createCellTree())
	
	print "\nPREPARSING COMPLETED"
	
	print "TITLE MCNPX: " + parser.title

	
	parser.close()

# initialize the parameters for the preparser
# interpret the input arguments and give them in a proper way to the preparser
def initialize():
	
	
		
	try:
		opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
	except getopt.error, msg:
		print msg
		print "for help use --help"
		sys.exit(2)
	
	if len(args) != 7:
		print "ERROR (MCNPXPreParser.py): not enough arguments"
		return 1

	# process arguments
	parse(args[0], args[1], args[2], args[3], args[4], args[5], args[6])
	

if __name__ == "__main__":
	initialize()

