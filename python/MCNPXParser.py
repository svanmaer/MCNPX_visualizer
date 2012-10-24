#######################################################################################################################
## MCNPXParser.py
#######################################################################################################################
##
## Core parser/builder
## Provides all the functionality for parsing and building a mcnpx file to a pov ray scene
## It parses useful information out of a mcnpx file and maps the combined surfaces of a cell to povray objects (using CSG)
##
## Part of MCNPX Visualiser
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################

#######################################################################################################################
## USEFULL LINKS
#######################################################################################################################
##
## POV-RAY
## -----------------------------------
## PRISMS:  http://www.povray.org/documentation/view/3.6.2/62/
## QUADRICS:    http://www.povray.org/documentation/view/3.6.0/299 (for infinite object description)
## BOX:     http://www.povray.org/documentation/view/3.6.1/276/
## SPHERE:  http://www.povray.org/documentation/view/3.6.1/283/
## CYLINDER:    http://www.povray.org/documentation/view/3.6.1/278/
##
## MATH
## -----------------------------------
## FRAMEWORK FOR SOLVING EQUATIONS: http://code.google.com/p/sympy/
#######################################################################################################################

import re
import copy
import math

import MCNPXPreProcess
import BoundingBox
from DataHolder import DataHolder
from Color import Color
import SurfaceCard
import CellCard
import DataCard
import container
import povray


class MCNPXParser:

#######################################################################################################################
## INITIALIZATION
#######################################################################################################################

    # initialization function
    # inputs a mcnpx-file (inputFile)
    # outputs the pov ray builded scene
    # optional (but recommended): input color map (link every material to a color)
    #------------------------------------------------------------------------------------------------------------------
    def __init__(self, inputFile, outputFile, colorMapFile=0):
        
        self.inputFile = inputFile
        self.outputFile = outputFile
        self.colorMapFile = colorMapFile
        
        self.cellCards = {}                 # contains all the different surface cards
        self.surfaceCards = {}              # contains all the different surface cards
        self.dataCards = {}                 # contains all the different data cards
        self.universes = {}                 # group all cells of the different universes
        self.title = ""
        
        self.complementCard = None
        
        self.colorMap = {}                  # color map for the different materials
        
        self.declaredUniverses = []         # list of all declared universe macro's - format: [depth, povray.Declare]

        self.transformationCards = {}       # transformations that are defined in the data cards
        self.transformationCardsFlag = {}
        self.materialCards = {}
        self.materialCardsName = {}
        
        self.subsurfaceMapNumberA = 97      # left character of a subsurface identifier
        self.subsurfaceMapNumberB = 97      # right character of a subsurface identifier
    
        ############################&####
        #  OPTIONS
        ################################
        self.useMacros = True
        

    def __repr__(self):
        return "MCNPXParser()"
    def __str__(self):
        return "member of MCNPXParser"
    
    # ==> close()
    # Close the parser
    #------------------------------------------------------------------------------------------------------------------
    def close(self):
        pass;

    # ==> preProcess()
    # Initialize the parser
    # Delegate the different blocks of mcnpx information to the right block
    # Strip comments
    # Read in materials color map
    #------------------------------------------------------------------------------------------------------------------
    def preProcess(self):
        #read input file look for other files to include
        #and separate the blocks (cell, surface, physics)
        pre = MCNPXPreProcess.MCNPXPreProcess(self.inputFile)
        
        self.messageBlock = pre.messageBlock    # preliminary unparsed message block
        self.cellBlock = pre.cellBlock          # preliminary unparsed cells block
        self.surfaceBlock = pre.surfaceBlock    # preliminary unparsed surfaces block
        self.dataBlock = pre.dataBlock          # preliminary unparsed data block
        self.dataBlockComments = pre.dataBlockWithComments          # preliminary unparsed data block (with comments)
            
        # read in the given colorMapFile
        # map a material on a "Color" object
        if (self.colorMapFile):
            for line in open(self.colorMapFile).readlines():
                dh = DataHolder()
                if (dh.set(re.match('^[\s]*(?P<material>[\d]+)[\s]+(?P<red>[\d]+)[\s]+(?P<green>[\d]+)[\s]+(?P<blue>[\d]+)[\s]+(?P<alpha>[\d\.]+)[\s]*', line ,flags=re.IGNORECASE))):
                    items = dh.value.groupdict()
                    self.colorMap[items['material']] = Color(float(items['red'])/255.0, float(items['green'])/255.0, float(items['blue'])/255.0, float(items['alpha']))
    # end preProcess
    
    
#######################################################################################################################
## PARSING CELLS
#######################################################################################################################

    # ==> parseCells()
    # interpret every line of the self.cellBlock
    # get cell number and process the surface operations of the cell
    # of the form: 
    #       TITLE: title of the mcnpx file
    # or    j m d geom params (j = cell number, m = material number if not a void, d = particle density, geom = specifications of geometry (intersection, union, complement), params = optional)
    # or    j LIKE n BUT list (j = cell number, n = number of another cell, list KEYWORD=value are params that differ of n)
    #------------------------------------------------------------------------------------------------------------------
    def parseCells(self):
        # read all cell cards out of file and put them in cellCards
        
        if len(self.cellBlock) == 0:
            return
        
        lineNr = 0
        line = self.cellBlock[lineNr]
        # check if the first line is a title
        
        beginPos = 0
        
        e = re.match('(?P<title>^[\s]*[\D]+[\S\s]*)', line ,flags=re.IGNORECASE)
        if e:
            self.title = e.groupdict()['title']
            beginPos = 1
        e = re.match('^[\s]*TITLE[\s]*[:]?(?P<title>[\S\s]*)', line ,flags=re.IGNORECASE)
        if e:
            self.title = e.groupdict()['title']
            beginPos = 1
    
                
                
        for lineNr in range(beginPos, len(self.cellBlock)):
            
            line = self.cellBlock[lineNr]
            #print line

            cellCard = re.split('[\s]+', line) 
            container.Container.remove_values_from_list(cellCard, '')
            
            title = re.match('^[\s]*TITLE[\s]*[:]?(?P<title>[\S\s]*)', line ,flags=re.IGNORECASE)
            
            
            # check if the form is of the second type (LIKE)
            likeCell = re.findall('(?<=like )[\d]+(?= BUT)', line, flags=re.IGNORECASE)
            if (  len(likeCell) == 1 ):
                likeCell = likeCell[0] # likeCell is the reference cell number
                likeParams = re.findall('(?<=but)[\d,\D,\s]+', line, flags=re.IGNORECASE)
                likeParams = likeParams[0].replace("&", " ")
                
                cellCardP = re.split('[\s]+', likeParams)
                
                # copy the reference cell card and change it with the new parameters
                card = copy.deepcopy(self.cellCards[int(likeCell)])
                self.cellCards[int(cellCard[0])] = card
                card.paramsData = cellCardP # change the data of the paramters to the new one
                card.parseParameters()
                
                # see if the cell is part of a universe. If so, add it the universe
                if (card.params.has_key('U')):
                    if (self.universes.has_key(card.params['U'])):
                        self.universes[card.params['U']].append(card.number)
                    else:
                        self.universes[card.params['U']] = [card.number]

                self.parseCellCardGeometry(card.number) # parse the geometry of the cell
            
            # check if the current line contains the title
            elif (title):
                self.title = int(title.groupdict()['title'])
                    
            # otherwise, the line is just a normal cell card    
            else: 
                card = CellCard.CellCard(int(cellCard[0]))
                card.material = int(cellCard[1])
                geometryStartPosition = 2
                # BEWARE: if there is no material specified there is no density so we can parse directly the geometry
                if (card.material is not 0):
                    # density is specified
                    try:
                        card.d = float(cellCard[2])
                    except ValueError:
                        pos = cellCard[2].find('-', 1)
                        if (pos is not -1):
                            cellCard[2] = cellCard[2][0:pos] + 'E' + cellCard[2][pos:]
                            print "WARNING: Bad density input for cell " + str(card.number) + " solved to " + cellCard[2]
                            
                    geometryStartPosition = 3
                else:
                    # density is not specified
                    card.d = 0;
                    geometryStartPosition = 2
                    pass
                # Read in the geometry and params
                # put geometry of cell in self.geometry list
                # put params of cell in self.params list
                card.geometry = []
                card.paramsData = []
                parametersStarted = False

                for n in range(geometryStartPosition, len(cellCard)):
                    if (re.match("^[a-z,\*]+[.]*", cellCard[n], flags=re.IGNORECASE) or parametersStarted):
                        # start/continue reading parameters
                        if (parametersStarted == False):
                            parametersStarted = True
                        card.paramsData.append(cellCard[n])
                    else:
                        # reading geometry
                        card.geometry.append(cellCard[n])
                        
                self.cellCards[int(cellCard[0])] = card
                card.parseParameters()

                # if the cell card is part of a universe, add it
                if (card.params.has_key('U')):
                    if (self.universes.has_key(card.params['U'])):
                        self.universes[card.params['U']].append(card.number)
                    else:
                        self.universes[card.params['U']] = [card.number]

                self.parseCellCardGeometry(card.number) # parse the geometry of the cell
                
    # ==> parseCellCardGeometry(cellNumber)
    # given the cellNumber, parse the geometry and parameters and put them in the right cellcard object
    # subsurfaces are already build and stored in the subsurfaceMapInv or subsurfaceMapInvColor
    #   (subsurfaceMapInv contains povray objects without color-
    # The dictionary contains a map from a two-character word to a builded povray object
    #   i.e.: #(300 : 500) will be transformed to #aa where aa->object(300 : 500)
    #------------------------------------------------------------------------------------------------------------------     
    def parseCellCardGeometry(self, cellNumber):
    
        cellNumber = int(cellNumber)
        if (not self.cellCards.has_key(int(cellNumber))):
            print "Error buildCellCard -> cell " + str(cellNumber) + " not known"
            raise(Exception("ERROR (Parse Geometry Cell " + str(cellNumber) + "): Cell " + str(cellNumber) + " not known"))
            
            return
        cellCard = self.cellCards[cellNumber]

        # iterate over geometry
        # every entry in the geometry needs to be intersected with each other because they are separated by spaces (see MCNPX manual)
        # check if an entry contains ':' or '#' or if they are separated by brackets '(' and ')'
        cellCard.fullGeometry = str(cellCard.geometry[0])
        
        # combine the full geometry in a string that will be used for parsing
        for subsurface in range(1, len(cellCard.geometry)):
            cellCard.fullGeometry = cellCard.fullGeometry + " " + str(cellCard.geometry[subsurface])
        # i.e. cellCard.fullGeometry = "A (B:C) D (F (G:H))" # STRING FOR TESTING BRACKETS

        openBrackets = 0 # used as stack counter
        
        cellCard.subsurfaceMap = {}         # contains a mapping of a subsurface string of the form '(...)' to an identifier
        cellCard.subsurfaceMapDepth = {}    # keeps the stack depth of a certain subsurface
        cellCard.subsurfaceMapInvColor = {} # contains a mapping of an identifier of a subsurface (i.e. aa) to an colored POV ray item
        cellCard.subsurfaceMapInv = {}      # contains a mapping of an identifier of a subsurface (i.e. aa) to an POV ray item

        # search for opening and closing brackets
        # if opening: push on the stack and add position to bracketPositions
        # if closing: interpret substring defined by the range cellCard.fullGeometry[bracketPositions.last(), currentPosition]
        # the builded Povray object is stored at the subsurfaceMapInv dictionary
        bracketPositions = []
        for i in range(0 , len(cellCard.fullGeometry)):
            if (cellCard.fullGeometry[i] == '('):
                openBrackets = openBrackets + 1
                bracketPositions.append(i)
                #print "Found open bracket at position " + str(i) + " (total: " + str(openBrackets) + ")"
            elif (cellCard.fullGeometry[i] == ')'):
                # define the substring for the subsurface
                beginPos = bracketPositions.pop()
                openBrackets = openBrackets - 1
                newSubGeometry = cellCard.fullGeometry[beginPos+1:i]
                # replace already defined subsurfaces in the string so there is no building error
                newSubGeometry = self.replaceSubGeometry(str(newSubGeometry), cellCard)
                
                cellCard.subsurfaceMap["(" + newSubGeometry + ")"] = chr(self.subsurfaceMapNumberA) + chr(self.subsurfaceMapNumberB)
                cellCard.subsurfaceMapDepth["(" + newSubGeometry + ")"] = openBrackets
            
                # build the subsurface map string and add it to the dictionary
                cellCard.subsurfaceMapInvColor[chr(self.subsurfaceMapNumberA)+chr(self.subsurfaceMapNumberB)] = self.buildSubGeometry(newSubGeometry,cellCard, {}, True)#cellCard.getPovRayArgs()) # parse the subcell between the brackets to a pov ray item
                cellCard.subsurfaceMapInv[chr(self.subsurfaceMapNumberA)+chr(self.subsurfaceMapNumberB)] = self.buildSubGeometry(newSubGeometry,cellCard, {}, False)
                cellCard.charToGeometry[chr(self.subsurfaceMapNumberA)+chr(self.subsurfaceMapNumberB)] = "(" + newSubGeometry + ")"#cellCard.fullGeometry[beginPos+1:i]
                self.subsurfaceMapNumberB = self.subsurfaceMapNumberB + 1
                if (self.subsurfaceMapNumberB > (97 + 25)):
                    self.subsurfaceMapNumberB = 97
                    self.subsurfaceMapNumberA = self.subsurfaceMapNumberA + 1

                
        # every subsurface is now calculated and a mapping to the pov ray item is added in the subsurface map
        # now we replace the subsurfaces in the total geometry statement so it can be interpreted
        # important: this need to be done in depth first
        cellCard.fullGeometry = self.replaceSubGeometry(cellCard.fullGeometry, cellCard)

        
    # ==> replaceSubGeometry(geometry, cellCard)
    # Replace all known subsurfaces in a string (geometry) with its corresponding 2 character identifier.
    # The subsurfaces are unique for a cellcard (so the cellcard that need to be used is the second argument)
    #------------------------------------------------------------------------------------------------------------------     
    def replaceSubGeometry(self, geometry, cellCard):
        # find max depth
        currentDepth = 0
        for depth in cellCard.subsurfaceMapDepth:
            if cellCard.subsurfaceMapDepth[depth] > currentDepth:
                currentDepth = cellCard.subsurfaceMapDepth[depth]
        
        for i in range(currentDepth, -1, -1):
            # count from max depth to 0
            for subsurface in cellCard.subsurfaceMap:
                if cellCard.subsurfaceMapDepth[subsurface] == i:
                    geometry =  str(geometry).replace(subsurface, " " + cellCard.subsurfaceMap[subsurface] + " ")
        return geometry

    # ==> getGeometryOfCellCard(cellNumber)
    # Returns the full geometry of a cell
    #------------------------------------------------------------------------------------------------------------------     
    def getGeometryOfCellCard(self, cellNumber):
        cellNumber = int(cellNumber)
        if (int(cellNumber) not in self.cellCards):
            raise(Exception("ERROR (Build Cell " + str(cellNumber) + "): Cell " + str(cellNumber) + " not known"))
            return
        cellCard = self.cellCards[cellNumber]
        
        fullGeometry = "Cell " + str(cellNumber) + ": " + str(cellCard.geometry[0])
        for subsurface in range(1, len(cellCard.geometry)):
            fullGeometry = fullGeometry + " " + str(cellCard.geometry[subsurface])
        return fullGeometry
    
    
    # ==> getCellCard(cellNumber)
    # Returns cellcard that is identified with a cellNumber (if exists)
    #------------------------------------------------------------------------------------------------------------------     
    def getCellCard(self, cellNumber):
        if (not self.cellCards.has_key(int(cellNumber))):
            raise(Exception("ERROR (Build Cell " + str(cellNumber) + "): Cell " + str(cellNumber) + " not known"))
            return  0
        return self.cellCards[cellNumber]


    def getTopLevelCells(self, parent):
        topLevel = []
        for cellNumber in self.cellCards:
            card = self.getCellCard(cellNumber)
            
            
            
            if (not card):
                continue
            
            dh = DataHolder()
            #if (card.params["IMP"] == "n=0" or card.params["IMP"] == "N=0"):
            if (card.params.has_key("IMP")):
                if (dh.set(re.match('[\w,\s]*n[\w,\s]*=[\s]*0', card.params["IMP"] ,flags=re.IGNORECASE))):
                    continue
            
            # IMPORTANT: CELLS ARE ONLY RENDERED WHEN THEY AREN'T UNIVERSES OR IF THEIR UNIVERSE IS DRAWN FROM A PARENT CELL
            if (parent == 0 and (card.params.has_key('U'))):
                continue
            topLevel.append(card)
        return topLevel
    
    def createCellTree(self):
        topLevel = self.getTopLevelCells(0)
        returnString = ""
        for card in topLevel:
            returnString = returnString + "0," + str(card.number) + ",0" + "\n"
            returnString = returnString + self.createCellTreeStringRecursive(card, 1)
        return returnString
            
            
    def createCellTreeStringRecursive(self, card, depth):
        #if not card.params.has_key('U'):
        #   return ""
        if depth > 22:
            #print card
            #print depth
            raise(Exception("ERROR: Reached a maximum recursion depth of 22."))
            return ""
        
        returnString = ""

        
        if (card.fillUniverse is not 0 & (not card.hasLAT)):
            
            for cell in self.universes[str(card.fillUniverse)]:
                #for i in range(0, depth):
                #   returnString = returnString + "\t"
                returnString = returnString + str(depth) + "," + str(cell) + "," + str(card.fillUniverse) + "\n"
                returnString = returnString + self.createCellTreeStringRecursive(self.getCellCard(cell), depth+1)
                
            return returnString 
        elif (card.hasLAT):
            dummy = {} # helps to see if the universe is already used
            dummy[card.params['U']] = card.params['U']
            # seek for all the universes that the lattice contains
            for universeCounter in range(0, len(card.latUniverses)):
                if dummy.has_key(card.latUniverses[universeCounter]):
                    # universe already encountered for the lattice
                    #print "HASKEY"
                    continue
                    pass
                else:
                    #print card.latUniverses
                    dummy[card.latUniverses[universeCounter]] = card.latUniverses[universeCounter]
                    for cell in self.universes[str(card.latUniverses[universeCounter])]:
                        #for i in range(0, depth):
                        #   returnString = returnString + "\t"
                        returnString = returnString + str(depth) + "," + str(cell) + "," + str(card.latUniverses[universeCounter]) + "\n"
                        returnString = returnString + self.createCellTreeStringRecursive(self.getCellCard(cell), depth+1)
            return returnString
        else:
            return ""
            
        
