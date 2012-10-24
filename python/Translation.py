#######################################################################################################################
## Translation.py
#######################################################################################################################
##
## Class to represent a translation
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################


class Translation:

	def __init__(self, o1=0.0, o2=0.0, o3=0.0):
		self.o1 = o1
		self.o2 = o2
		self.o3 = o3

	def __repr__(self):
		return "Translation()"
	def __str__(self):
		return "Translation (" + str(self.o1) + "," + str(self.o2) + "," + str(self.o3) + ")"
	