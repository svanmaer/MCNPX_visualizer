#######################################################################################################################
## SurfaceCard.py
#######################################################################################################################
##
## Combine all the information a mcnpx surface card describes
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################

import povray
import math
import container
import BoundingBox 
import Rotation

class SurfaceCard:

	def __init__(self, number, mnemonic, data, n=0):
		self.number = number		# surface card number
		self.n = n					# optional surface card n (see mcnpx manual)
		self.mnemonic = mnemonic	# surface card type
		self.data = data			# surface card data
		
		self.bb = 0					# bounding box of the surface card
		
		self.translation = 0		# optional translation on the surface card
		self.rotation = 0			# optional rotation on the surface card
		
	def __repr__(self):
		return "SurfaceCard()"
	def __str__(self):
		return "Surface " + str(self.number) + " of type " + self.mnemonic;
		
	# ==> getSurfaceLine()
	# Returns the brief description of the surface
	#------------------------------------------------------------------------------------------------------------------
	def getSurfaceLine(self):
		fullSurface = str(self.number) + " " + str(self.n) + " " +  self.mnemonic
		for i in self.data:
			fullSurface = fullSurface + " " + str(i)
		return fullSurface
		
	# ==> getTransformationArgs()
	# Returns the optional transformation arguments of the surface in a dictionary so it can be used by pov ray
	#------------------------------------------------------------------------------------------------------------------
	def getTransformationArgs(self):
		transArgs = {}
		
		# http://www.povray.org/documentation/view/3.6.1/49/
		# http://www.f-lohmueller.de/pov_tut/trans/trans_400e.htm
		if (self.rotation):
			rot = Rotation.Rotation(self.rotation[0], self.rotation[1], self.rotation[2],
									self.rotation[3], self.rotation[4], self.rotation[5],
									self.rotation[6], self.rotation[7], self.rotation[8])
			
			if (self.translation):
				transArgs['matrix'] = povray.Vector(rot.xx, rot.xy, rot.xz,
										rot.yx, rot.yy, rot.yz,
										rot.zx, rot.zy, rot.zz,
										float(self.translation[0]), float(self.translation[1]), float(self.translation[2]))
			else:
				transArgs['matrix'] = povray.Vector(rot.xx, rot.xy, rot.xz,
									rot.yx, rot.yy, rot.yz,
									rot.zx, rot.zy, rot.zz,
									0.0, 0.0, 0.0)
		else:
			if (self.translation):
				transArgs['translate'] = povray.Vector(float(self.translation[0]), float(self.translation[1]), float(self.translation[2]))

		return transArgs
	
	
	# ==> isCylinder()
	# If the mnemonic type is cylindrical, return True
	#------------------------------------------------------------------------------------------------------------------
	def isCylinder(self):
		if (self.mnemonic == 'CX' or self.mnemonic == 'cx'
			or self.mnemonic == 'CY' or self.mnemonic == 'cy'
			or self.mnemonic == 'CZ' or self.mnemonic == 'cz'):
			return True
		else:
			return False
		
	# ==> getCylinderRadius()
	# If the mnemonic type is cylindrical, returns the radius of the cylinder
	#------------------------------------------------------------------------------------------------------------------
	def getCylinderRadius(self):
		if (self.mnemonic == 'CX' or self.mnemonic == 'cx'
			or self.mnemonic == 'CY' or self.mnemonic == 'cy'
			or self.mnemonic == 'CZ' or self.mnemonic == 'cz'):
			return self.data[0]
		else:
			return 0
		
	# ==> getCylinderDirection()
	# If the mnemonic type is cylindrical, returns the axis of the main direction (X, Y or Z)
	#------------------------------------------------------------------------------------------------------------------
	def getCylinderDirection(self):
		if (self.getCylinderDirection == 'CX' or self.mnemonic == 'cx'):
			return "X"
		elif self.mnemonic == 'CY' or self.mnemonic == 'cy':
			return "Y"
		elif self.mnemonic == 'CZ' or self.mnemonic == 'cz':
			return "Z"
		else:
			return 0
		
	# ==> getCylinderRadius()
	# If the mnemonic type is cylindrical, returns the radius of the cylinder
	#------------------------------------------------------------------------------------------------------------------
	def getCylinderBoundaries(self):
		if (self.mnemonic == 'CX' or self.mnemonic == 'cx'
			or self.mnemonic == 'CY' or self.mnemonic == 'cy'
			or self.mnemonic == 'CZ' or self.mnemonic == 'cz'):
			return self.data[0]
		else:
			return 0
		
			
	# ==> writeSurfaceToFile(file)
	# Used as a coupling for the GUI
	# Only writes a surface to a file if it is a simple macrobody
	#------------------------------------------------------------------------------------------------------------------
	def writeSurfaceToFile(self, file):
			
		# BOX (Box)	# http://www.povray.org/documentation/view/3.6.1/276/
		if (self.mnemonic == 'BOX' or self.mnemonic == 'box'):
			if (len(self.data) != 12):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return 0
			targetX = self.data[0] + self.data[3] + self.data[6] + self.data[9];
			targetY = self.data[1] + self.data[4] + self.data[7] + self.data[10];
			targetZ = self.data[2] + self.data[5] + self.data[8] + self.data[11];
			file.writeln("BOX" + "&"
						+ str(self.data[0]) + "&" + str(self.data[1]) + "&" + str(self.data[2] )
						+ "&" + str(targetX) + "&" + str(targetY) + "&" + str(targetZ))
			return 1
		elif (self.mnemonic == 'RPP' or self.mnemonic == 'rpp'):
			if (len(self.data) != 6):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return 0
			file.writeln("BOX" + "&"
						+ str(self.data[0]) + "&" + str(self.data[2]) + "&" + str(self.data[4]) 
						+ "&" + str(self.data[1]) + "&" + str(self.data[3]) + "&" + str(self.data[5]))
			return 1
		# RCC (Right Circular Cylinder)	# http://www.povray.org/documentation/view/3.6.1/278/
		elif (self.mnemonic == 'RCC' or self.mnemonic == 'rcc'): 
			if (len(self.data) != 7):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return 0
			file.writeln("CYLINDER" + "&"
						+ str(self.data[0]) + "&" + str(self.data[1]) + "&" + str(self.data[2]) 
						+ "&" + str(self.data[3]) + "&" + str(self.data[4]) + "&" + str(self.data[5])  + "&" + str(self.data[6]))
			return 1
		# SO -  centered at Origin and radius R  (3000 SO R)
		elif (self.mnemonic == 'SO' or self.mnemonic == 'so' or self.mnemonic == 's0'):
			if (len(self.data) != 1):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return
			file.writeln("SPHERE" + "&"
						+ str(0.0) + "&" + str(0.0) + "&" + str(0.0) 
						+ "&" + str(self.data[0]))
			return 1
		# S -  General  (3000 SO X Y Z R)
		elif (self.mnemonic == 'S' or self.mnemonic == 's'):
			if (len(self.data) != 4):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return
			file.writeln("SPHERE" + "&"
						+ str(self.data[0]) + "&" + str(self.data[1]) + "&" + str(self.data[2]) 
						+ "&" + str(self.data[3]))
			return 1
			
		# SX -  Centered on x-axis  (3000 SX X R)
		elif (self.mnemonic == 'SX' or self.mnemonic == 'sx'):
			if (len(self.data) != 2):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return
			file.writeln("SPHERE" + "&"
						+ str(self.data[0]) + "&" + str(0.0) + "&" + str(0.0) 
						+ "&" + str(self.data[1]))
			return 1
			
		# SY -  Centered on y-axis  (3000 SY Y R)
		elif (self.mnemonic == 'SY' or self.mnemonic == 'sy'):
			if (len(self.data) != 2):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return
			file.writeln("SPHERE" + "&"
						+ str(0.0) + "&" + str(self.data[0]) + "&" + str(0.0) 
						+ "&" + str(self.data[1]))
			return 1
			
		# SZ -  Centered on z-axis  (3000 SZ Z R)
		elif (self.mnemonic == 'SZ' or self.mnemonic == 'sz'):
			if (len(self.data) != 2):
				raise(Exception("ERROR (Write Surface " + str(self.number) + "): Write surface " + str(self.number) + " of type " + str(self.mnemonic) + " to file not succeeded. Not enough arguments."))
				return
			file.writeln("SPHERE" + "&"
						+ str(0.0) + "&" + str(0.0) + "&" + str(self.data[0]) 
						+ "&" + str(self.data[1]))
			return 1
	
	
	# ==> getBoundingBox(file)
	# Returns the approached bounding box (if possible) of the surface 
	# For some surfaces it is not possible and returns 0
	#------------------------------------------------------------------------------------------------------------------
	def getBoundingBox(self):
		bb = 0 # bounding box
		if (self.mnemonic == 'RHP' or self.mnemonic == 'rhp'): 
			# regular grid
			if (len(self.data) == 9):
				base = [float(self.data[0]),float(self.data[1]), float(self.data[2])]
				h = [float(self.data[3]),float(self.data[4]), float(self.data[5])]
				side1 = [float(self.data[6]),float(self.data[7]), float(self.data[8])]
				a = 2*container.Container.lengthVector(side1) / math.sqrt(3.0)
				
				if ( (h[0] != 0.0) and (h[1] == 0.0) and (h[2] == 0.0)):
					bb = BoundingBox.BoundingBox(base[0], base[1]-a, base[2]-a,base[0]+h[0], base[1]+a, base[2]+a)
				elif ( (h[0] == 0.0) and (h[1] != 0.0) and (h[2] == 0.0)):
					bb = BoundingBox.BoundingBox(base[0]-a, base[1], base[2]-a,base[0]+a, base[1]+h[1], base[2]+a)
				elif ( (h[0] == 0.0) and (h[1] == 0.0) and (h[2] != 0.0)):
					bb = BoundingBox.BoundingBox(base[0]-a, base[1]-a, base[2],base[0]+a, base[1]+a, base[2]+h[2])
				else:
					return 0
				return bb
			else:
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			
		# BOX (Box)	# http://www.povray.org/documentation/view/3.6.1/276/
		if (self.mnemonic == 'BOX' or self.mnemonic == 'box'):
			if (len(self.data) != 12):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			targetX = self.data[0] + self.data[3] + self.data[6] + self.data[9];
			targetY = self.data[1] + self.data[4] + self.data[7] + self.data[10];
			targetZ = self.data[2] + self.data[5] + self.data[8] + self.data[11];
			bb = BoundingBox.BoundingBox(self.data[0], self.data[1], self.data[2], 
										targetX, targetY, targetZ)
			return bb
		
		# RPP (rectangular parallelepiped)	# http://www.povray.org/documentation/view/3.6.1/276/
		elif (self.mnemonic == 'RPP' or self.mnemonic == 'rpp'):
			#print "RPP"
			if (len(self.data) != 6):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
				
			bb = BoundingBox.BoundingBox(self.data[0], self.data[2], self.data[4], self.data[1], self.data[3], self.data[5])
			return bb

		# SPH (sphere)	# http://www.povray.org/documentation/view/3.6.1/283/
		elif (self.mnemonic == 'SPH' or self.mnemonic == 'sph'):
			if (len(self.data) != 4):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			r = self.data[3]
			bb = BoundingBox.BoundingBox(self.data[0]-r,self.data[1]-r,self.data[2]-r,self.data[0]+r,self.data[1]+r,self.data[2]+r)
			bb.isSphere = True
			return bb

		# RCC (Right Circular Cylinder)	# http://www.povray.org/documentation/view/3.6.1/278/
		elif (self.mnemonic == 'RCC' or self.mnemonic == 'rcc'): 
			if (len(self.data) != 7):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			
			
			r = self.data[6]
			if ( (self.data[3] != 0.0) and (self.data[4] == 0.0) and (self.data[5] == 0.0)):
				bb = BoundingBox.BoundingBox(self.data[0], min(self.data[1]-r,self.data[4]-r), min(self.data[2]-r,self.data[5]-r),
						self.data[0]+self.data[3], max(self.data[1]+r,self.data[4]+r), max(self.data[2]+r,self.data[5]+r))
			elif ( (self.data[3] == 0.0) and (self.data[4] != 0.0) and (self.data[5] == 0.0)):
				bb = BoundingBox.BoundingBox(min(self.data[0]-r,self.data[3]-r), self.data[1], min(self.data[2]-r,self.data[5]-r),
						max(self.data[0]+r,self.data[3]+r), self.data[1]+self.data[4], max(self.data[2]+r,self.data[5]+r))
			elif ( (self.data[3] == 0.0) and (self.data[4] == 0.0) and (self.data[5] != 0.0)):
				bb = BoundingBox.BoundingBox(min(self.data[0]-r,self.data[3]-r), min(self.data[1]-r,self.data[4]-r), self.data[2],
						max(self.data[0]+r,self.data[3]+r), max(self.data[1]+r,self.data[4]+r), self.data[2]+self.data[5])
			else:
				return 0
			return bb

			
		# SPHERE
		# SO -  centered at Origin and radius R  (3000 SO R)
		if (self.mnemonic == 'SO' or self.mnemonic == 'so' or self.mnemonic == 's0'):
			if (len(self.data) != 1):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			r = self.data[0]
			bb = BoundingBox.BoundingBox(0.0-r, 0.0-r, 0.0-r, 0.0+r, 0.0+r, 0.0+r)
			bb.isSphere = True
			return bb
		# S -  General  (3000 SO X Y Z R)
		if (self.mnemonic == 'S' or self.mnemonic == 's'):
			if (len(self.data) != 4):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			r = self.data[3]
			bb = BoundingBox.BoundingBox(self.data[0]-r, self.data[1]-r, self.data[2]-r, self.data[0]+r, self.data[1]+r, self.data[2]+r)
			bb.isSphere = True
			return bb
		# SX -  Centered on x-axis  (3000 SX X R)
		if (self.mnemonic == 'SX' or self.mnemonic == 'sx'):
			if (len(self.data) != 2):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			r = self.data[1]
			bb = BoundingBox.BoundingBox(self.data[0]-r, 0.0-r, 0.0-r, self.data[0]+r, 0.0+r, 0.0+r)
			bb.isSphere = True
			return bb
		# SY -  Centered on y-axis  (3000 SY Y R)
		if (self.mnemonic == 'SY' or self.mnemonic == 'sy'):
			if (len(self.data) != 2):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			r = self.data[1]
			bb = BoundingBox.BoundingBox(0.0-r, self.data[0]-r, 0.0-r, 0.0+r, self.data[0]+r, 0.0+r)
			bb.isSphere = True
			return bb
		# SZ -  Centered on z-axis  (3000 SZ Z R)
		if (self.mnemonic == 'SZ' or self.mnemonic == 'sz'):
			if (len(self.data) != 2):
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
			r = self.data[1]
			bb = BoundingBox.BoundingBox(0.0-r, 0.0-r, self.data[0]-r, 0.0+r, 0.0+r, self.data[0]+r)
			bb.isSphere = True
			return bb
		
		if (self.mnemonic == 'RHP' or self.mnemonic == 'rhp' or self.mnemonic == 'HEX' or self.mnemonic == 'hex'): 
			# regular grid
			if (len(self.data) == 9):
				offset = self.getHexOffset()
				bb = BoundingBox.BoundingBox()
				base = self.data[0:3]

				if (offset['x'] == 'a'):
					bb.minX =  - (offset['a'] - offset['a']/3.0)
					bb.maxX =  (offset['a'] - offset['a']/3.0)
				elif (offset['y'] == 'a'):
					bb.minY =  - (offset['a'] - offset['a']/3.0)
					bb.maxY =  (offset['a'] - offset['a']/3.0)
				elif (offset['z'] == 'a'):
					bb.minZ =  - (offset['a'] - offset['a']/3.0)
					bb.maxZ =  (offset['a'] - offset['a']/3.0)
				else:
					print "Error (SurfaceCard::getBoudingBox) => ERROR in BB RHP"
				if (offset['x'] == 'b'):
					bb.minX =  - offset['b']/2.0
					bb.maxX =  offset['b']/2.0
				elif (offset['y'] == 'b'):
					bb.minY =  - offset['b']/2.0
					bb.maxY =  offset['b']/2.0
				elif (offset['z'] == 'b'):
					bb.minZ =  - offset['b']/2.0
					bb.maxZ =  offset['b']/2.0
				else:
					print "Error (SurfaceCard::getBoudingBox) => ERROR in BB RHP"
				if (offset['x'] == 'h'):
					bb.minX =  base[0]
					bb.maxX =  base[0] + offset['h']
				elif (offset['y'] == 'h'):
					bb.minY =  base[1]
					bb.maxY =  base[1] + offset['h']
				elif (offset['z'] == 'h'):
					bb.minZ =  base[2]
					bb.maxZ =  base[2] + offset['h']
				else:
					print "Error (SurfaceCard::getBoudingBox) => ERROR in BB RHP"
					
				return bb
			else:
				print "WARNING: Get bounding box for surface " + str(self.number) + " of type " + str(self.mnemonic) + " failed."
				return 0
		
		
	# ==> getHexOffset()
	# Returns the offset of a hexagon shape and the direction
	#------------------------------------------------------------------------------------------------------------------
	def getHexOffset(self):
		if (self.mnemonic == 'RHP' or self.mnemonic == 'rhp' or self.mnemonic == 'HEX' or self.mnemonic == 'hex'): 
			# regular grid
			if (len(self.data) == 9):
				
				base = [float(self.data[0]),float(self.data[1]), float(self.data[2])]
				h = [float(self.data[3]),float(self.data[4]), float(self.data[5])]
				side1 = [float(self.data[6]),float(self.data[7]), float(self.data[8])]
				a = 2.0*container.Container.lengthVector(side1) / math.sqrt(3.0)
				a = math.fabs(a)
				
				hexOffset = [0, 0, 0, 0, 0, 0]
				hexOffset = {'x':0, 'y':0, 'z':0,'a':0, 'b':0, 'h':0}
				if ( (side1[0] != 0.0) and (side1[1] == 0.0) and (side1[2] == 0.0)):
					hexOffset['x'] = 'b'
					hexOffset['b'] = 2 * math.fabs(side1[0])
				elif ( (side1[0] == 0.0) and (side1[1] != 0.0) and (side1[2] == 0.0)):
					hexOffset['y'] = 'b'
					hexOffset['b'] = 2 * math.fabs(side1[1])
				elif ( (side1[0] == 0.0) and (side1[1] == 0.0) and (side1[2] != 0.0)):
					hexOffset['z'] = 'b'
					hexOffset['b'] = 2 * math.fabs(side1[2])
				else:
					return 0
				
				if ( (h[0] != 0.0) and (h[1] == 0.0) and (h[2] == 0.0)):
					hexOffset['x'] = 'h'
					hexOffset['h'] = h[0] - base[0]
				elif ( (h[0] == 0.0) and (h[1] != 0.0) and (h[2] == 0.0)):
					hexOffset['y'] = 'h'
					hexOffset['h'] = h[1] - base[0]
				elif ( (h[0] == 0.0) and (h[1] == 0.0) and (h[2] != 0.0)):
					hexOffset['z'] = 'h'
					hexOffset['h'] = h[2] - base[0]
				else:
					return 0
				
				if ( (hexOffset['x'] == 0.0) and (hexOffset['y'] != 0.0) and (hexOffset['z'] != 0.0)):
					hexOffset['x'] = 'a'
					hexOffset['a'] = a/2.0 + a;# + 4.5
				elif ( (hexOffset['x'] != 0.0) and (hexOffset['y'] == 0.0) and (hexOffset['z'] != 0.0)):
					hexOffset['y'] = 'a'
					hexOffset['a'] = a/2.0 + a
				elif ( (hexOffset['x'] != 0.0) and (hexOffset['y'] != 0.0) and (hexOffset['z'] == 0.0)):
					hexOffset['z'] = 'a'
					hexOffset['a'] = a/2.0 + a
				else:
					return 0
				return hexOffset
			else:
				
				return 0
		return 0
	
	
	# ==> getRectangularOffset(isMin)
	# Returns the offset of a rectangular surface
	# is used for lattices
	# so also the bounds of planes is added to it
	# functionality is limited to PX, PY, PZ, BOX and RPP
	# returns offset [minX, minY, minZ, maxX, maxY, maxZ]
	#------------------------------------------------------------------------------------------------------------------
	def getRectangularOffset(self, isMin):

		print self
		if (self.mnemonic == 'PX' or self.mnemonic == 'px'):
			if (len(self.data) != 1):
				print str(len(self.data))
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type PX has not enough or too much arguments (" + str(len(self.data)) + " instead of 1)"))
				return
			if (isMin):
				return ['inf', 'inf', 'inf', self.data[0], 'inf', 'inf']
			else:
				return [self.data[0], 'inf', 'inf', 'inf', 'inf', 'inf']
		# PY - PLANE with normal to y-axis and D (3000 PY D)
		if (self.mnemonic == 'PY' or self.mnemonic == 'py'):
			if (len(self.data) != 1):
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type PY has not enough or too much arguments (" + str(len(self.data)) + " instead of 1)"))
				return
			if (isMin):
				return ['inf', 'inf', 'inf', 'inf', self.data[0], 'inf']
			else:
				return ['inf', self.data[0], 'inf', 'inf', 'inf', 'inf']
		# PZ - PLANE with normal to z-axis and D (3000 PZ D)
		if (self.mnemonic == 'PZ' or self.mnemonic == 'pz'):
			if (len(self.data) != 1):
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type PZ has not enough or too much arguments (" + str(len(self.data)) + " instead of 1)"))
				return
			if (isMin):
				return ['inf', 'inf', 'inf', 'inf', 'inf', self.data[0]]
			else:
				return ['inf', 'inf', self.data[0], 'inf', 'inf', 'inf']
		# RPP (rectangular parallellepiped)	# http://www.povray.org/documentation/view/3.6.1/276/
		if (self.mnemonic == 'RPP' or self.mnemonic == 'rpp'):
			if (len(self.data) != 6):
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type RPP has not enough or too much arguments (" + str(len(self.data)) + " instead of 6)"))
				return 0
			return [self.data[0], self.data[2], self.data[4], self.data[1], self.data[3], self.data[5]]
		
		if (self.mnemonic == 'BOX' or self.mnemonic == 'box'):
			print self.data
			if (len(self.data) != 12):
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type BOX has not enough or too much arguments (" + str(len(self.data)) + " instead of 12)"))
				return 0
			targetX = self.data[0] + self.data[3] + self.data[6] + self.data[9];
			targetY = self.data[1] + self.data[4] + self.data[7] + self.data[10];
			targetZ = self.data[2] + self.data[5] + self.data[8] + self.data[11];
			return [self.data[0], self.data[1], self.data[2], targetX, targetY, targetZ]
		# CX - CYLINDER parallel to x-axis
		if (self.mnemonic == 'CX' or self.mnemonic == 'cx'):
			if (len(self.data) != 1):
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type CX has not enough or too much arguments (" + str(len(self.data)) + " instead of 1)"))
				return
			if (isMin):
				return ['inf', -self.data[0], -self.data[0], 'inf', self.data[0], self.data[0]]
			else:
				return ['inf', -self.data[0], -self.data[0], 'inf', self.data[0], self.data[0]]
		# CY - CYLINDER parallel to y-axis
		if (self.mnemonic == 'CY' or self.mnemonic == 'cy'):
			if (len(self.data) != 1):
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type CY has not enough or too much arguments (" + str(len(self.data)) + " instead of 1)"))
				return
			if (isMin):
				return [-self.data[0], 'inf', -self.data[0], self.data[0], 'inf', self.data[0]]
			else:
				return [-self.data[0], 'inf', -self.data[0], self.data[0], 'inf', self.data[0]]
		# CZ - CYLINDER parallel to z-axis
		if (self.mnemonic == 'CZ' or self.mnemonic == 'cz'):
			if (len(self.data) != 1):
				raise(Exception("ERROR (Build Surface Card " + str(self.number) + "): Surface " + str(self.number) + " of type CZ has not enough or too much arguments (" + str(len(self.data)) + " instead of 1)"))
				return
			if (isMin):
				return [-self.data[0], -self.data[0], 'inf', self.data[0], self.data[0], 'inf']
			else:
				return [-self.data[0], -self.data[0], 'inf', self.data[0], self.data[0], 'inf']
		if (self.mnemonic == 'RHP' or self.mnemonic == 'rhp' or self.mnemonic == 'HEX' or self.mnemonic == 'hex'): 
			offset = self.getHexOffset()
			offsetX = offset[offset['x']]
			offsetY = offset[offset['y']]
			offsetZ = offset[offset['z']]
			return [-offsetX, -offsetY, -offsetZ, offsetX, offsetY, offsetZ]
			
		return 0
				