#######################################################################################################################
## BUILDING CELLS
#######################################################################################################################

    # ==> buildCell(cellNumber, parent, depth,useColor, buildVoid = False, scale=1.0):
    # Given the cellNumber, build the POV version of the cell, including all parameters and sub universes
    # Only renders the cell when there is a parent defined or or the cell is no part of a universe
    #       cellNumber = identifier of the cell that needs to be rendered
    #       parent = identifier of the parent cell/universe
    #       depth = specifies the depth of the cell in the universe hierarchy 
    #       buildVoid = specify if cells with empty materials will be rendered
    #       scale = change the scale of the builded cell
    # It outputs a builded pov ray object
    #------------------------------------------------------------------------------------------------------------------
    def buildCell(self, cellNumber, parent, depth,useColor, buildVoid = False, scale=1.0):
        card = self.getCellCard(cellNumber)
        
        if (not card):
            return
        
        # IMPORTANT: CELLS ARE ONLY RENDERED WHEN THEY AREN'T UNIVERSES OR IF THEIR UNIVERSE IS DRAWN FROM A PARENT CELL
        if (parent == 0 and (card.params.has_key('U'))):
            return 0
        
        # check if void cells need to be drawn
        if (card.material == 0 and not buildVoid):
            # no material means void, so no rendering
            return False
    
        # build the full geometry of the cell card
        totalPovRayBuild = self.buildSubGeometry(card.fullGeometry, card, card.getPovRayArgs(), useColor, scale)# )

        if card.number == 201:
            print card
            
        if (self.complementCard != None):
            return totalPovRayBuild
        # INTERPRET THE CELL PARAMETERS AND EXECUTE THEM
        # ------------------------------------------------
        
        
            
        if (card.fillUniverse is not 0 & (not card.hasLAT)):
            # filling cell with universe card.fillUniverse 
            universe = self.buildUniverse(card.fillUniverse, parent = card.number, depth = depth, povRayArgs = card.getUniversePovRayArgs(), buildVoid = buildVoid, useColor = useColor)
            return povray.Object(universe, **card.getPovRayArgs()) # transform the cards if there are args defined
            
        elif (card.hasLAT):
            # build the lattice that the cell describes
            lattice = self.buildLattice(card, parent, depth,buildVoid)
            return povray.Object(lattice, **card.getPovRayArgs()) # transform the cards if there are args defined
        
        else:
            # if there is no sub universe or lattice defined, only the parent geometry is returned
            return totalPovRayBuild
            

    # ==> buildLattice(latticeCard, parent, depth, buildVoid = False):
    # Build the lattice that is defined in the latticeCard
    #       latticeCard = card that contains the lattice that need to be build
    #       parent = identifier of the parent cell/universe
    #       depth = specifies the depth of the cell in the universe hierarchy 
    #       buildVoid = specify if cells with empty materials will be rendered
    #------------------------------------------------------------------------------------------------------------------
    def buildLattice(self, latticeCard, parent, depth, buildVoid = False):
        universeCounter = 0
        lattice = []

        # only build a lattice when there is a parent defined
        # so top level lattices doesn't exists
        if (not parent):
            return
        parentCard = self.getCellCard(parent)
        if (not parentCard):
            return
        bbParent = self.getBoundingBoxOfGeometryOfCell(parentCard.number)
        
        # current universe of the lattice cell card (also used in the lattice)
        latticeCardUniverse = -1
        if(latticeCard.params.has_key('U')):
            latticeCardUniverse = latticeCard.params['U']
        
        #latticeCellCardWithoutColor = self.buildSubGeometry(latticeCard.fullGeometry, latticeCard, {}, False)
        #latticeCellCard = self.buildSubGeometry(latticeCard.fullGeometry, latticeCard, {}, True)#latticeCard.getPovRayArgs())
        #parentCardGeometry = self.buildSubGeometry(parentCard.fullGeometry, parentCard,{}, True)# parentCard.getPovRayArgs())
        
        
        #-----------------------------------------------
        # RECTANGULAR LATTICE
        #-----------------------------------------------
        if (latticeCard.typeLAT == 1):
            
            if (not bbParent):
                offset = self.getRectangularOffset(parentCard)
                noInf = True
                for i in range(0,len(offset)):
                    if (offset[i] == 'inf'):
                        noInf = False
                        
                if (noInf):
                    bbParent = BoundingBox.BoundingBox(offset[0],offset[1],offset[2],offset[3],offset[4],offset[5])
                
            bb = self.getBoundingBoxOfGeometry(latticeCard.fullGeometry)
            offset = self.getRectangularOffset(latticeCard)
            
            if (offset[0] != 'inf' and offset[3] != 'inf'):
                offsetX = offset[3] - offset[0]
            else:
                offsetX = 0
            if (offset[1] != 'inf' and offset[4] != 'inf'):
                offsetY = offset[4] - offset[1]
            else:
                offsetY = 0
            if (offset[2] != 'inf' and offset[5] != 'inf'):
                offsetZ = offset[5] - offset[2]
            else:
                offsetZ = 0
            
            hasBB = True    
            for o in offset:
                if o == 'inf':
                    hasBB = False
                    
            if hasBB:
                bb = BoundingBox.BoundingBox(offset[0],offset[1],offset[2],offset[3],offset[4],offset[5])

            # check for infinite direction
            if (latticeCard.minK == 0 and latticeCard.maxK == 0):
                if (not hasBB or not bbParent):
                    raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + "): Bounding box of lattice card must be known in order to fill the lattice correctly."))
                    minK = 0
                    maxK = 0
                else:
                    if (bbParent):
                        width = offset[5] - offset[2]
                        widthParent = bbParent.maxZ - bbParent.minZ
                        amount = widthParent/width
                        minK =  -int(math.floor(amount/2))
                        maxK =  int(math.floor(amount/2))
            else:
                minK = latticeCard.minK
                maxK = latticeCard.maxK
                
            if (latticeCard.minJ == 0 and latticeCard.maxJ == 0):
                if (not hasBB or not bbParent):
                    raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + "): Bounding box of lattice card must be known in order to fill the lattice correctly."))
                    minJ = 0
                    maxJ = 0
                else:
                    if (bbParent):
                        width = offset[4] - offset[1]
                        widthParent = bbParent.maxY - bbParent.minY
                        amount = widthParent/width
                        minJ =  -int(math.floor(amount/2))
                        maxJ =  int(math.floor(amount/2))
            else:
                minJ = latticeCard.minJ
                maxJ = latticeCard.maxJ
                
            print "minJ: " + str(minJ)
            print "maxJ: " + str(maxJ)
            
            if (latticeCard.minI == 0 and latticeCard.maxI == 0):
                if (not hasBB or not bbParent):
                    raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + "): Bounding box of lattice card must be known in order to fill the lattice correctly."))
                    minI = 0
                    maxI = 0
                else:
                    if (bbParent):
                        width = offset[3] - offset[0]
                        widthParent = bbParent.maxX - bbParent.minX
                        amount = widthParent/width
                        minI =  -int(math.floor(amount/2))
                        maxI =  int(math.floor(amount/2))
            else:
                minI = latticeCard.minI
                maxI = latticeCard.maxI
                
            print "minI: " + str(minI)
            print "maxI: " + str(maxI)
                
            
            # START LOOP        
            universeCounter = 0  # counter for the current universe position in the lattice (increments for every build lattice item
            # Loop through the entire lattice and build element per element and at it to the 'lattice' list
            for k in range(minK, maxK+1):
                universeCounter = 0
                for j in range(minJ, maxJ+1):
                    for i in range(minI, maxI+1):
                        args = {}
                        args['translate'] = povray.Vector(i*offsetX, j*offsetY, k*offsetZ)
                        
                        # if the lattice element contains the current cell universe => build cell geometry
                        if ( (latticeCardUniverse != -1) and (latticeCard.latUniverses[universeCounter] == latticeCardUniverse)):
                            if (self.useMacros):
                                declareString = "Lat" + str(latticeCard.number) + "_U" + str(latticeCard.latUniverses[universeCounter])
                                hasKey = False
                                for i in range(0, len(self.declaredUniverses)):
                                    if (self.declaredUniverses[i][1] == str(declareString)):
                                        hasKey = True
                                if (not hasKey):
                                    universe = self.buildSubGeometry(latticeCard.fullGeometry, latticeCard, {}, True)
                                    if (universe):
                                        universe = povray.Object(universe, povray.BoundingBox(bb.buildPOVRay()))
                                        self.declaredUniverses.append([depth, str(declareString),povray.Declare(declareString,  universe)])
                                if (args.has_key('translate')):
                                    trans = args['translate']
                                else:
                                    trans = povray.Vector(0, 0, 0)
                                if (args.has_key('rotate')):
                                    rotate = args['rotate']
                                else:
                                    rotate = povray.Vector(0, 0, 0)
                                
                                found = False
                                for decUni in self.declaredUniverses:
                                    if decUni[1] == str(declareString):
                                        found = True
                                        break
        
                                if found:
                                    universe = povray.Instance(str(declareString), [trans, rotate])
                                else:
                                    universe = 0
                                
                                if (universe):
                                    it = povray.Object(universe)
                                else:
                                    it = 0
                            else: # no macros
                                it = self.buildSubGeometry(latticeCard.fullGeometry, latticeCard, args, True)
                                if (it):
                                    it = povray.Object(it, povray.BoundingBox(bb.buildPOVRay()))
                        
                        else:
                            if (self.useMacros):
                                declareString = "Lat" + str(latticeCard.number) + "_U" + str(latticeCard.latUniverses[universeCounter])
                                hasKey = False
                                for i in range(0, len(self.declaredUniverses)):
                                    if (self.declaredUniverses[i][1] == str(declareString)):
                                        hasKey = True
                                if (not hasKey):
                                    universe = self.buildUniverse(latticeCard.latUniverses[universeCounter], latticeCard.number, depth,{} , buildVoid, bb.buildPOVRay())
                                    self.declaredUniverses.append([depth, str(declareString),povray.Declare(declareString,  universe)])
                                
                                if (args.has_key('translate')):
                                    trans = args['translate']
                                else:
                                    trans = povray.Vector(0, 0, 0)
                                if (args.has_key('rotate')):
                                    rotate = args['rotate']
                                else:
                                    rotate = povray.Vector(0, 0, 0)
                                universe = povray.Instance(str(declareString), [trans, rotate])
                                
                                if (universe):
                                    it = povray.Object(universe)#povray.Object(universe, **args)#bbLatticeItem, clippedBy2, **args)
                                else:
                                    it = 0
                                
                            else: # no macros
                                universe = self.buildUniverse(latticeCard.latUniverses[universeCounter], latticeCard.number, depth,{} , buildVoid, bb.buildPOVRay())
                                if (universe):
                                    if (universe.kwargs.has_key('translate')):
                                            print "ERROR: Two translations at the same time"
                                    for a in args.keys():
                                        universe.kwargs[a] = args[a]    
                                    it = universe
                                else:
                                    it = 0
                        # only add item if it is valid              
                        if (it):
                            lattice.append(it)

                        universeCounter = universeCounter + 1
                    # end i
                # end j
            # end k
        # end (latticeCard.typeLAT == 1)
                    
        #-----------------------------------------------
        # HEXAGONAL LATTICE
        #-----------------------------------------------    
        if (latticeCard.typeLAT == 2):
            hexOffset = self.getHexOffset(latticeCard)
            if (not hexOffset):
                raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + " LAT=2): HexOffset could not be calculated"))
                return
            offsetZ = [0, 0, latticeCard.latticeWidth[2]]
            # normal = [0, 1, 0]
            # angle = 30 * math.pi / 180.0
            offsetX = 0
            offsetY = 0
            b = math.fabs(hexOffset['b']) # pitch
            a = math.fabs(hexOffset['a']) # length of a hexagon plane
            #c = math.fabs(hexOffset['a']) # length to an angle of the hexagon
            offsetX = hexOffset[hexOffset['x']]
            offsetY = hexOffset[hexOffset['y']]
            offsetZ = hexOffset[hexOffset['z']]
            offsetX_extra = 0
            offsetY_extra = 0
            offsetZ_extra = 0
            
            if (not bbParent):
                print parentCard
                offset = self.getRectangularOffset(parentCard)
                noInf = True
                for i in range(0,len(offset)):
                    if (offset[i] == 'inf'):
                        noInf = False
                        
                if (noInf):
                    bbParent = BoundingBox.BoundingBox(offset[0],offset[1],offset[2],offset[3],offset[4],offset[5])
            
            # check for infinite direction
            if (latticeCard.minK == 0 and latticeCard.maxK == 0):
                if (not hexOffset or not bbParent):
                    raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + "): Bounding box of lattice card must be known in order to fill the lattice correctly."))
                    minK = 0
                    maxK = 0
                else:
                    if (bbParent):
                        width = offsetZ
                        widthParent = bbParent.maxZ - bbParent.minZ
                        amount = widthParent/width
                        minK =  -int(math.floor(amount/2))
                        maxK =  int(math.floor(amount/2))
            else:
                minK = latticeCard.minK
                maxK = latticeCard.maxK
                
            print "minK: " + str(minK)
            print "maxK: " + str(maxK)
                
            if (latticeCard.minJ == 0 and latticeCard.maxJ == 0):
                if (not hexOffset or not bbParent):
                    raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + "): Bounding box of lattice card must be known in order to fill the lattice correctly."))
                    minJ = 0
                    maxJ = 0
                else:
                    if (bbParent):
                        width = offsetY
                        widthParent = bbParent.maxY - bbParent.minY
                        amount = widthParent/width
                        minJ =  -int(math.floor(amount/2))
                        maxJ =  int(math.floor(amount/2))
            else:
                minJ = latticeCard.minJ
                maxJ = latticeCard.maxJ
                
            print "minJ: " + str(minJ)
            print "maxJ: " + str(maxJ)
            
            if (latticeCard.minI == 0 and latticeCard.maxI == 0):
                if (not hexOffset or not bbParent):
                    raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + "): Bounding box of lattice card must be known in order to fill the lattice correctly."))
                    minI = 0
                    maxI = 0
                else:
                    if (bbParent):
                        width = offsetX
                        widthParent = bbParent.maxX - bbParent.minX
                        amount = widthParent/width
                        minI =  -int(math.floor(amount/2))
                        maxI =  int(math.floor(amount/2))
            else:
                minI = latticeCard.minI
                maxI = latticeCard.maxI
                
            print "minI: " + str(minI)
            print "maxI: " + str(maxI)
            
            universeCounter = 0  # counter for the current universe position in the lattice (increments for every build lattice item
            for k in range(minK, maxK+1):
                for j in range(minJ, maxJ+1):
                    for i in range(minI, maxI+1):
                        # UPDATE OFFSETS
                        offset = 0
                        t = 0
                        if (hexOffset['x'] == 'a'):
                            offset = b/2 
                            t = i
                        elif (hexOffset['y'] == 'a'):

                            offset = b/2
                            # a_as = 'y'
                            t = j
                        elif (hexOffset['z'] == 'a'):
                            offset = b/2
                            # a_as = 'z'
                            t = k
                        else:
                            raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + " LAT=2): HexOffset could not be calculated, no 'a' found in hex offset"))
                            return
                        
                        offsetX_extra = 0
                        offsetY_extra = 0
                        offsetZ_extra = 0
                        if (hexOffset['x'] == 'b'):
                            offsetX_extra = offset
                        elif (hexOffset['y'] == 'b'):
                            offsetY_extra = offset
                        elif (hexOffset['z'] == 'b'):
                            offsetZ_extra = offset
                        else:
                            raise(Exception("ERROR (Build Cell " + str(latticeCard.number) + " LAT=2): HexOffset could not be calculated, no 'a' found in hex offset"))
                            return
                        
                        args = {}
                        args['translate'] = povray.Vector(i*offsetX+t*offsetX_extra, j*offsetY+t*offsetY_extra, k*offsetZ+t*offsetZ_extra)
                    
                        # if the lattice element contains the current cell universe => build cell geometry
                        
                        if ( (latticeCardUniverse != -1) and (latticeCard.latUniverses[universeCounter] == latticeCardUniverse)):
                            if (self.useMacros):
                                declareString = "Lat" + str(latticeCard.number) + "_U" + str(latticeCard.latUniverses[universeCounter])
                                hasKey = False
                                for i in range(0, len(self.declaredUniverses)):
                                    if (self.declaredUniverses[i][1] == str(declareString)):
                                        hasKey = True
                                if (not hasKey):
                                    universe = self.buildSubGeometry(latticeCard.fullGeometry, latticeCard, {}, True)
                                    self.declaredUniverses.append([depth, str(declareString),povray.Declare(declareString,  universe)])
                                if (args.has_key('translate')):
                                    trans = args['translate']
                                else:
                                    trans = povray.Vector(0, 0, 0)
                                if (args.has_key('rotate')):
                                    rotate = args['rotate']
                                else:
                                    rotate = povray.Vector(0, 0, 0)
                                universe = povray.Instance(str(declareString), [trans, rotate])
                                
                                if (universe):
                                    it = povray.Object(universe)#povray.Object(universe, **args)#bbLatticeItem, clippedBy2, **args)
                                else:
                                    it = 0
                            else: # no macros
                                it = self.buildSubGeometry(latticeCard.fullGeometry, latticeCard, args, True)
                        
                        # the lattice element contains a universe, different from the current universe
                        else:
                            if (self.useMacros):
                                declareString = "Lat" + str(latticeCard.number) + "_U" + str(latticeCard.latUniverses[universeCounter])
                                hasKey = False
                                for i in range(0, len(self.declaredUniverses)):
                                    if (self.declaredUniverses[i][1] == str(declareString)):
                                        hasKey = True
                                if (not hasKey):
                                    universe = self.buildUniverse(latticeCard.latUniverses[universeCounter], latticeCard.number, depth,{} , buildVoid)
                                    self.declaredUniverses.append([depth, str(declareString),povray.Declare(declareString,  universe)])

                                if (args.has_key('translate')):
                                    trans = args['translate']
    
                                else:
                                    trans = povray.Vector(0, 0, 0)
                                if (args.has_key('rotate')):
                                    rotate = args['rotate']
                                else:
                                    rotate = povray.Vector(0, 0, 0)
                                universe = povray.Instance(str(declareString), [trans, rotate])
                                
                                if (universe):
                                    it = povray.Object(universe)#povray.Object(universe, **args)#bbLatticeItem, clippedBy2, **args)
                                else:
                                    it = 0
                                
                            else: # no macros   
                                universe = self.buildUniverse(latticeCard.latUniverses[universeCounter], latticeCard.number, depth,{}, buildVoid)
                                if (universe):
                                    for a in args.keys():
                                        universe.kwargs[a] = args[a]
                                    it = universe#povray.Object(universe, **args)#bbLatticeItem, clippedBy2, **args)
                                else:
                                    it = 0
                        if (it):
                            lattice.append(it)
                            
                        universeCounter = universeCounter + 1
                    # end i
                # end j
            # end k
        # end (latticeCard.typeLAT == 2)
            
        # combine the entire lattice in 1 big povray object
        # clippedBy = povray.ClippedBy("bounded_by")
        args = parentCard.getUniversePovRayArgs()
        if (len(lattice) > 0):
            #elements = []
            #if bbParent:
            #   elements.append(bbParent.buildPOVRay())
            #   elements.append(clippedBy)
            return povray.Union(*lattice)#, **parentCard.getPovRayArgs())#, **args)
        else:
            print "ERROR (buildLattice) => lattice only consists of 1 povray element"
            lattice = lattice[0]
            return lattice
        
    # ==> getBoundingBoxOfGeometryOfCell(card):
    # Returns the bounding box of a cell
    #------------------------------------------------------------------------------------------------------------------
    def getBoundingBoxOfGeometryOfCell(self, card):
        cellCard = self.cellCards[int(card)]
        return self.getBoundingBoxOfGeometry(cellCard.fullGeometry)

    # ==> getBoundingBoxOfGeometry(geometry):
    # Returns the bounding box of a geometry (string)
    #------------------------------------------------------------------------------------------------------------------
    def getBoundingBoxOfGeometry(self, geometry):
        if (re.search('\:', geometry)):
            return 0 # couldn't calculate bounding box if there if the cell is combined of a union
        else:
            # single element or intersection
            intersection = re.split('[\s]+', geometry)
            container.Container.remove_values_from_list(intersection, "")
            if (len(intersection) is 0 ):
                return 0 # no element in geometry
            else:
                # totalBoundingBox will grow additive for every surface in it
                totalBoundingBox = BoundingBox.BoundingBox()
                totalBoundingBox.exists = True
                for geom in intersection:
                    if (re.search('[a-z,A-Z]+', geom)):
                        raise(Exception("ERROR (Build Geometry " + str(geometry) + "): Unable to find bounding box for subsurface " + str(geom)))
                        return 0
                    else:
                        surface = geom
                        if (surface[0] == '-'):
                            surface = surface[1:]
                        elif (surface[0] == '#'):
                            return 0 # unable to find bounding box for complements
                        else:
                            surface = surface
                        print geometry
                        bb =  self.surfaceCards[int(surface)].getBoundingBox()
                        if (bb):
                            totalBoundingBox.append(bb)
                        else:
                            return 0        
                return totalBoundingBox         
        return 0
                    
                
        
    # ==> buildSubGeometry(geometry, card, povRayArgs={}, useColor=True, scale= 1.0):
    # build the geometry or sub-geometry of a cell to a POV Ray element
    # split the geometry in unions and intersections and bundle the surfaces in a correct way
    # output = povray object
    #       geometry: string that defines the geometry to build
    #       card: card of which the geometry is
    #       povRayArgs: optional extra parameters for povary (i.e. transformations)
    #       useColor: build the povray object with or without colors
    #       scale: scale the output povray object
    #------------------------------------------------------------------------------------------------------------------
    def buildSubGeometry(self, geometry, card, povRayArgs={}, useColor=True, scale= 1.0):
        # seek for unions
        
        if (re.search('\:', geometry)):
            
            totalBoundingBox = BoundingBox.BoundingBox()
            totalBoundingBox.exists = True
            union = re.split('[\:]', geometry)
            unionList = []

            for surface in union:
                if (surface == ""):
                    pass #ignore empty surfaces
                elif (re.search('[a-z,A-Z]+', surface)): # if the subsurface is subsurface identifier
                    # subsurface found
                    # request the already calculated subsurface out of the subsurfaceMapInv
                    if (surface[0] == '-'):
                        if (useColor):
                            povItem = card.subsurfaceMapInvColor[surface[1:].strip()]
                            if povItem:
                                unionList.append(povItem)
                        else:
                            povItem = card.subsurfaceMapInv[surface[1:].strip()]
                            if povItem:
                                unionList.append(povItem)
                    elif(surface[0] == '#'):
                        if (useColor):
                            povItem = card.subsurfaceMapInv[surface[1:].strip()]
                            if povItem:
                                unionList.append(povray.Object(povItem,'inverse'))
                        else:
                            povItem = card.subsurfaceMapInv[surface[1:].strip()]
                            if povItem:
                                unionList.append(povray.Object(povItem,'inverse'))
                    else:
                        if (useColor):
                            povItem = card.subsurfaceMapInvColor[surface.strip()]
                            if povItem:
                                unionList.append(povItem)
                        else:
                            povItem = card.subsurfaceMapInv[surface.strip()]
                            if povItem:
                                unionList.append(povItem)
                else: # a normal surface
                    bb = BoundingBox.BoundingBox()
                    surf = self.buildCellSurface(str(surface), card, bb, useColor)
                    if (totalBoundingBox.exists and bb.exists):
                        totalBoundingBox.append(bb)
                    else:
                        totalBoundingBox.exists = False
                    if (surf):
                        unionList.append(surf)
            
            # if there are more than 1 elements => combine it in a unions structure, otherwise just an object
            if (len(unionList) > 1):
                if (scale != 1.0):
                    unionList.append('scale ' + str(scale))
                return povray.Union(*unionList,  **povRayArgs)
            elif (len(unionList) == 1):
                if (scale != 1.0):
                    unionList.append('scale ' + str(scale)) 
                return povray.Object(*unionList,  **povRayArgs)
            else:
                return 0
        # otherwise intersection (unless the number of surfaces is 1)
        else:
            
            totalBoundingBox = BoundingBox.BoundingBox()
            totalBoundingBox.exists = True
            geometry = geometry.replace('# ', "#")
            intersection = re.split('[\s]+', geometry)
            intersectionList = []
            diff = self.buildIntersectionAsDifference(intersection, card, povRayArgs, useColor)
            if (diff):
                pass
            
            for surface in intersection:
                if (surface == ""):
                    pass #ignore empty surfaces
                elif (re.search('[a-z,A-Z]+', surface)): # if the subsurface is subsurface identifier
                    # subsurface found
                    # request the already calculated subsurface out of the subsurfaceMapInv
                    if (surface[0] == '-'):
                        if (useColor):
                            povItem = card.subsurfaceMapInvColor[surface[1:].strip()]
                            if (povItem):
                                intersectionList.append(povItem)
                        else:
                            povItem = card.subsurfaceMapInv[surface[1:].strip()]
                            if (povItem):
                                intersectionList.append(povItem)
                    elif(surface[0] == '#'):
                        if (useColor):
                            povItem = card.subsurfaceMapInvColor[surface[1:].strip()]
                            if (povItem):
                                intersectionList.append(povray.Object(povItem,'inverse'))
                        else:
                            povItem = card.subsurfaceMapInv[surface[1:].strip()]
                            if (povItem):
                                intersectionList.append(povray.Object(povItem,'inverse'))
                    else:
                        if (useColor):
                            povItem = card.subsurfaceMapInvColor[surface.strip()]
                            if (povItem):
                                intersectionList.append(povItem)
                        else:
                            povItem = card.subsurfaceMapInv[surface.strip()]
                            if (povItem):
                                intersectionList.append(povItem)
                else: # a normal surface to interpret
                    bb = BoundingBox.BoundingBox()
                    surf = self.buildCellSurface(str(surface), card, bb, useColor)
                    if (totalBoundingBox.exists and bb.exists):
                        totalBoundingBox.append(bb)
                    else:
                        totalBoundingBox.exists = False
                    if (surf):
                        intersectionList.append(surf)
            
            # if there is more than 1 element in the intersectionList, combine it as an Intersection, otherwise just an Object
            if (len(intersectionList) == 1):
                if (scale != 1.0):
                    intersectionList.append('scale ' + str(scale))
                return povray.Object(*intersectionList,  **povRayArgs)
            elif (len(intersectionList) > 1):
                if (scale != 1.0):
                    intersectionList.append('scale ' + str(scale))
                bb = BoundingBox.BoundingBox()
                if (totalBoundingBox.exists):
                    intersectionList.append(povray.BoundingBox(totalBoundingBox.buildPOVRay()))
                    intersectionList.append(povray.ClippedBy("bounded_by"))
                return povray.Intersection(*intersectionList,  **povRayArgs)
            else:
                return 0

    # ==> buildIntersectionAsDifference(intersection, card, povRayArgs, useColor):
    # Building the intersectionList as a Difference object in povray
    #       intersection: list of surfaces to intersect
    #       card: cell card of which the intersection is
    #       povRayArgs: extra povray options to add (i.e. transformations)
    #       useColor: build with or without colors
    #------------------------------------------------------------------------------------------------------------------
    def buildIntersectionAsDifference(self, intersection, card, povRayArgs, useColor):
        objects = []
        differences = []
        totalBoundingBox = BoundingBox.BoundingBox()
        totalBoundingBox.exists = True
        # check if you can rewrite the intersection by a difference
        for surface in intersection:
            if (surface == ""):
                pass
            elif (re.search('[a-z,A-Z]+', surface)):
                # subsurface found
                # request the already calculated subsurface out of the subsurfaceMapInv
                if (surface[0] == '-'):
                    if (useColor):
                        povItem = card.subsurfaceMapInvColor[surface[1:].strip()]
                        if povItem:
                            differences.append(povItem)
                    else:
                        povItem = card.subsurfaceMapInv[surface[1:].strip()]
                        if povItem:
                            differences.append(povItem)
                elif(surface[0] == '#'):
                    if (useColor):
                        povItem = card.subsurfaceMapInvColor[surface[1:].strip()]
                        if povItem:
                            differences.append(povray.Object(povItem,'inverse'))
                    else:
                        povItem = card.subsurfaceMapInv[surface[1:].strip()]
                        if povItem:
                            differences.append(povray.Object(povItem,'inverse'))
                else:
                    if (useColor):
                        povItem = card.subsurfaceMapInvColor[surface.strip()]
                        if povItem:
                            differences.append(povItem)
                    else:
                        povItem = card.subsurfaceMapInv[surface.strip()]
                        if povItem:
                            differences.append(povItem)
            else:
                if (surface[0] == '-'):
                    bb = BoundingBox.BoundingBox()
                    surf = self.buildCellSurface(str(surface), card, bb)
                    if (surf):
                        objects.append(surf)
                        if (totalBoundingBox.exists and bb.exists):
                            totalBoundingBox.append(bb)
                        else:
                            totalBoundingBox.exists = False
                elif(surface[0] == '#'):
                    #print "ERROR (buildIntersectionAsDifference): unable to parse complement # in difference instruction"
                    return 0
                else:
                    bb = BoundingBox.BoundingBox()
                    surf = self.buildCellSurface(str('-' + surface), card, bb, useColor)
                    if (surf):
                        differences.append(surf)
                        if (totalBoundingBox.exists and bb.exists):
                            totalBoundingBox.append(bb)
                        else:
                            totalBoundingBox.exists = False
                    
        container.Container.remove_values_from_list(objects, 0)
        container.Container.remove_values_from_list(differences, 0)
        if (len(differences) > 0):
            differences.append("cutaway_textures")
            if (totalBoundingBox.exists):
                differences.append(povray.BoundingBox(totalBoundingBox.buildPOVRay()))
                differences.append(povray.ClippedBy("bounded_by"))

        # if there is more than 1 element in the list, combine it as an Difference, otherwise just an Object
        if (len(objects) == 0):
            return 0
        elif (len(objects) == 1):
            if (len(differences) == 0):
                return povray.Union(objects[0], **povRayArgs)
            else:
                return povray.Difference(objects[0], *differences, **povRayArgs)
        else:
            return povray.Difference(povray.Union(*objects), *differences, **povRayArgs)


    # ==> buildCellSurface(surface, card, bb,  useColor=True):
    # Building the cell surface to a povray object
    # The surface could be of the form A, -A and #A (string!)
    #       surface: surface instruction (can still have an operator in front of it)
    #       card: cell card of which the surface is part of
    #       bb: output bounding box of the surface (.exists will set to true if the bb is found)
    #       useColor: build with or without colors
    #------------------------------------------------------------------------------------------------------------------
    def buildCellSurface(self, surface, card, bb, useColor=True):
        # the surface could be of the form A, -A and #A (string!)
        bbExists = True
        useInverse = False
        surfaceCard = surface
        surface = surface.replace(" ", "")
        if (len(surface) == 0):
            return 0
        elif (surface[0] == '-'):
            useInverse = True
            surfaceCard = surface[1:]
        elif (surface[0] == '#'):
            bbExists = False
            # use the inverse of a cell and not a surface (see manual MCNPX for # operator)
            targetCellCard = surface[1:]
            #self.complementCard = card
            if (self.buildTexture(card.material, True)):
                self.complementCard = card
                povItem = self.buildCell(int(targetCellCard), card, 0, False, bb)
                self.complementCard = None
                return povray.Object(povItem, 'inverse')
            else:
                return 0
        else:
            bbExists = False
        
        if (bbExists):  
            print "jaja"
            print card
            box = self.getBoundingBoxOfGeometry(surface)
            # copy the bounding box in bb
            if (box):
                bb.clone(box)
                bb.exists = 1       
        else:
            bb.exists = 0
        
        if (not self.materialCards.has_key((card.material)) and (card.material != 0)):
            raise(Exception("ERROR (Build Cell " + str(card.number) + " Material): Material " + str(card.material) + " of cell " + str(card.number) + " not known!"))
        return self.buildSurfaceCard(int(surfaceCard), card.material, useColor, useInverse)
    # END buildCellSurface(self, surface, card, bb, useColor=True):

    
    # ==> buildUniverse(universeNumber, parent, depth, povRayArgs = {}, buildVoid = False, clippedBy=0, useColor = True):
    # Builds all the cells that are contained in the universe
    # Clip the universe by the parent card geometry and bounding box
    #       universeNumber: number of the universe
    #       parent: card number of the parent card
    #       depth: depth in the universe hierarchy
    #       povRayArgs: extra arguments that must be given to the pov ray object
    #       buildVoid: buid cells with empty materials or not
    #       clippedBy: optional clipped by some parent geometry
    #       useColor: render the universe with colors or not
    #------------------------------------------------------------------------------------------------------------------ 
    def buildUniverse(self, universeNumber, parent, depth, povRayArgs = {}, buildVoid = False, clippedBy=0, useColor = True):
        if (depth+1 > 22):
            raise(Exception("ERROR: Reached a maximum recursion depth of 22 (call from parent " + str(parent) + " to universe " + str(universeNumber)));
            return
        if (not self.universes.has_key(str(universeNumber))):
            raise(Exception("ERROR (Build Universe " + str(universeNumber) + "): Universe " + str(universeNumber) + " not found."))
            return

        if (self.useMacros):
            hasKey = False
            for i in range(0, len(self.declaredUniverses)):
                if (self.declaredUniverses[i][1] == str(universeNumber)):
                    hasKey = True
            if (not hasKey):
                universeItems = []
                for cellCard in self.universes[str(universeNumber)]:
                    item = self.buildCell(cellCard, parent, depth+1, useColor, buildVoid)
                    if item:
                        universeItems.append(povray.Object(item))
                self.declaredUniverses.append([depth, str(universeNumber),povray.Declare("universe" + str(universeNumber), *universeItems)])
            if (povRayArgs.has_key('translate')):
                trans = povRayArgs['translate']
            else:
                trans = povray.Vector(0, 0, 0)
            if (povRayArgs.has_key('rotate')):
                rotate = povRayArgs['rotate']
            else:
                rotate = povray.Vector(0, 0, 0)

            universeItems = [povray.Instance("universe" + str(universeNumber), [trans, rotate])]
            uni =  povray.Object(*universeItems )
        else: # no macros
            universeItems = []
            for cellCard in self.universes[str(universeNumber)]:
                item = self.buildCell(cellCard, parent, depth+1, useColor, buildVoid)
                if item:
                    universeItems.append(povray.Object(item,**povRayArgs))
            uni =  povray.Object(*universeItems)
            
        # if there is no parent defined, just return the combined universe (without clipping some parent geometry
        if (not parent):
            return uni
        parentCard = self.getCellCard(parent)
        # if no parent card found, return also the combined universe
        if (not parentCard):
            return uni 
        
        useDifference = False # define if you want to use difference or clipped by to intersect with the parent geometry
        
        # request the parent card geometry and bounding box
        parentCardGeometry = self.buildSubGeometry(parentCard.fullGeometry, parentCard, {}, False)
        bbParent = self.getBoundingBoxOfGeometryOfCell(parentCard.number)
        
        if (useDifference):
            lattice = [povray.Union(*universeItems)]
            lattice.append(povray.Object(parentCardGeometry, 'inverse'))
            
            if (bbParent):
                lattice.append(povray.BoundingBox(bbParent.buildPOVRay()))
            lattice.append('cutaway_textures')
            int =  povray.Difference(*lattice)
            
        else: # use clipped by instead of difference
            lattice = universeItems#[uni]#universeItems
            if (bbParent):
                lattice.append(povray.BoundingBox(bbParent.buildPOVRay()))
            if (clippedBy):
                clippedBy2 = povray.ClippedBy(clippedBy)
            else:
                clippedBy2 = povray.ClippedBy(parentCardGeometry)
            lattice.append(clippedBy2)
        
            if (len(lattice) > 1):
                int =  povray.Union(*lattice)
            elif (len(lattice) == 1):
                int =  povray.Object(*lattice)
                    
        return int
    
    
    
    

