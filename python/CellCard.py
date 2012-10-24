#######################################################################################################################
## CellCard.py
#######################################################################################################################
##
## Combine all the information a mcnpx cell card describes
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################

import re
import math

import povray
import Translation
import Rotation
import container
from DataHolder import DataHolder


class CellCard:

	def __init__(self, number, material=0, geometry=0, paramsData=0, d=0, like=0):
		self.number = number			# cell card number
		self.material = material		# cell card material
		self.geometry = geometry		# cell card geometry
		self.d = d						# cell card density (not always specified)
		self.paramsData = paramsData	# parameters of the cell card (full string)
		self.params = {}				# parsed parameters of the cell	
		self.like = like				# LIKE BUT parameters

		self.subsurfaceMap = {}			# contains a mapping of a subsurface of the form '(...)' to an identifier
		self.subsurfaceMapInv = {}		# contains a mapping of an identifier of a subsurface to an POV ray item
		self.charToGeometry = {}
		self.fullGeometry = ""			# the full geometry described in terms of subsurfaces and without brackets

		# PARAMETERS
		self.translation = 0
		self.rotation = 0

		self.universeTranslation = 0
		self.universeRotation = 0
		self.fillUniverse = 0			# universe that must fill the current cell

		self.povRayArgs = {} 			# extra arguments for the cell (i.e. transformations)

		# LAT parameters
		self.hasLAT = False
		self.typeLAT = 0
		self.minI = 0
		self.maxI = 0
		self.minJ = 0
		self.maxJ = 0
		self.minK = 0
		self.maxK = 0
		self.latUniverses = []

		self.latticeWidth = [6.0, 6.0, 6.0]
		

	def __repr__(self):
		return "CellCard()"
	def __str__(self):
		geometry = ""
		for geo in self.geometry:
			geometry = geometry + " " + geo
		return "CellCard " + str(self.number) + " with material " + str(self.material) + " and geometry" + geometry


	# ==> printGeometry()
	# Print out the entire geometry of the cell
	#------------------------------------------------------------------------------------------------------------------
	def printGeometry(self):
		for geo in self.geometry:
			print geo,

	# ==> getPovRayArgs()
	# Returns the arguments of the cellcard in a dictionary that can be used by pov ray
	# For example a translation or rotation declared by a TRCL or *TRCL
	#------------------------------------------------------------------------------------------------------------------
	def getPovRayArgs(self):
		self.povRayArgs = {}
		if (self.translation):
			self.povRayArgs['translate'] = povray.Vector(self.translation.o1, self.translation.o2, self.translation.o3)

		# http://www.povray.org/documentation/view/3.6.1/49/
		# http://www.f-lohmueller.de/pov_tut/trans/trans_400e.htm
		if (self.rotation):
			self.povRayArgs['matrix'] = povray.Vector(self.rotation.xx, self.rotation.xy, self.rotation.xz,
									self.rotation.yx, self.rotation.yy, self.rotation.yz,
									self.rotation.zx, self.rotation.zy, self.rotation.zz,
									0.0, 0.0, 0.0)

		return self.povRayArgs


	# ==> getUniversePovRayArgs()
	# Returns the arguments of the cellcard in a dictionary that can be used by pov ray
	# Specific for the filling of a sub universe
	# For example a translation or rotation declared by a FILL or *FILL
	#------------------------------------------------------------------------------------------------------------------
	def getUniversePovRayArgs(self):
		universePovRayArgs = {'//UNIVERSE' : self.fillUniverse}
		if (self.universeTranslation):
			universePovRayArgs['translate'] = povray.Vector(self.universeTranslation.o1, self.universeTranslation.o2, self.universeTranslation.o3)

		if (len(universePovRayArgs) > 1):
			return universePovRayArgs
		else:
			return {}

	
	# ==> interpretParameters()
	# Parse the parameters of the cell out of the parameter string and put them in a dictionary self.params
	#------------------------------------------------------------------------------------------------------------------
	def interpretParameters(self):
		params =  self.params

		# CHECK IF THE CELL CONTAINS A LATTICE
		self.hasLAT = False
		if (params.has_key('LAT')):
			# a cell containing a LAT also needs to have a FILL parameter
			if (params.has_key('FILL') or params.has_key('*FILL')):
				self.hasLAT = True
				self.typeLAT = int(params['LAT'])

				if ((self.typeLAT == 1) or (self.typeLAT == 2)):
					# the cell describes a rectangular (square) lattice comprised of hexehedra
					ranges = re.match('[ ]*[-]?[\d]+[:]{1}[\d]+[ ]+[-]?[\d]+[:]{1}[\d]+[ ]+[-]?[\d]+[:]{1}[\d]+[ ]+', params['FILL'][:100])
					if (ranges):
						# parse the fully specified boundaries of the lattice
						rangesItems = re.findall('[-]?[\d]+', ranges.group(0))
						if (len(rangesItems) == 6):
							self.minI = int(rangesItems[0])
							self.maxI = int(rangesItems[1])
							self.minJ = int(rangesItems[2])
							self.maxJ = int(rangesItems[3])
							self.minK = int(rangesItems[4])
							self.maxK = int(rangesItems[5])
						else:
							raise(Exception("ERROR (Parse Cell " + str(self.number) + "): Problem reading fill boundary parameters, too little args in cell " + str(self.number)))

						# parse the universes of the lattice
						latUniverses = re.split('[\s]+', params['FILL'][len(ranges.group(0)):])
						container.Container.remove_values_from_list(latUniverses, '')
						# check for parameters in lattice universes
						# for example 200 10R (repeat 200 ten times)
						latUniversesNew = []
						for i in range(0, len(latUniverses)):
							if(latUniverses[i][len(latUniverses[i])-1] == 'R' or latUniverses[i][len(latUniverses[i])-1] == 'r'):#re.match('R$',latUniverses[i], flags=re.IGNORECASE)):
								previousElement = latUniversesNew[len(latUniversesNew)-1]
								elements = int(latUniverses[i][:len(latUniverses[i])-1])
								for x in range(0, elements):
									latUniversesNew.append(previousElement)
							else:
								latUniversesNew.append(latUniverses[i])
						self.latUniverses = latUniversesNew
					else:
						#print "ERROR (Cell " + str(self.number) + "): no fully specified fill found in cell " + str(self.number) + " [CellCard::interpretParameters]"
						raise(Exception("ERROR (Parse Cell " + str(self.number) + "): No fully specified fill found in cell " + str(self.number)))
				else:
					raise(Exception("ERROR (Parse Cell " + str(self.number) + "): Cell " + str(self.number) + " contains a LAT with unknown type " + str(self.typeLAT)))
			else:
				raise(Exception("ERROR (Parse Cell " + str(self.number) + "): Cell " + str(self.number) + " contains LAT, but no FILL"))
				print 
				return 0
		elif (params.has_key('FILL') or params.has_key('*FILL')):
			if (not self.hasLAT):
				if params.has_key('FILL'):
					fill = params['FILL']
				else:
					fill = params['*FILL']
					
				# get all the items between the brackets (these are the transform parameters)
				transformParams = re.findall('(?<=\()[\d,\D,\s]+(?=\))', fill)
				self.fillUniverse = int(re.split('[\(,\[]+', fill)[0])
				
				# if there is a transformation, parse the transformation data and put it in a Transformation object
				if ( len(transformParams) >= 1):
					# split the transform parameters and remove empty items
					transformParamsItems = re.split('[\s]+', transformParams[0])
					container.Container.remove_values_from_list(transformParamsItems, '')

					# check if there is a translation defined on the fill parameters
					if (len(transformParamsItems) >= 3):
						self.universeTranslation = Translation.Translation(transformParamsItems[0], transformParamsItems[1], transformParamsItems[2])

					# check if there is a rotation defined on the fill parameters	
					if (len(transformParamsItems) >= 12):
						if (params.has_key('*FILL')):
							for i in range(3, len(transformParamsItems)):
								transformParamsItems[i] = math.cos(float(transformParamsItems[i])*math.pi / 180.0)
						self.universeRotation = Rotation.Rotation(transformParamsItems[3], transformParamsItems[4], transformParamsItems[5],
											transformParamsItems[6], transformParamsItems[7], transformParamsItems[8],
											transformParamsItems[9], transformParamsItems[10], transformParamsItems[11])
					
		if (params.has_key('TRCL')):
			trcl = params['TRCL']
			transformParams = re.findall('(?<=\()[\d,\D,\s]+(?=\))', trcl)
			if (len(transformParams) >= 1):
				transformParamsItems = re.split('[\s]+', transformParams[0])

				# check if there is a translation defined on the fill parameters
				if (len(transformParamsItems) >= 3):
					self.translation = Translation.Translation(transformParamsItems[0], transformParamsItems[1], transformParamsItems[2])

				# check if there is a rotation defined on the fill parameters
				if (len(transformParamsItems) >= 12):
					self.rotation = Rotation.Rotation(transformParamsItems[3], transformParamsItems[4], transformParamsItems[5],
						transformParamsItems[6], transformParamsItems[7], transformParamsItems[8],
						transformParamsItems[9], transformParamsItems[10], transformParamsItems[11])
					
		elif (params.has_key('*TRCL')):
			trcl = params['*TRCL']
			transformParams = re.findall('(?<=\()[\d,\D,\s]+(?=\))', trcl)
			if (len(transformParams) >= 1):
				transformParamsItems = re.split('[\s]+', transformParams[0])
				
				# check if there is a translation defined on the fill parameters
				if (len(transformParamsItems) >= 3):
					self.translation = Translation.Translation(transformParamsItems[0], transformParamsItems[1], transformParamsItems[2])

				# check if there is a rotation defined on the fill parameters
				if (len(transformParamsItems) >= 12):
					for i in range(3, len(transformParamsItems)):
						transformParamsItems[i] = math.cos(float(transformParamsItems[i])*math.pi / 180.0)
					self.rotation = Rotation.Rotation(transformParamsItems[3], transformParamsItems[4], transformParamsItems[5],
						transformParamsItems[6], transformParamsItems[7], transformParamsItems[8],
						transformParamsItems[9], transformParamsItems[10], transformParamsItems[11])
		return 1

	# ==> parseParameters()
	# Parse all the parameters that are contained in the cell (from string to dictionary)
	#------------------------------------------------------------------------------------------------------------------
	def parseParameters(self):
		currentParam = ' '
		foundParamWithoutEqualSign = False
		
		dh = DataHolder()
		for i in range(0,len(self.paramsData)):
			if (dh.set(re.match('^[\s]*FILL[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'FILL'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*\*FILL[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = '*FILL'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*AREA[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'AREA'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*VOL[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'VOL'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*U[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'U'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*TRCL[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'TRCL'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*\*TRCL[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = '*TRCL'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])	
			elif (dh.set(re.match('^[\s]*LAT[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'LAT'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*TR[\s]*[=]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'TR'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*IMP[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'IMP'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*MAT[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'MAT'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*RHO[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'RHO'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*PWT[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'PWT'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*EXT[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'EXT'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*FCL[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'FCL'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*WWN[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'WWN'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*DXC[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'DXC'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*NONU[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'NONU'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*PD[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'PD'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			elif (dh.set(re.match('^[\s]*TMP[\s]*[:]?(?P<data>[\S]*)', self.paramsData[i] ,flags=re.IGNORECASE))):
				currentParam = 'TMP'
				self.params[currentParam] = [];
				self.params[currentParam].append(dh.value.groupdict()['data'])
			else:
				if (currentParam == ' '):
					pass
				elif (len(self.paramsData[i]) > 0 and (self.paramsData[i][0] == ':' or self.paramsData[i][0] == '=') and foundParamWithoutEqualSign):
					# found a '=' of the parameter just found, test if the previous entry was the parameters, instead the '=' is a part of the parameter list
					self.params[currentParam].append(self.paramsData[i][1:])
				else:
					self.params[currentParam].append(self.paramsData[i])
				foundParamWithoutEqualSign = False
		
		# combine all the parameter data per parameter 
		for param in self.params:
			if len(self.params[param]) == 0:
				continue
			container.Container.remove_values_from_list(self.params[param], "")
			paramString = ' '.join(self.params[param])
			self.params[param] = paramString
		
		self.interpretParameters()