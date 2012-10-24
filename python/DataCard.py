#######################################################################################################################
## DataCard.py
#######################################################################################################################
##
## Combine all the information a mcnpx data card describes
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################


class DataCard:

	def __init__(self, mnemonic, data):
		self.mnemonic = mnemonic
		self.data = data

	def __repr__(self):
		return "DataCard()"
	def __str__(self):
		return "Datacard of type " + self.mnemonic;
	
	# ==> getDataLine()
	# Returns the brief description of the surface
	#------------------------------------------------------------------------------------------------------------------
	def getDataLine(self):
		fullData = str(self.mnemonic)
		for i in self.data:
			fullData = fullData + " " + str(i)
		return fullData