#######################################################################################################################
## PARSING SURFACES
#######################################################################################################################
    
    # ==> parseSurfaces():
    # Interpret every line of the surface block
    # Get surface number and search for type surface (mnemonic)
    # Of the form: 
    #       j k a list (j = surface number, a = mnemonic, list = data)
    #------------------------------------------------------------------------------------------------------------------
    def parseSurfaces(self):

        # read all surface cards out of file and put them in surfaceCards
        for line in self.surfaceBlock:
        
            surfaceCard = re.split('[\s]+', line)
            container.Container.remove_values_from_list(surfaceCard, '')
            
            translation = 0
            rotation = 0
            mnemonic = 0
        
            if (re.match('[\d]+', str(surfaceCard[1]))):
                surfaceCardData =  surfaceCard[3:len(surfaceCard)]
                translation =  self.transformationCards[surfaceCard[1]][0:3]
                rotation =  self.transformationCards[surfaceCard[1]][3:]
                mnemonic = surfaceCard[2]
            else:
                surfaceCardData =  surfaceCard[2:len(surfaceCard)]
                mnemonic = surfaceCard[1]
                
            surfaceCardData = [float(i) for i in surfaceCardData]
            self.surfaceCards[int(surfaceCard[0])] = SurfaceCard.SurfaceCard(int(surfaceCard[0]), mnemonic,surfaceCardData)
            self.surfaceCards[int(surfaceCard[0])].translation = translation
            self.surfaceCards[int(surfaceCard[0])].rotation = rotation
    # END parseSurfaces(self):
            
    # ==> getMaterialColor(material):
    # Hardcoded map from material numbers to povray colors
    #------------------------------------------------------------------------------------------------------------------ 
    def getMaterialColor(self, material):
        materialColors = {}
        materialColors[0] = 'White'
        materialColors[1] = 'Wheat'
        materialColors[2] = 'Gray65' #'Green'
        materialColors[3] = 'White' #Blue'
        materialColors[4] = 'NeonPink' #Coral'
        materialColors[5] = 'Gray90'
        materialColors[6] = 'Gray85'
        materialColors[7] = 'Orange'
        materialColors[8] = 'Gray45'
        materialColors[9] = 'Black'
        materialColors[10] = 'Blue'
        materialColors[11] = 'OrangeRed'
        materialColors[12] = 'SlateBlue'
        materialColors[13] = 'Red'
        materialColors[14] = 'Clear'
        materialColors[15] = 'Blue'
        materialColors[16] = 'DimGray'
        materialColors[17] = 'Gray35'
        materialColors[18] = 'Gray30'
        materialColors[19] = 'NeonPink'
        materialColors[20] = 'Cyan'
        materialColors[21] = 'NeonPink'

        if (material == 0):
            return "White"
        elif (material > 21):
            return ""
        else:
            return materialColors[material]



