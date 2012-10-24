#######################################################################################################################
## Color.py
#######################################################################################################################
##
## Basic class for color representation
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################


class Color:

    def __init__(self, red = 1.0, green = 1.0, blue = 1.0, alpha = 1.0):
        self.red = red
        self.green = green
        self.blue = blue
        self.alpha = alpha

    def __repr__(self):
        return "Color()"
    def __str__(self):
        return "BoundingBox(" + str(self.red) + "," + str(self.green) + "," + str(self.blue) + " - " + str(self.alpha) + ")"
    
    # ==> toPovRay()
    # Build the color to a povray object
    #------------------------------------------------------------------------------------------------------------------
    def toPovRay(self):
        return "rgb<" + str(self.red) + "," + str(self.green) + "," + str(self.blue) + ">"