#######################################################################################################################
## container.py
#######################################################################################################################
##
## Container class contains some diverse mathematical static functionality
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################


import re
import math
from sympy import core, solvers, matrices

class Container:

	# ==> remove_values_from_list(the_list, val)
	# Remove all the occurrences from 'val' out of 'the_list'
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def remove_values_from_list(the_list, val):
		if (not isinstance(the_list, list) ):
			return
		while val in the_list:
			the_list.remove(val)
			
			
	# ==> isFloat(s)
	# Check if a value has a float notation (take also into account the scientific E notation)
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod	
	def isFloat(self, s):
		if (re.search('\.', s) or re.search('E', s) or re.search('e', s)):
			return True
		else:
			return False
			
	# ==> cross(v1, v2)
	# Cross product between vector v1 and v2
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod	
	def cross(v1, v2):
		cr = [0.0, 0.0, 0.0]
		cr[0] = v1[1] * v2[2] - v1[2] * v2[1];
		cr[1] = v1[2] * v2[0] - v1[0] * v2[2];
		cr[2] = v1[0] * v2[1] - v1[1] * v2[0];
		return cr
	
	# ==> dot(v1, v2)
	# Dot product between vector v1 and v2
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def dot(v1, v2):
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]

	# ==> angle(v1, v2)
	# Angle between vector v1 and v2
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def angle(v1, v2):
		# returns acos in radians
		return math.acos(Container.dot(v1,v2))
	
	# ==> lineParameterOfIntersection(p1, v1, p2, v2)
	# calculate the parameters of the two 3D lines where they intersect
	# 		p1: position vector 1
	# 		v1: direction vector 1
	# 		p2: position vector 2
	# 		v2: direction vector 2
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def lineParameterOfIntersection(p1, v1, p2, v2):
		t1 = core.Symbol('t1')
		t2 = core.Symbol('t2')
		solved =  solvers.solve([p1[0]+v1[0]*t1 - p2[0] - v2[0]*t2, p1[1]+v1[1]*t1 - p2[1] - v2[1]*t2, p1[2]+v1[2]*t1 - p2[2] - v2[2]*t2], t1, t2)
		result = {}
		if (solved.has_key(t1)):
			result['t1'] = solved[t1]
		if (solved.has_key(t2)):
			result['t2'] = solved[t2]
		return result
	

	# ==> angle(p, v, t)
	# calculate the coordinate of a line based on a parameters
	# 		p: position vector
	#		v: direction vector
	#		t: parameter
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def coordinateLine(p, v, t):
		coordinate = []
		coordinate[0] = p[0]+v[0]*t
		coordinate[1] = p[1]+v[1]*t
		coordinate[2] = p[2]+v[2]*t
		return coordinate
	
	# ==> normalizeVector(v)
	# normalize vector
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def normalizeVector(v):
		norm = Container.lengthVector(v)
		v[0] = v[0] / norm
		v[1] = v[1] / norm
		v[2] = v[2] / norm
		return v
	
	# ==> getNormalizedVector(v)
	# Returns the normalized vector without changing the original one
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def getNormalizedVector(v):
		normV = [0.0, 0.0, 0.0]
		norm = Container.lengthVector(v)
		normV[0] = v[0] / norm
		normV[1] = v[1] / norm
		normV[2] = v[2] / norm
		return normV
	
	# ==> lengthVector(v)
	# Returns the norm of the vector v
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def lengthVector(v):
		return math.sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]))
	
	# ==> multVector(v, number)
	# Multiply a vector with a constant
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def multVector(v, number):
		v[0] = number * v[0]
		v[1] = number * v[1]
		v[2] = number * v[2]
		return v
	
	# ==> rotateVectorAroundNormal(point, n, angle, turnAroundPoint = [0.0, 0.0, 0.0]):
	# Return a vector around a plane (defined by the normal)
	#		point: point in space
	# 		n: normal of the plane
	#		angle: degrees of rotating
	#		turnAroundPoint: central point of plane where the point turns around
	#------------------------------------------------------------------------------------------------------------------
	@staticmethod
	def rotateVectorAroundNormal(point, n, angle, turnAroundPoint = [0.0, 0.0, 0.0]):
		p = matrices.Matrix(4, 1,(point[0],point[1],point[2],1.0))
		mul = matrices.Matrix((
				[n[0]*n[0] + (n[1]*n[1] + n[2]*n[2])*math.cos(angle), n[0]*n[1]*(1.0-math.cos(angle)) - n[2]*math.sin(angle), n[0]*n[2]*(1.0-math.cos(angle)) + n[1]*math.sin(angle), 0.0],
				[n[0]*n[1]*(1.0 - math.cos(angle)) + n[2]*math.sin(angle), n[1]*n[1] + (n[0]*n[0] + n[2]*n[2])*math.cos(angle), n[1]*n[2]*(1.0-math.cos(angle)) - n[0]*math.sin(angle), 0.0],
				[n[0]*n[2]*(1.0 - math.cos(angle)) - n[1]*math.sin(angle), n[1]*n[2]*(1.0 - math.cos(angle)) - n[0]*math.sin(angle), n[2]*n[2] + (n[0]*n[0]+n[1]*n[1])*math.cos(angle), 0.0],
				[0.0, 0.0, 0.0, 1.0]))
		res =  mul.multiply(p)
		return res[:3]