#######################################################################################################################
## BUILDING SURFACES
#######################################################################################################################
    
    # ==> buildTexture(material)
    def buildTexture(self, material, useColor):
        # if the surface can be build with colors and the material is defined, request it from the colorMap
        # if transparency if entirely 0 => does not render the surface
        if (material != "" and int(material) >= 0 and useColor):
            if (self.colorMap.has_key(str(material))):
                materialColor = self.colorMap[str(material)].toPovRay() #self.getMaterialColor(material)
                transparancy = self.colorMap[str(material)].alpha
                if (transparancy == 0.0):
                    return 0
            else:
                useColor = False
        else:
            useColor = False
        material = ""
        
        if (useColor):
            if (transparancy != 1.0):
                pigment = povray.Pigment(  transmit=1.0-transparancy,color=materialColor )
            else:
                pigment = povray.Pigment(  color=materialColor )
            texture = povray.Texture(pigment)       
        else:
            texture = ""
        return texture
            
    
    # ==> buildSurfaceCard(surfaceNumber, material, useColor, inverse=False, bb=0, args={}):
    # build a surface card to a POV Ray element 
    #       surfaceNumber: surface to build
    #       material: material in which the surface must be build
    #       useColor: whether or not color is used
    #       inverse: if true => swap inside and outside of object
    #       bb: optional bounding box => if defined the bounding box is used for the surface
    #       args: optional extra parameters for the surface
    #------------------------------------------------------------------------------------------------------------------ 
    def buildSurfaceCard(self, surfaceNumber, material, useColor, inverse=False, bb=0):
        if (surfaceNumber not in self.surfaceCards):
            raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " not known"))
            return
        
        surfaceCard = self.surfaceCards[surfaceNumber]

        if (not inverse):
            inverseWrite = 'inverse'
        else:
            inverseWrite = ''
            
        if (self.complementCard != None):
            print str(material) + " to " + str(self.complementCard.material)
            material = self.complementCard.material
            
            print "material complement"

        # if the surface can be build with colors and the material is defined, request it from the colorMap
        # if transparency if entirely 0 => does not render the surface
        if (material != "" and int(material) >= 0 and useColor):
            if (self.colorMap.has_key(str(material))):
                materialColor = self.colorMap[str(material)].toPovRay() #self.getMaterialColor(material)
                transparancy = self.colorMap[str(material)].alpha
                if (transparancy == 0.0):
                    return 0
            else:
                useColor = False
        else:
            useColor = False
        material = ""
        
        if (useColor):
            if (transparancy != 1.0):
                pigment = povray.Pigment(  transmit=1.0-transparancy,color=materialColor )
            else:
                pigment = povray.Pigment(  color=materialColor )
            texture = povray.Texture(pigment)       
        else:
            texture = ""
            
        povrayObject = 0 # output object
        # PLANE
        # P -  with given normal and D  (3000 P A B C D)
        if (surfaceCard.mnemonic == 'P' or surfaceCard.mnemonic == 'p'):
            if (len(surfaceCard.data) != 4):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type P has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 4)"))
                return
            povrayObject = povray.Plane(povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]), surfaceCard.data[3]/math.sqrt(math.pow(surfaceCard.data[0],2)+math.pow(surfaceCard.data[1],2)+math.pow(surfaceCard.data[2],2))
            , texture, material, material, inverseWrite);
            
        # PX - PLANE with normal to x-axis and D (3000 PX D)
        elif (surfaceCard.mnemonic == 'PX' or surfaceCard.mnemonic == 'px'):
            if (len(surfaceCard.data) != 1):
                print str(len(surfaceCard.data))
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type PX has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 1)"))
                return
            povrayObject = povray.Plane(povray.Vector(1.0, 0.0, 0.0), surfaceCard.data[0]
            , texture, material, inverseWrite)
            
        # PY - PLANE with normal to y-axis and D (3000 PY D)
        elif (surfaceCard.mnemonic == 'PY' or surfaceCard.mnemonic == 'py'):
            if (len(surfaceCard.data) != 1):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type PY has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 1)"))
                return
            povrayObject = povray.Plane(povray.Vector(0.0, 1.0, 0.0), surfaceCard.data[0]
            , texture, material, inverseWrite)
            
        # PZ - PLANE with normal to z-axis and D (3000 PZ D)
        elif (surfaceCard.mnemonic == 'PZ' or surfaceCard.mnemonic == 'pz'):
            if (len(surfaceCard.data) != 1):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type PZ has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 1)"))
                return
            povrayObject = povray.Plane(povray.Vector(0.0, 0.0, 1.0), surfaceCard.data[0], texture, material, inverseWrite)
            

        # SPHERE
        # SO -  centered at Origin and radius R  (3000 SO R)
        if (surfaceCard.mnemonic == 'SO' or surfaceCard.mnemonic == 'so' or surfaceCard.mnemonic == 's0'):
            if (len(surfaceCard.data) != 1):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type SO has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 1)"))
                return
            povrayObject = povray.Sphere(povray.Vector(0.0, 0.0, 0.0), surfaceCard.data[0]
                , texture, material, inverseWrite)
            
        # S -  General  (3000 SO X Y Z R)
        elif (surfaceCard.mnemonic == 'S' or surfaceCard.mnemonic == 's'):
            if (len(surfaceCard.data) != 4):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type S has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 4)"))
                return
            povrayObject = povray.Sphere(povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]), surfaceCard.data[3]
                , texture, material, inverseWrite)
            
        # SX -  Centered on x-axis  (3000 SX X R)
        elif (surfaceCard.mnemonic == 'SX' or surfaceCard.mnemonic == 'sx'):
            if (len(surfaceCard.data) != 2):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type SX has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 2)"))
                return
            povrayObject = povray.Sphere(povray.Vector(surfaceCard.data[0], 0.0, 0.0), surfaceCard.data[1]
                , texture, material, inverseWrite)
            
        # SY -  Centered on y-axis  (3000 SY Y R)
        elif (surfaceCard.mnemonic == 'SY' or surfaceCard.mnemonic == 'sy'):
            if (len(surfaceCard.data) != 2):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type SY has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 2)"))
                return
            povrayObject = povray.Sphere(povray.Vector(0.0, surfaceCard.data[0], 0.0), surfaceCard.data[1]
                , texture, material, inverseWrite)
            
        # SZ -  Centered on z-axis  (3000 SZ Z R)
        elif (surfaceCard.mnemonic == 'SZ' or surfaceCard.mnemonic == 'sz'):
            if (len(surfaceCard.data) != 2):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type SZ has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 2)"))
                return
            povrayObject = povray.Sphere(povray.Vector(0.0, 0.0, surfaceCard.data[0]), surfaceCard.data[1], texture, material, inverseWrite)
            
        
        # INFINITE CYLINDERS
        # http://www.povray.org/documentation/view/3.6.0/299/ (pov ray Quadric)
        # C/X -  infinite parallel to x-axis  (3000 C/X Y Z R)
        # (y-y')^2 + (z-z')^2 - R^2 = 0
        # <=> (0x^2 + y^2 + z^2) + (0xy + 0xz + 0yz) + (0x - 2y'y - 2z'z) + (-R^2 + y'^2 + z'^2)  = 0
        if (surfaceCard.mnemonic == 'C/X' or surfaceCard.mnemonic == 'c/x' or surfaceCard.mnemonic == 'c/X' or surfaceCard.mnemonic == 'C/x'):
            if (len(surfaceCard.data) != 3):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type C/X has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 3)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(0.0, 1.0, 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(0.0, -2*surfaceCard.data[0], -2*surfaceCard.data[1]),  # G, H, I
                -surfaceCard.data[2]*surfaceCard.data[2] + surfaceCard.data[0]*surfaceCard.data[0] + surfaceCard.data[1]*surfaceCard.data[1], 
                texture, material, inverseWrite)
            
        # C/Y -  infinite parallel to y-axis  (3000 C/Y X Z R)
        # (x-x')^2 + (z-z')^2 - R^2 = 0
        # <=> (x^2 + 0y^2 + z^2) + (0xy + 0xz + 0yz) + (-2x'x + 0'y - 2z'z) - R^2 = 0
        elif (surfaceCard.mnemonic == 'C/Y' or surfaceCard.mnemonic == 'c/y' or surfaceCard.mnemonic == 'c/Y' or surfaceCard.mnemonic == 'C/y'):
            if (len(surfaceCard.data) != 3):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type C/Y has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 3)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, 0.0, 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(-2*surfaceCard.data[0], 0.0, -2*surfaceCard.data[1]),  # G, H, I
                -surfaceCard.data[2]*surfaceCard.data[2] + surfaceCard.data[0]*surfaceCard.data[0] + surfaceCard.data[1]*surfaceCard.data[1], 
                texture, material, inverseWrite)
            
        # C/Z - infinite parallel to z-axis  (3000 C/Z X Y R)
        # (x-x')^2 + (y-y')^2 - R^2 = 0
        # <=> (x^2 + y^2 + 0z^2) + (0xy + 0xz + 0yz) + (-2x'x - 2y'y + 0z) - R^2 = 0
        elif (surfaceCard.mnemonic == 'C/Z' or surfaceCard.mnemonic == 'c/z' or surfaceCard.mnemonic == 'c/Z' or surfaceCard.mnemonic == 'C/z'):
            if (len(surfaceCard.data) != 3):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type C/Z has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 3)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, 1.0, 0.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(-2*surfaceCard.data[0], -2*surfaceCard.data[1], 0.0),  # G, H, I
                -surfaceCard.data[2]*surfaceCard.data[2] + surfaceCard.data[0]*surfaceCard.data[0] + surfaceCard.data[1]*surfaceCard.data[1], 
                texture, material, inverseWrite)
            
        # CX -  infinite on x-axis  (3000 CX R)
        # y^2 + z^2 - R^2 = 0
        # <=> (0x^2 + y^2 + z^2) + (0xy + 0xz + 0yz) + (0x - 0y + 0z) - R^2 = 0
        elif (surfaceCard.mnemonic == 'CX' or surfaceCard.mnemonic == 'cx' or surfaceCard.mnemonic == 'cX' or surfaceCard.mnemonic == 'Cx'):
            if (len(surfaceCard.data) != 1):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type CX has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 1)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(0.0, 1.0, 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(0.0, 0.0, 0.0),  # G, H, I
                -surfaceCard.data[0]*surfaceCard.data[0],
                texture, material, inverseWrite)
            
        # CY -  infinite on y-axis  (3000 CY R)
        # x^2 + z^2 - R^2 = 0
        # <=> (x^2 + 0y^2 + z^2) + (0xy + 0xz + 0yz) + (0x - 0y + 0z) - R^2 = 0
        elif (surfaceCard.mnemonic == 'CY' or surfaceCard.mnemonic == 'cy' or surfaceCard.mnemonic == 'cY' or surfaceCard.mnemonic == 'Cy'):
            if (len(surfaceCard.data) != 1):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type CY has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 1)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, 0.0, 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(0.0, 0.0, 0.0),  # G, H, I
                -surfaceCard.data[0]*surfaceCard.data[0],
                texture, material, inverseWrite)
            
        # CZ -  infinite on z-axis  (3000 CZ R)
        # x^2 + y^2 - R^2 = 0
        # <=> (x^2 + y^2 + 0z^2) + (0xy + 0xz + 0yz) + (0x - 0y + 0z) - R^2 = 0
        elif (surfaceCard.mnemonic == 'CZ' or surfaceCard.mnemonic == 'cz' or surfaceCard.mnemonic == 'cZ' or surfaceCard.mnemonic == 'Cz'):
            if (len(surfaceCard.data) != 1):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type CZ has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 1)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, 1.0, 0.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(0.0, 0.0, 0.0),  # G, H, I
                -surfaceCard.data[0]*surfaceCard.data[0],
                texture, material, inverseWrite)
            

        # INFINITE CONES        
        # http://www.povray.org/documentation/view/3.6.0/299/ (pov ray Quadric)
        # K/X -  infinite parallel to x-axis  (3000 K/X X Y Z t^2)      
        # (y-y')^2 + (z-z')^2 - t^2(x-x')^2 = 0
        # <=> (-t^2x^2 + y^2 + z^2) + (0xy + 0xz + 0yz) + (2t^2x'x - 2y'y - 2z'z) + (y'^2 + z'^2 - t^2x'^2)  = 0
        if (surfaceCard.mnemonic == 'K/X' or surfaceCard.mnemonic == 'k/x' or surfaceCard.mnemonic == 'k/X' or surfaceCard.mnemonic == 'K/x'):
            if ((len(surfaceCard.data) != 4) and (len(surfaceCard.data) != 5)):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type K/X has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 4 or 5)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(-surfaceCard.data[3], 1.0, 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(2*surfaceCard.data[3]*surfaceCard.data[0], -2*surfaceCard.data[1], -2*surfaceCard.data[2]),  # G, H, I
                surfaceCard.data[1]*surfaceCard.data[1] + surfaceCard.data[2]*surfaceCard.data[2] - surfaceCard.data[0]*surfaceCard.data[0]*surfaceCard.data[3], 
                texture, material, inverseWrite)
            
        # K/Y -  infinite parallel to y-axis  (3000 K/Y X Y Z t^2)  
        # (x-x')^2 + (z-z')^2 - t^2(y-y')^2 = 0
        # <=> (x^2 - t^2y^2 + z^2) + (0xy + 0xz + 0yz) + (-2x'x + 2t^2y'y - 2z'z) + (x'^2 + z'^2 - t^2y'^2)  = 0
        elif (surfaceCard.mnemonic == 'K/Y' or surfaceCard.mnemonic == 'k/y' or surfaceCard.mnemonic == 'k/Y' or surfaceCard.mnemonic == 'K/y'):
            if ((len(surfaceCard.data) != 4) and (len(surfaceCard.data) != 5)):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type K/Y has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 4 or 5)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, -surfaceCard.data[3], 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(-2*surfaceCard.data[0], 2*surfaceCard.data[3]*surfaceCard.data[1], -2*surfaceCard.data[2]),  # G, H, I
                surfaceCard.data[0]*surfaceCard.data[0] + surfaceCard.data[2]*surfaceCard.data[2] - surfaceCard.data[1]*surfaceCard.data[1]*surfaceCard.data[3], 
                texture, material, inverseWrite)
            
        # K/Z -  infinite parallel to z-axis  (3000 K/Z X Y Z t^2)                  
        # (x-x')^2 + (y-y')^2 - t^2(z-z')^2 = 0
        # <=> (x^2 - y^2 - t^2z^2) + (0xy + 0xz + 0yz) + (-2x'x - 2y'y + 2t^2z'z) + (x'^2 + y'^2 - t^2z'^2)  = 0
        elif (surfaceCard.mnemonic == 'K/Z' or surfaceCard.mnemonic == 'k/z' or surfaceCard.mnemonic == 'k/Z' or surfaceCard.mnemonic == 'K/z'):
            if ((len(surfaceCard.data) != 4) and (len(surfaceCard.data) != 5)):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type K/Z has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 4 or 5)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, 1.0, -surfaceCard.data[3]),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(-2*surfaceCard.data[0], -2*surfaceCard.data[1], 2*surfaceCard.data[3]*surfaceCard.data[2]),  # G, H, I
                surfaceCard.data[0]*surfaceCard.data[0] + surfaceCard.data[1]*surfaceCard.data[1] - surfaceCard.data[2]*surfaceCard.data[2]*surfaceCard.data[3], 
                texture, material, inverseWrite)
            
        # KX -  infinite on x-axis  (3000 KX X t^2)                 
        # y^2 + z^2 - t^2(x-x')^2 = 0
        # <=> (-t^2x^2 + y^2 + z^2) + (0xy + 0xz + 0yz) + (2t^2x'x + 0y + 0z) - t^2x'^2  = 0
        elif (surfaceCard.mnemonic == 'KX' or surfaceCard.mnemonic == 'kx' or surfaceCard.mnemonic == 'kX' or surfaceCard.mnemonic == 'Kx'):
            if ((len(surfaceCard.data) != 2) and (len(surfaceCard.data) != 3)):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type KX has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 2 or 3)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(-surfaceCard.data[1], 1.0, 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(2*surfaceCard.data[1]*surfaceCard.data[0], 0.0, 0.0),  # G, H, I
                -surfaceCard.data[0]*surfaceCard.data[0]*surfaceCard.data[1], 
                texture, material, inverseWrite)
            
        # KY -  infinite on y-axis  (3000 K/Y Y t^2)                    
        # x^2 + z^2 - t^2(y-y')^2 = 0
        # <=> (x^2 - t^2y^2 + z^2) + (0xy + 0xz + 0yz) + (0x + 2t^2y'y + 0z) - t^2y'^2  = 0
        elif (surfaceCard.mnemonic == 'KY' or surfaceCard.mnemonic == 'ky' or surfaceCard.mnemonic == 'kY' or surfaceCard.mnemonic == 'Ky'):
            if ((len(surfaceCard.data) != 2) and (len(surfaceCard.data) != 3)):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type KY has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 3 or 3)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, -surfaceCard.data[1], 1.0),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(0.0, 2*surfaceCard.data[1]*surfaceCard.data[0], 0.0),  # G, H, I
                -surfaceCard.data[0]*surfaceCard.data[0]*surfaceCard.data[1], 
                texture, material, inverseWrite)
            
        # KZ -  infinite on z-axis  (3000 K/Z Z t^2)                        
        # x^2 + y^2 - t^2(z-z')^2 = 0
        # <=> (x^2 - y^2 - t^2z^2) +  + (0x + 0y + 2t^2z'z) - t^2z'^2  = 0
        elif (surfaceCard.mnemonic == 'KZ' or surfaceCard.mnemonic == 'kz' or surfaceCard.mnemonic == 'kZ' or surfaceCard.mnemonic == 'Kz'):
            if ((len(surfaceCard.data) != 2) and (len(surfaceCard.data) != 3)):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type KZ has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 2 or 3)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(1.0, 1.0, -surfaceCard.data[1]),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(0.0, 0.0, 2*surfaceCard.data[1]*surfaceCard.data[0]),  # G, H, I
                -surfaceCard.data[0]*surfaceCard.data[0]*surfaceCard.data[1], 
                texture, material, inverseWrite)
            
        
        # ELLIPSOID, HYPERBOLOID, PARABOLOID
        # http://www.povray.org/documentation/view/3.6.0/299/ (pov ray Quadric)
        # SQ - (3000 SQ A B C D E F G x' y' z') 
        # A(x-x')^2 + B(y-y')^2 + C(z-z')^2 +2D(x-x') + 2E(y-y') + 2F(z-z') + G = 0
        # <=> [Ax^2 + By^2 + Cz^2] + [0xy + 0xz + 0yz] + [ (-2Ax'+2D)x + (-2By'+2E)y + (-2Cz' + 2F)z ] + [Ax'^2 + By'^2 + Cz'^2 - 2Dx' - 2Ey' - 2Fz' + G]   = 0
        if (surfaceCard.mnemonic == 'SQ' or surfaceCard.mnemonic == 'sq' or surfaceCard.mnemonic == 'sQ' or surfaceCard.mnemonic == 'Sq'):
            if (len(surfaceCard.data) != 10):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type SQ has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 10)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]),  # A, B, C
                povray.Vector(0.0, 0.0, 0.0),  # D, E, F
                povray.Vector(-2*surfaceCard.data[0]*surfaceCard.data[7] + 2*surfaceCard.data[3], -2*surfaceCard.data[1]*surfaceCard.data[8]+2*surfaceCard.data[4], -2*surfaceCard.data[2]*surfaceCard.data[9]+2*surfaceCard.data[5]),  # G, H, I
                surfaceCard.data[7]*surfaceCard.data[7]*surfaceCard.data[0] + surfaceCard.data[8]*surfaceCard.data[8]*surfaceCard.data[1] + surfaceCard.data[9]*surfaceCard.data[9]*surfaceCard.data[2] - 2*surfaceCard.data[3]*surfaceCard.data[7] - 2*surfaceCard.data[4]*surfaceCard.data[8] - 2*surfaceCard.data[5]*surfaceCard.data[9] + surfaceCard.data[6], 
                texture, material, inverseWrite)
            

        # CYLINDER, CONE, ELLIPSOID, HYPERBOLOID, PARABOLOID
        # http://www.povray.org/documentation/view/3.6.0/299/ (pov ray Quadric)
        # GQ - (3000 GQ A B C D E F G H I J K)
        # Ax^2 + By^2 + Cz^2 + Dxy + Fxz + Eyz + Gx + Hy + Jz + K = 0
        # <=> [Ax^2 + By^2 + Cz^2] + [0xy + 0xz + 0yz] + [ (-2Ax'+2D)x + (-2By'+2E)y + (-2Cz' + 2F)z ] + [Ax'^2 + By'^2 + Cz'^2 - 2Dx' - 2Ey' - 2Fz' + G]   = 0
        if (surfaceCard.mnemonic == 'GQ' or surfaceCard.mnemonic == 'gq' or surfaceCard.mnemonic == 'gQ' or surfaceCard.mnemonic == 'Gq'):
            if (len(surfaceCard.data) != 10):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type GQ has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 10)"))
                return
            povrayObject = povray.Quadric(
                povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]),  # A, B, C
                povray.Vector(surfaceCard.data[3], surfaceCard.data[5], surfaceCard.data[4]),  # D, E, F
                povray.Vector(surfaceCard.data[6], surfaceCard.data[7], surfaceCard.data[8]),  # G, H, I
                surfaceCard.data[9], 
                texture, material, inverseWrite)
            


        # SURFACES DEFINED BY MACROBODIES
        # BOX (Box) # http://www.povray.org/documentation/view/3.6.1/276/
        if (surfaceCard.mnemonic == 'BOX' or surfaceCard.mnemonic == 'box'):
            if (len(surfaceCard.data) != 12):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type BOX has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 12)"))
                return
            targetX = surfaceCard.data[0] + surfaceCard.data[3] + surfaceCard.data[6] + surfaceCard.data[9];
            targetY = surfaceCard.data[1] + surfaceCard.data[4] + surfaceCard.data[7] + surfaceCard.data[10];
            targetZ = surfaceCard.data[2] + surfaceCard.data[5] + surfaceCard.data[8] + surfaceCard.data[11];
            povrayObject = povray.Box(povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]), 
                povray.Vector(targetX, targetY, targetZ)
            , texture, material, inverseWrite)
            

        # RPP (rectangular parallelepiped)  # http://www.povray.org/documentation/view/3.6.1/276/
        elif (surfaceCard.mnemonic == 'RPP' or surfaceCard.mnemonic == 'rpp'):
            if (len(surfaceCard.data) != 6):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type RPP has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 6)"))
                return
            povrayObject = povray.Box(povray.Vector(surfaceCard.data[0], surfaceCard.data[2], surfaceCard.data[4]), povray.Vector(surfaceCard.data[1], surfaceCard.data[3], surfaceCard.data[5])
            , texture, material, inverseWrite)
        

        # SPH (sphere)  # http://www.povray.org/documentation/view/3.6.1/283/
        elif (surfaceCard.mnemonic == 'SPH' or surfaceCard.mnemonic == 'sph'):
            if (len(surfaceCard.data) != 4):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type SPH has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 4)"))
                return
            povrayObject = povray.Sphere(povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]), surfaceCard.data[3]
                , texture, material, inverseWrite)
            

        # RCC (Right Circular Cylinder) # http://www.povray.org/documentation/view/3.6.1/278/
        elif (surfaceCard.mnemonic == 'RCC' or surfaceCard.mnemonic == 'rcc'): 
            if (len(surfaceCard.data) != 7):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type RCC has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 7)"))
                return
            povrayObject = povray.Cylinder(povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]),
                povray.Vector(surfaceCard.data[3], surfaceCard.data[4], surfaceCard.data[5]),
                surfaceCard.data[6], 
                texture, material, inverseWrite)

        # TRC (Truncated Right Angle Cone)
        elif (surfaceCard.mnemonic == 'TRC' or surfaceCard.mnemonic == 'trc'): 
            if (len(surfaceCard.data) != 8):
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type TRC has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 8)"))
                return
            povrayObject = povray.Cone(
                povray.Vector(surfaceCard.data[0], surfaceCard.data[1], surfaceCard.data[2]),
                surfaceCard.data[6],
                povray.Vector(surfaceCard.data[0]+surfaceCard.data[3], surfaceCard.data[1]+surfaceCard.data[4], surfaceCard.data[2]+surfaceCard.data[5]),
                surfaceCard.data[7], 
                texture, material, inverseWrite)
            
        
        elif (surfaceCard.mnemonic == 'RHP' or surfaceCard.mnemonic == 'rhp' or surfaceCard.mnemonic == 'HEX' or surfaceCard.mnemonic == 'hex'): 
            
            # ATTENTION: POV RAY only can define a prism in 1 plane
            # regular grid
            if (len(surfaceCard.data) == 9):
                base = [float(surfaceCard.data[0]),float(surfaceCard.data[1]), float(surfaceCard.data[2])]
                h = [float(surfaceCard.data[3]),float(surfaceCard.data[4]), float(surfaceCard.data[5])]
                side1 = [float(surfaceCard.data[6]),float(surfaceCard.data[7]), float(surfaceCard.data[8])]
                normal = container.Container.getNormalizedVector(h)
                side1v = container.Container.cross(side1, normal)
                angle = -60.0*math.pi / 180.0
                side2 = container.Container.rotateVectorAroundNormal(side1, normal, angle)
                side2v = container.Container.cross(side2, normal)
                
                # use side -1 and calculated side 3 (+120)
                angle = 120.0*math.pi / 180.0
                side3 = container.Container.rotateVectorAroundNormal(side1, normal, angle)
                side3v = container.Container.cross(side3, normal)
                side1Neg = [-side1[0],-side1[1],-side1[2]]

                coordinates = []
                # calculate intersections of two sides and get 3 coordinates of hexagon
                params =  container.Container.lineParameterOfIntersection(side1, side1v, side2, side2v)
                t1 = params['t1']
                t2 = params['t2']
                # t1 is where side 1 intersects with side 2
                # -t1 is the opposite coordinate
                # -t2 is the opposite coordinate where side 2 intersects side 1
                coordinates.append([side1[0] + -t1*side1v[0], side1[1] + -t1*side1v[1], side1[2] + -t1*side1v[2]])
                coordinates.append([side1[0] + t1*side1v[0] , side1[1] + t1*side1v[1], side1[2] + t1*side1v[2]])
                coordinates.append([side2[0] + -t2*side2v[0], side2[1] + -t2*side2v[1], side2[2] + -t2*side2v[2]])

                params =  container.Container.lineParameterOfIntersection(side1Neg, side1v, side3, side3v)
                t1 = params['t1']
                t2 = params['t2']
                coordinates.append([side1Neg[0] + -t1*side1v[0], side1Neg[1] + -t1*side1v[1], side1Neg[2] + -t1*side1v[2]])
                coordinates.append([side1Neg[0] + t1*side1v[0], side1Neg[1] + t1*side1v[1], side1Neg[2] + t1*side1v[2]])
                coordinates.append([side3[0] + -t2*side3v[0], side3[1] + -t2*side3v[1], side3[2] + -t2*side3v[2] ])
                
                # Enable this to get all the coorindate points
                # bottomCoordinates = copy.copy(coordinates)
                # for cor in bottomCoordinates:
                #   coordinates.append([cor[0]+h[0], cor[1]+h[1], cor[2]+h[2]])
                
                args = {}
                args['translate'] = povray.Vector(base[0],base[1],base[2])
                if ( (h[0] != 0.0) and (h[1] == 0.0) and (h[2] == 0.0)):
                    c1 = 1
                    c2 = 2
                    h_c = 0
                    args['rotate'] = povray.Vector(0.0, 0.0, -90.0)
                    
                elif ( (h[0] == 0.0) and (h[1] != 0.0) and (h[2] == 0.0)):
                    c1 = 0
                    c2 = 2
                    h_c = 1

                elif ( (h[0] == 0.0) and (h[1] == 0.0) and (h[2] != 0.0)):
                    c1 = 0
                    c2 = 1
                    h_c = 2
                    args['rotate'] = povray.Vector(90.0, 0.0, 0.0)
                else:
                    print "ERROR (Parsing RHP): RHP not parallel to an axis"
                    return
                
                outputCoordinates = []
                outputCoordinates.append(",")
                for cor in range(0, len(coordinates)-1):
                    outputCoordinates.append(povray.Vector(coordinates[cor][c1], coordinates[cor][c2]))
                    outputCoordinates.append(",")
                outputCoordinates.append(povray.Vector(coordinates[len(coordinates)-1][c1], coordinates[len(coordinates)-1][c2]))   
                outputCoordinates.append(texture)
                outputCoordinates.append(inverseWrite)
                bb = surfaceCard.getBoundingBox()
                if (bb):
                    povrayObject = povray.Object(povray.Prism(0, h[h_c],6, *outputCoordinates, **args ), povray.BoundingBox(bb.buildPOVRay()))
                else:
                    povrayObject = povray.Prism(0, h[h_c],6, *outputCoordinates , **args )

            elif (len(surfaceCard.data) == 15):
                base = [surfaceCard.data[0],surfaceCard.data[1], surfaceCard.data[2]]
                h = [surfaceCard.data[3],surfaceCard.data[4], surfaceCard.data[5]]
                side1 = [surfaceCard.data[6],surfaceCard.data[7], surfaceCard.data[8]]
                side2 = [surfaceCard.data[9],surfaceCard.data[10], surfaceCard.data[11]]
                side3 = [surfaceCard.data[12],surfaceCard.data[13], surfaceCard.data[14]]
                side1v = container.Container.cross(side1, h)
                side2v = container.Container.cross(side2, h)
                side3v = container.Container.cross(side3, h)
                print "WARNING: Hexagon is not regular and is not been drawn"
            else:
                raise(Exception("ERROR (Build Surface Card " + str(surfaceNumber) + "): Surface " + str(surfaceNumber) + " of type RHP has not enough or too much arguments (" + str(len(surfaceCard.data)) + " instead of 15)"))
                return
        if (povrayObject == 0):
            print "WARNING (Build Surface Card " + str(surfaceNumber) + ": Type of surface \'" + str(surfaceCard.mnemonic) + "\' of surface card " + str(surfaceNumber) + " not defined"
            return 0
        transArgs = surfaceCard.getTransformationArgs()
        if (transArgs.has_key('translate')):
                povrayObject.kwargs['translate'] = transArgs['translate']
        if (transArgs.has_key('matrix')):
                povrayObject.kwargs['matrix'] = transArgs['matrix']
        return povrayObject




