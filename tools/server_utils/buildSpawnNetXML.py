# -*- coding: utf-8 -*-
#
# builds a xml file of current spawnnet data used inside of the fiddler plugin
#


import DataUnPacker
from ServerUtils import sanitize
import sys, os, time, codecs
from pprint import pprint

def get_datafiles(root_path):
  datastore = os.path.join(root_path,'datastore.txt')
  regionroot = os.path.join(root_path,'ds','spawnnet_new')
  regionsGroup = None
  regionsUser = None
  runePos = None
  for typ, name, elem in DataUnPacker.DatafileReader(datastore):
    if typ != 'DataFile':
      continue
    if 'Package' in elem and elem['Package'] == 'spawnnet_new':
      version = int(elem['Version'])
      file_end = '.{}.txt'.format((version%10))
      f = os.path.join(regionroot,elem['Name']+file_end)
      if os.path.exists(os.path.join(regionroot,elem['Name']+file_end)):
        if elem['Name'] == 'RegionsGroup':
          regionsGroup = f
        elif elem['Name'] == 'RegionsUser':
          regionsUser = f
        elif elem['Name'] == 'RunePos':
          runePos = f
      if regionsGroup is not None and regionsUser is not None and runePos is not None:
        break
  return (regionsGroup, regionsUser, runePos)
      
def write_spawnregions(xml, regionsGroup, regionUser):
  groups={}
  for typ, name, elem in DataUnPacker.DatafileReader(regionsGroup, u_members=None, is_datafile=True):
    for k,v in elem.items():
      if k != '__order__':
        if isinstance(v, DataUnPacker.PolObject):
          elem[k]=v.real
    groups[name]=elem
    
  for typ, name, elem in DataUnPacker.DatafileReader(regionUser, u_members=None, is_datafile=True):
    if name=='Realms':
      continue
    if elem['realm'].real != 'britannia':
      continue
    userdata = elem['userdata'].real
    xml.write('  <region>\n')
    xml.write('    <name>{}</name>\n'.format(name))
    xml.write('    <x1>{}</x1>\n'.format(userdata['koords'][0]))
    xml.write('    <x2>{}</x2>\n'.format(userdata['koords'][2]))
    xml.write('    <y1>{}</y1>\n'.format(userdata['koords'][1]))
    xml.write('    <y2>{}</y2>\n'.format(userdata['koords'][3]))
    xml.write('    <mintime>{}</mintime>\n'.format(userdata['mintime']))
    xml.write('    <maxtime>{}</maxtime>\n'.format(userdata['maxtime']))

    for group_key, group_value in userdata['groups'].items():
      xml.write('    <group>\n')
      xml.write('      <name>{}</name>\n'.format(group_key))
      xml.write('      <enabled>{}</enabled>\n'.format(group_value['enabled']))
      xml.write('      <max_amt>{}</max_amt>\n'.format(group_value['max_amt']))
      xml.write('      <spawn_factor>{}</spawn_factor>\n'.format(group_value['spawn_factor']))
      if group_key in groups:
        group = groups[group_key]
        xml.write('      <containerobjtype>{}</containerobjtype>\n'.format(group['Containerobjtype']))
        xml.write('      <flags>{}</flags>\n'.format(group['Flags']))
        xml.write('      <groupingamt>{}</groupingamt>\n'.format(group['GroupingAmt']))
        xml.write('      <spawntype>{}</spawntype>\n'.format(group['Spawntype']))
        xml.write('      <stackamount>{}</stackamount>\n'.format(group['StackAmount']))
        for entry_key, entry_value in group['EntryList'].items():
          xml.write('      <template procent="{}">{}</template>\n'.format(entry_value,entry_key))
      else:
        xml.write('      <containerobjtype>{}</containerobjtype>\n'.format(0))
        xml.write('      <flags>{}</flags>\n'.format(0))
        xml.write('      <groupingamt>{}</groupingamt>\n'.format(0))
        xml.write('      <spawntype>{}</spawntype>\n'.format(0))
        xml.write('      <stackamount>{}</stackamount>\n'.format(0))
        xml.write('      <template procent="{}">{}</template>\n'.format(0,''))

      xml.write('    </group>\n')
    xml.write('  </region>\n')
  
def write_runes(xml, datapath):
  items = os.path.join(datapath,'items.txt')
  for typ, name, elem in DataUnPacker.DatafileReader(items, objtypes=[0xa300]):
    if elem['Realm'] != 'britannia':
      continue
    if 'Container' in elem:
      continue
    if 'CProp' not in elem:
      continue
    if 'userdata' not in elem['CProp'] or 'spawndata' not in elem['CProp']:
      continue
    
    userdata = DataUnPacker.DeSerialize(elem['CProp']['userdata']).unpack().real
    spawndata = DataUnPacker.DeSerialize(elem['CProp']['spawndata']).unpack().real

    xml.write('  <rune>\n')
    xml.write('    <x>{}</x>\n'.format(elem['X']))
    xml.write('    <y>{}</y>\n'.format(elem['Y']))
    xml.write('    <template>{}</template>\n'.format(sanitize(userdata['template'])))
    xml.write('    <amount>{}</amount>\n'.format(userdata['amount']))
    xml.write('    <range>{}</range>\n'.format(userdata['range']))
    xml.write('    <mintime>{}</mintime>\n'.format(userdata['mintime']))
    xml.write('    <maxtime>{}</maxtime>\n'.format(userdata['maxtime']))
    xml.write('    <flags>{}</flags>\n'.format(userdata['flags']))
    xml.write('    <spawntype>{}</spawntype>\n'.format(userdata['spawntype']))
    xml.write('    <stackamount>{}</stackamount>\n'.format(userdata['stackamount']))
    if (str.lower(str(userdata['containertype'])) not in ['<keine>','<keiner>']):
      xml.write('    <containertype>{}</containertype>\n'.format(sanitize(userdata['containertype'])))
    if (str.lower(str(userdata['note'])) not in ['<none>','<keiner>']):
      xml.write('    <note>{}</note>\n'.format(sanitize(userdata['note'])))
    xml.write('    <questnr>{}</questnr>\n'.format(userdata['questnr']))
    xml.write('    <grouping>{}</grouping>\n'.format(userdata['group']))
    if ('templatearray' in spawndata and len(spawndata['templatearray'])):
      xml.write('    <group>')
      xml.write('{}'.format(', '.join([str(g) for g in set(spawndata['templatearray'])])))
      xml.write('</group>\n')
    xml.write('  </rune>\n')

    
def write_xml(datapath, xmlpath):
  regionsGroup, regionsUser, runePos = get_datafiles(datapath)
  if regionsGroup is None or regionsUser is None:
    return False
  
  with codecs.open(xmlpath,'w','ISO-8859-1') as f:
    f.write('<?xml version="1.0" encoding="ISO-8859-1" ?>\n')
    f.write('<Runen>\n')
    write_runes(f, datapath)
    write_spawnregions(f,regionsGroup,regionsUser)
    f.write('</Runen>\n')
  return True
        
if __name__ == '__main__':
  path = '/gameworld/Pol/data/'
  s=time.time()
  write_xml(path,'results/PergonSpawnNet.xml')
  e=time.time()
  print('total time : {}'.format(e-s))

