#######################################################################################################################
## BoundingBox.py
#######################################################################################################################
##
## Representation of a bounding box
## Always a rectangular representation based on min and max values for the three main axis
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################

import povray


class BoundingBox:

    def __init__(self, minX=0, minY=0, minZ=0, maxX=0, maxY=0, maxZ=0, isSphere = False):
        self.minX = minX
        self.minY = minY
        self.minZ = minZ
        self.maxX = maxX
        self.maxY = maxY
        self.maxZ = maxZ
        self.isSphere = isSphere    # in the future also possible to have a spherical bounding box
        
        self.exists = 0             # bool to see if the bounding box exists (good for compound bounded boxes)

    def __repr__(self):
        return "BoundingBox()"
    def __str__(self):
        return "BoundingBox(" + str(self.minX) + "," + str(self.minY) + "," + str(self.minZ) + " - " + str(self.maxX) + "," + str(self.maxY) + "," + str(self.maxZ) + ")"
    
    # ==> clone(bb)
    # Clone the instance information of bb in the current instance
    #------------------------------------------------------------------------------------------------------------------
    def clone(self, bb):
        self.minX = bb.minX
        self.minY = bb.minY
        self.minZ = bb.minZ
        self.maxX = bb.maxX
        self.maxY = bb.maxY
        self.maxZ = bb.maxZ
        self.isSphere = bb.isSphere
        self.exists = bb.exists
    
    # ==> append(bb)
    # Append the current bounding box with the information out of the bb instance
    #------------------------------------------------------------------------------------------------------------------
    def append(self, bb):
        self.minX = min(self.minX,bb.minX)
        self.minY = min(self.minY,bb.minY)
        self.minZ = min(self.minZ,bb.minZ)
        self.maxX = max(self.maxX,bb.maxX)
        self.maxY = max(self.maxY,bb.maxY)
        self.maxZ = max(self.maxZ,bb.maxZ)
        if (self.isSphere):
            self.isSphere = False
    
    # ==> buildPOVRay()
    # Build the instance to a pov ray object
    #------------------------------------------------------------------------------------------------------------------
    def buildPOVRay(self):
        if (not self.isSphere):
            return povray.Box(povray.Vector(self.minX,self.minY,self.minZ), povray.Vector(self.maxX,self.maxY,self.maxZ))
        if (self.isSphere):
            return povray.Sphere(povray.Vector( self.minX + (self.maxX - self.minX)/2.0, self.minY + (self.maxY - self.minY)/2.0, self.minZ + (self.maxZ - self.minZ)/2.0), (self.maxZ - self.minZ)/2.0) 
        return 0
                