#######################################################################################################################
## PARSING DATA CARDS
#######################################################################################################################

    # ==> parseDataCards():
    # interpret every line of the data cards block
    # get data card mnemonic and information
    #------------------------------------------------------------------------------------------------------------------ 
    def parseDataCards(self):
        dh = DataHolder()
        
        # store the previous commentary line in this variable (used for seeking the name of a material
        previousCommentaryLine = None

        # read all data cards out of file and put them in surfaceCards
        for line in self.dataBlockComments:
            dh = DataHolder()

            # see if the current line is a commentary line
            # if so, continue the loop and store it in previous line
            if (dh.set(re.match('^[\s]*[c][\s]+', line ,flags=re.IGNORECASE))):
                previousCommentaryLine = line
                continue

                
            dataCard = re.split('[\s]+', line)
            container.Container.remove_values_from_list(dataCard, '')
            
            dataCardData =  dataCard[1:len(dataCard)]
            self.dataCards[str(dataCard[0])] = DataCard.DataCard(str(dataCard[0]),dataCardData)
            
            # check for transformation datacards
            if (re.match("^tr", str(dataCard[0])) or re.match("^TR", str(dataCard[0]))):
                key = dataCard[0][2:]
                key = key.replace("=", "")
                container.Container.remove_values_from_list(dataCardData, "=")
                self.transformationCards[key] = dataCardData
                self.transformationCardsFlag[key] = False
            if (re.match("^\*tr", str(dataCard[0])) or re.match("^\*TR", str(dataCard[0]))):
                key = dataCard[0][3:]
                key = key.replace("=", "")
                container.Container.remove_values_from_list(dataCardData, "=")
                for i in range(3, len(dataCardData)):
                    dataCardData[i] = math.cos(float(dataCardData[i])*math.pi / 180.0)
                self.transformationCards[key] = dataCardData
                self.transformationCardsFlag[key] = True
                
            # check for material data cards
            if (dh.set(re.match('^[\s]*m(?P<material>[\d]+)[\s]+(?P<data>[\S\s]*)', line ,flags=re.IGNORECASE))):
                material = int(dh.value.groupdict()['material'])
                self.materialCards[material] = dh.value.groupdict()['data']
                # check if there is a command line before the material card and if it defines a materials name for the card
                if (previousCommentaryLine != None):
                    if (dh.set(re.match('^[\s]*[c][\s]+[m]' + str(material) + '[\s]*[\=][\s]*(?P<data>[\S]*)', previousCommentaryLine ,flags=re.IGNORECASE))):
                        self.materialCardsName[material] = dh.value.groupdict()['data']
                    else: 
                        self.materialCardsName[material] = None
                else:
                    self.materialCardsName[material] = None
            previousCommentaryLine = None # current line is not commentary line
            
        


