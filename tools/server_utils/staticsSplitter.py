import DataUnPacker

movablestatics = '../../pol/config/statics/items.cfg'
movablestatics = 'd:/items.cfg'

movablestatics = DataUnPacker.DatafileReader(movablestatics, is_cfg=True)

i_cont=0
i_cont_misc=0
i_rest=0
i_inval=0
i_light=0
i_anbind=0
with open('lockables_contdoors.cfg', 'w') as f_cont:
  with open('lockables_misc.cfg', 'w') as f_misc:
    with open('rest.cfg', 'w') as f_rest:
      with open('invalid.cfg', 'w') as f_inval:
        with open('light.cfg', 'w') as f_light:
          with open('anbindpfosten.cfg', 'w') as f_anbind:
            for t, n, elem in movablestatics:
              if elem.get('Script',[''])[0] == 'sign' or elem.get('Script',[''])[0] == 'staticsign':
                DataUnPacker.packOntoFile(f_inval, (t, n, elem),False)
                i_inval+=1
                continue
              if elem.get('Script',[''])[0] == 'change':
                elem['ID']=[int(elem.get('ObjType')[0][2:],16)] # fiddler brauch id zum freezen
                DataUnPacker.packOntoFile(f_light, (t, n, elem),False)
                i_light+=1
                continue
              if elem.get('Script',[''])[0] == 'parking':
                DataUnPacker.packOntoFile(f_anbind, (t, n, elem),False)
                i_anbind+=1
                continue
                
              if elem.get('Type',[''])[0] == 'Lockable':
                if elem.get('Script',[''])[0] == '::items/opencont':
                  DataUnPacker.packOntoFile(f_cont, (t, n, elem),False)
                  i_cont+=1
                elif elem.get('Script',[''])[0] == 'use' and elem.get('ControlScript',[''])[0] == 'control':
                  DataUnPacker.packOntoFile(f_cont, (t, n, elem),False)
                  i_cont+=1
                else:
                  DataUnPacker.packOntoFile(f_misc, (t, n, elem),False)
                  i_cont_misc+=1
                continue
                
              DataUnPacker.packOntoFile(f_rest, (t, n, elem),False)
              i_rest+=1
      
print('container',i_cont)
print('container misc',i_cont_misc)
print('rest',i_rest)
print('invalid',i_inval)
print('light',i_light)
print('anbind',i_anbind)
