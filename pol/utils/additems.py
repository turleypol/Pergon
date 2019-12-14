import DataUnPacker

items = '../data/items.txt'
npcequip = '../data/npcequip.txt'
pcequip = '../data/pcequip.txt'
storage = '../data/storage.txt'
add = 'add.txt'
mergedata = []
serials = {}

items = DataUnPacker.DatafileReader(items, u_members=['Serial'])
for t, n, elem in items:
    if 'Serial' in elem:
        serials[elem['Serial']] = 1
npcequip = DataUnPacker.DatafileReader(npcequip, u_members=['Serial'])
for t, n, elem in npcequip:
    if 'Serial' in elem:
        serials[elem['Serial']] = 1
pcequip = DataUnPacker.DatafileReader(pcequip, u_members=['Serial'])
for t, n, elem in pcequip:
    if 'Serial' in elem:
        serials[elem['Serial']] = 1
storage = DataUnPacker.DatafileReader(storage, u_members=['Serial'])
for t, n, elem in storage:
    if 'Serial' in elem:
        serials[elem['Serial']] = 1
add = DataUnPacker.DatafileReader(add, u_members=['Serial'])
for t, n, elem in add:
    if elem['Serial'] not in serials:
        mergedata.append((t, n, elem))
with open('items-add.txt', 'w') as f:
  for e in mergedata:
    e[2]['Serial'] = '{:#x}'.format(e[2]['Serial'])
    DataUnPacker.packOntoFile(f, e)