#######################################################################################################################
## EXTRA OPERATORS
#######################################################################################################################

    # ==> getHexOffset(cellCard):
    # Returns the offset of a hexagonal shape specified in a cellcard geometry (offset = [minX, minY, minZ, maxX, maxY, maxZ])
    #------------------------------------------------------------------------------------------------------------------ 
    def getHexOffset(self, cellCard):
        geometry = cellCard.fullGeometry
        if (re.search('\:', geometry)):
            return 0
        else:
            intersection = re.split('[\s]+', geometry)
            container.Container.remove_values_from_list(intersection, "")
            if (len(intersection) > 1 or len(intersection) == 0):
                return 0
            surface = intersection[0]
            if (surface[0] == '-'):
                surface = surface[1:]
            elif (surface[0] == '#'):
                return 0
            else:
                surface = surface
            return self.surfaceCards[int(surface)].getHexOffset()
        return 0
    
    # ==> getRectangularOffset(cellCard):
    # Returns the offset of a rectangular shape specified in a cellcard geometry (offset = [minX, minY, minZ, maxX, maxY, maxZ])
    #------------------------------------------------------------------------------------------------------------------ 
    def getRectangularOffset(self, cellCard):
        print "simontest" + str(cellCard)
        geometry = cellCard.fullGeometry
        if (re.search('\:', geometry)):
            return 0
        else:
            intersection = re.split('[\s]+', geometry)
            container.Container.remove_values_from_list(intersection, "")
            offset = ['inf', 'inf', 'inf', 'inf', 'inf', 'inf']
            
            if (len(intersection) == 0):
                return 0
            for surface in intersection:
                isMin = False
                if (surface[0] == '-'):
                    isMin = True
                    surface = surface[1:]
                elif (surface[0] == '#'):
                    return 0
                else:
                    surface = surface
                boundary = self.surfaceCards[int(surface)].getRectangularOffset(isMin)
                print "boundary"
                print boundary
                for i in range(0,3):
                    if (offset[i] == 'inf'):
                        offset[i] = boundary[i]
                    else:
                        if (not boundary[i] == 'inf'):
                            offset[i] = max(offset[i], boundary[i])
                for i in range(3,6):
                    if (offset[i] == 'inf'):
                        offset[i] = boundary[i]
                    else:
                        if (not boundary[i] == 'inf'):
                            offset[i] = min(offset[i], boundary[i])
            return offset
        return 0
        
