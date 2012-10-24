#######################################################################################################################
## DataHolder.py
#######################################################################################################################
##
## Class to represent a datavalue
## When setted, it directly gives back the value
## Good when you want to use a re.match statement in an if-test
##
## Part of MCNPX Visualizer
## (c) Nick Michiels for SCK-CEN Mol (2011)
#######################################################################################################################

class DataHolder(object):
        
        
    def set(self, value): self.value = value; return value