# -*- coding: utf-8 -*-
#
# builds a house xml used for the homepage of current multi definitions
#

import DataUnPacker
from ServerUtils import sanitize

import sys, os, time, codecs
from pprint import pprint

def buildHouseXML(root_path, xml):
  cfg = os.path.join(root_path,'pkg','std','housing','itemdesc.cfg')
  if not os.path.exists(cfg):
    return False
  houses=dict()
  items = dict()
  housetypes = set()
  for typ, name, elem in DataUnPacker.DatafileReader(cfg,is_cfg=True):
    if typ == 'House':
      if 'MultiID' in elem:
        houses[elem['Name'][0]]=(int(name,16),elem)
    elif typ == 'Item':
      if 'Script' in elem:
        if elem['Script'][0] == 'housedeed':
          if int(elem.get('VendorSellsFor',['0'])[0]):
            if 'HouseObjType' in elem:
              items[int(name,16)] = elem
              housetypes.add(elem.get('HouseType',['UNKNOWN'])[0])
  
  housetypes = sorted(list(housetypes))
  pprint(housetypes)
  usedtypes = dict()
   
  mrcspawn = os.path.join(root_path,'config','mrcspawn.cfg')
  if not os.path.exists(cfg):
    return False
  sellable = []
  for typ, name, elem in DataUnPacker.DatafileReader(mrcspawn,is_cfg=True):
    if typ == 'ProductGroup':
      if name == 'Deeds_S':
        for item in elem['Item']:
          l = item.split()
          sellable.append(int(l[0],16))
        break
  
  for deed_obj, deed_elem in items.items():
    if deed_elem['HouseObjType'][0] not in houses:
      continue
    house_elem = houses[deed_elem['HouseObjType'][0]][1]
    multiid = int(house_elem['MultiID'][0],16)
    if (deed_obj not in sellable):
      print('ignoring 0x{:X}: 0x{:X}'.format(multiid,deed_obj))
      continue
    h_type=deed_elem.get('HouseType',['UNKNOWN'])[0]
    usedtypes[h_type]=usedtypes.get(h_type,0)+1
    xml.write('  <house multiid="0x{:X}">\n'.format(multiid))
    name = deed_elem['Desc'][0]
    name = name.replace( # gnaaa...
      'Baupla%ene/n% fuer einen ','').replace(
      'Baupla%ene/n% fuer eine ','').replace(
      'Baupla%ene/n% fuer ein ','').replace(
      'Baupla%ene/n% fuer ','').replace(
      'Baupla%ene/n% ','').replace(
      'Aufbauanleitung%en% fuer ein ','').strip()
    xml.write('    <name>{}</name>\n'                .format( sanitize(name) )) 
    xml.write('    <type>{}</type>\n'                .format( deed_elem.get( 'HouseType', ['UNKNOWN'] )[0] )) 
    xml.write('    <typeid>{}</typeid>\n'            .format( housetypes.index(deed_elem.get( 'HouseType', ['UNKNOWN'] )[0] ))) 
    xml.write('    <gold>{}</gold>\n'                .format( deed_elem['VendorSellsFor'][0] )) 
    xml.write('    <ingots>{}</ingots>\n'            .format( deed_elem.get( 'Barren',    [0] )[0] ))
    xml.write('    <boards>{}</boards>\n'            .format( deed_elem.get( 'Bretter',   [0] )[0] ))
    xml.write('    <granites>{}</granites>\n'        .format( deed_elem.get( 'Granit',    [0] )[0] ))
    xml.write('    <leathers>{}</leathers>\n'        .format( deed_elem.get( 'Leder',     [0] )[0] ))
    xml.write('    <claystones>{}</claystones>\n'    .format( deed_elem.get( 'Lehm',      [0] )[0] ))
    xml.write('    <marble>{}</marble>\n'            .format( deed_elem.get( 'Marmor',    [0] )[0] ))
    xml.write('    <sandstones>{}</sandstones>\n'    .format( deed_elem.get( 'Sandstein', [0] )[0] ))
    xml.write('    <trunks>{}</trunks>\n'            .format( deed_elem.get( 'Staemme',   [0] )[0] ))
    xml.write('    <clothes>{}</clothes>\n'          .format( deed_elem.get( 'Stoff',     [0] )[0] ))
    xml.write('    <straws>{}</straws>\n'            .format( deed_elem.get( 'Stroh',     [0] )[0] ))
    xml.write('    <crystals>{}</crystals>\n'        .format( deed_elem.get( 'Kristall',  [0] )[0] ))
    xml.write('  </house>\n')

  pprint(usedtypes)    
  return True

def write_xml(server_root, xmlpath):
  with codecs.open(xmlpath,'w','ISO-8859-1') as f:
    f.write('<?xml version="1.0" encoding="ISO-8859-1" ?>\n')
    f.write('<houses>\n')
    buildHouseXML(server_root, f )
    f.write('</houses>\n')
  return True
        
if __name__ == '__main__':
  path = '/gameworld/Pol/'
  s=time.time()
  write_xml(path,'results/PergonHouses.xml')
  e=time.time()
  print('total time : {}'.format(e-s))