#######################################################################################################################
## OUTSIDE WORLD FUNCTIONS
#######################################################################################################################
        
    # ==> getImpZeroCellCard():
    # Returns the cellcard where imp=0  TODO: return more cellcards with imp:0
    #------------------------------------------------------------------------------------------------------------------ 
    def getImpZeroCellCard(self):
        found = []
        for card in self.cellCards:
            #print self.cellCards[card].params
            if (self.cellCards[card].params.has_key("IMP")):
                dh = DataHolder()
                if (dh.set(re.match('[\w,\s]*n[\w,\s]*=[\s]*0', self.cellCards[card].params["IMP"] ,flags=re.IGNORECASE))):
                    found.append(self.cellCards[card])
        if (len(found) > 1):
            print "WARNING: Too much cellcards with imp:n=0"
            return found
        elif (len(found) == 0):
            print "WARNING: Celcard with imp:n=0 not found"
            return 0
        else:
            return found
        
    # ==> writeOuterCaseToFile(cellCard, file):
    # Write the geometry of the cellcard with imp=0 to a file
    # This geometry is a simple rectangular or cylindrical geometry so it can be used and processed in the GUI (OpenGL)
    #------------------------------------------------------------------------------------------------------------------ 
    def writeOuterCaseToFile(self, cellCard, file): 
        geometry = cellCard.fullGeometry
        
        isComplement = False
        if (re.search('[a-z,A-Z]+', geometry)):
            if (geometry[0] == "#"):
                geometry = geometry[1:]
                isComplement = True
            geometry = geometry.strip() # remove spaces in front and in the back
            container.Container.remove_values_from_list(geometry, " ")
            geometry = cellCard.charToGeometry[geometry]
            geometry =  geometry[1:-1]

        isCylinder = False
            
        if (re.search('\:', geometry) or isComplement or re.match('^[\s]*[\d]+[\s]*$', geometry, flags=re.IGNORECASE)):
            if (isComplement):
                union = re.split('[\s]+', geometry)
            else:   
                union = re.split('[:]+', geometry)
            offset = ['inf', 'inf', 'inf', 'inf', 'inf', 'inf']
            print union
            print "jaja1"
            for surface in union:
                surface = surface.replace(" ", "");
                if (isComplement):
                        isMin = True
                else:
                    isMin = False
                if (surface[0] == '-'):
                    if (isComplement):
                        isMin = False
                    else:
                        isMin = True
                    surface = surface[1:]
                elif (surface[0] == '#'):
                    return 0
                else:
                    surface = surface
                
                if (self.surfaceCards[int(surface)].isCylinder()):
                    isCylinder = True
                    radius = self.surfaceCards[int(surface)].getCylinderRadius()
                    cylinderDirection = self.surfaceCards[int(surface)].getCylinderDirection()
                    
                    #direction = self.surfaceCards[int(surface)].getCylinderDirection()
                else:
                    boundary = self.surfaceCards[int(surface)].getRectangularOffset(isMin)
                    if (boundary):
                        for i in range(0,3):
                            if (offset[i] == 'inf'):
                                offset[i] = boundary[i]
                            else:
                                if (not boundary[i] == 'inf'):
                                    offset[i] = max(offset[i], boundary[i])
                        for i in range(3,6):
                            if (offset[i] == 'inf'):
                                offset[i] = boundary[i]
                            else:
                                if (not boundary[i] == 'inf'):
                                    offset[i] = min(offset[i], boundary[i])
            if (isCylinder):

                for i in range(0,6):
                    if (offset[i] == 'inf'):
                        offset[i] = 0.0
                file.writeln("CYLINDER" + "&" + cylinderDirection + "&"
                        + str(offset[3]) + "&" + str(offset[4]) + "&" + str(offset[5]) 
                            + "&" + str(offset[0]) + "&" + str(offset[1]) + "&" + str(offset[2]) + "&" +  str(radius))
            else:
                if (len(union) == 1):
                    self.surfaceCards[int(union[0])].writeSurfaceToFile(file)
                else:
                    for i in range(0,6):
                        if (offset[i] == 'inf'):
                            offset[i] = 0.0
                    file.writeln("BOX" + "&"
                            + str(offset[3]) + "&" + str(offset[4]) + "&" + str(offset[5]) 
                                + "&" + str(offset[0]) + "&" + str(offset[1]) + "&" + str(offset[2]))
        else: # TODO: process unions
            intersection = re.split('[\s]+', geometry)
            container.Container.remove_values_from_list(intersection, "")
                
            if (len(intersection) == 1):
                self.surfaceCards[int(intersection[0])].writeSurfaceToFile(file)
            if (len(intersection) == 0):
                return 0
        
        
    
