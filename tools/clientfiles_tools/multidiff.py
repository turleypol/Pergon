# -*- coding: utf-8 -*-
#
# prints differences between two multi files
#

import os
import sys
import struct
from pprint import pprint as print

class IDX_STRUCT:
  size = 12
  def __init__(self,bytes):
    unpack = struct.unpack('III',bytes)#offset,length,unknown
    self.offset = unpack[0]
    self.length = unpack[1]
    self.unknown = unpack[2]

class ELEM_STRUCT:
  size = 12
  def __init__(self,bytes):
    unpack = struct.unpack('HhhhI',bytes)#graphic,x,y,z,flags
    self.graphic = unpack[0]
    self.x = unpack[1]
    self.y = unpack[2]
    self.z = unpack[3]
    self.flags = unpack[4]
  def __repr__(self):
    return '0x{:<4X} {:<3} {:<3} {:<3} {}'.format(self.graphic,self.x,self.y,self.z,self.flags)
  
  def __eq__(self, other):
    return (self.graphic == other.graphic and self.x == other.x and self.y == other.y and self.z == other.z and self.flags == other.flags)
     
def read_idx(file):
  multis = []
  with open(file,'rb') as f:
    while True:
      bytes = f.read(IDX_STRUCT.size)
      if not bytes:
          break
      multis.append(IDX_STRUCT(bytes))
  return multis
  
def read_multi(file, multidefs):
  multi={}
  with open(file,'rb') as f:
    for i,idx in enumerate(multidefs):
      if idx.offset == 0xFFffFFff:
        multi[i]=[]
        continue
      f.seek(idx.offset)
      elem_count = idx.length//ELEM_STRUCT.size
      elems=[]
      for c in range(elem_count):
        bytes = f.read(ELEM_STRUCT.size)
        if not bytes:
          print('failed to read elem {} in 0x{:X} {}'.format(c,i,bytes))
          break
        elems.append(ELEM_STRUCT(bytes))
      multi[i]=elems
  return multi
      
def compare_multis(multis1, multis2):
  if len(multis1) != len(multis2):
    print('different idx size')
    return
  for m1,m2 in zip(multis1.items(),multis2.items()):
    if len(m1[1]) != len(m2[1]):
      print('different number of elements in 0x{:X}'.format(m1[0]))
      continue
      
    for e1,e2 in zip(m1[1],m2[1]):
      if e1 == e2:
        continue
      print('different 0x{:X} {} != {}'.format (m1[0],e1,e2))
      break

if __name__ == '__main__':
  if len(sys.argv)!=3:
    print('need 2 parameters {multi location1} {multi location2}')
    sys.exit(0)
  location1= sys.argv[1]
  location2= sys.argv[2]
  
  print('reading location1 {}'.format(location1))
  multidefs = read_idx(os.path.join(location1,'multi.idx'))
  multis = read_multi(os.path.join(location1,'multi.mul'),multidefs)
  print('reading location2 {}'.format(location2))
  multidefs2 = read_idx(os.path.join(location2,'multi.idx'))
  multis2 = read_multi(os.path.join(location2,'multi.mul'),multidefs2)
  
  print('comparing')
  compare_multis(multis,multis2)
