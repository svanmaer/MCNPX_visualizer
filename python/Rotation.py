#######################################################################################################################
## Rotation.py
#######################################################################################################################
##
## Class to represent a rotation
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################


class Rotation:

	def __init__(self, xx=1.0, yx=0.0, zx=0.0, xy=0.0, yy=1.0, zy=0.0, xz=0.0, yz=0.0, zz=1.0):
		self.xx = xx
		self.yx = yx
		self.zx = zx
		self.xy = xy
		self.yy = yy
		self.zy = zy
		self.xz = xz
		self.yz = yz
		self.zz = zz

	def __repr__(self):
		return "Rotation()"
	def __str__(self):
		return "Rotation (" + str(self.xx) + "," + str(self.yx) + "," + str(self.zx) + "," + str(self.xy) + "," + str(self.yy) + "," + str(self.zy) + "," + str(self.xz) + "," + str(self.yz) + "," + str(self.zz) + ")"
