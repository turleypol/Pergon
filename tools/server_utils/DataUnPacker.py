# -*- coding: utf-8 -*-
import traceback 
import sys
import codecs
from functools import partial
from pprint import pprint

# wrapper class for string lines, to avoid copies
class StrWrapper(object):
  def __init__(self, string):
    self._string = string
    self._i = 0
    self._max = len(string)
    
  # return via instance[i] char at given pos
  def __getitem__(self,i):
    return self._string[i]
  # print some informations
  def __str__(self):
    return '{}   {}/{}'.format(self._string,self._i,self._max)
    
  # return next char in string 
  @property
  def next(self):
    s=self._string[self._i]
    self._i+=1
    return s
  
  # return next char in string without increasing index var
  @property
  def peak(self):
    return self._string[self._i]
    
  # return and give up control of remaining string
  def move(self):
    i=self._i
    self._i = self._max
    return self._string[i:]
      
  # return current index
  @property
  def index(self):
    return self._i
  # set current index
  @index.setter
  def index(self,i):
    self._i=i
    return i

  # python 3.x bool check
  def __bool__(self):
    return self._i < self._max
  # python 2.x bool check
  def __nonzero__(self):
    return self._i < self._max
    
  # like str.find from current position
  def find(self,char):
    return self._string.find(char,self._i)
  # like str.splice from current position till given
  def splice(self,index):
    s = self._string[self._i:index]
    self._i = index+1
    return s

# base class for pol types
class PolObject(object):
  def __init__(self, value):
    self._value = None
    self._setvalue(value)
  # get value
  @property  
  def value(self):
    return self._value
  # set value
  @value.setter
  def value(value):
    return self._setvalue(value)
  # convert it into python base types
  @property
  def real(self):
    return self._value
    
# pol string class
class PolString (PolObject):
  def __init__(self, value):
    PolObject.__init__(self,value)
    
  def _setvalue(self, value):
    if (isinstance(value,str)):
      self._value = value
    else:
      self._value = ""
    return self._value
    
  def __repr__(self):
    return self.__str__()
  def __str__(self):
    return 'String '+self._value
    
  @staticmethod
  def unpack(string, has_len):
    if (not has_len): # complete line is string
      return PolString(string.move())
    else:
      # S7:basdasd
      cnt = int(string.splice(string.find(':')))
      s=''
      while cnt:
        cnt -=1
        if string:
          s+= string.next
      return PolString(s)
  def pack(self, string):
    string += 'S{}:{}'.format(len(self._value),self._value)
    return string
    
# int pol type
class PolInt (PolObject):
  
  def __init__(self, value):
    PolObject.__init__(self, value)
    
  def _setvalue(self, value):
    if (isinstance(value,int)):
      self._value = value
    elif (isinstance(value,float)):
      self._value = int(value)
    else:
      self._value = 0
    return self._value
    
  def __repr__(self):
    return self.__str__()
  def __str__(self):
    return 'Int {}'.format(self._value)
    
  @staticmethod
  def unpack(string):
    s=""
    if string.peak == '-':
      s=string.next
    while string:
      if string.peak.isdigit():
        s+=string.next
        continue
      break
    if len(s):
      return PolInt(int(s))
  def pack(self, string):
    string += 'i{}'.format(self._value)
    return string
    
# double pol type
class PolDouble (PolObject):
  def __init__(self, value):
    PolObject.__init__(self, value)
    
  def _setvalue(self, value):
    if (isinstance(value,int)):
      self._value = float(value)
    elif (isinstance(value,float)):
      self._value = value
    else:
      self._value = 0.0
    return self._value
    
  def __repr__(self):
    return self.__str__()
  def __str__(self):
    return 'Double {}'.format(self._value)
    
  @staticmethod
  def unpack(string):
    s=""
    if string.peak in ['+','-']:
      s=string.next
    while string:
      c = string.next
      if c.isdigit() or c =='.':
        s+=c
        continue
      if c=='e': # science notation
        if string.peak in ['+','-']: # but could also be an error type
          s+= c+string.next
          continue
      string.index=string.index-1 # failure reset index
      break
    if len(s):
      return PolDouble(float(s))
  
  def pack(self, string):
    string += 'r{}'.format(self._value)
    return string
    
# error pol type
class PolError (PolObject):
  def __init__(self, value):
    PolObject.__init__(self, value)
    
  def _setvalue(self, value):
    if (isinstance(value,dict)):
      self._value = value
    else:
      self._value = {"error":str(value)}
    return self._value
    
  @property
  def real(self):
    return { str(k):str(v) for k,v in self._value.items() }
      
  def __repr__(self):
    return self.__str__()
  def __str__(self):
    r="Error {"
    for k,v in self._value.items():
      r+="{}:{}, ".format(k,v)
    return r.rstrip(', ')+"}"
    
  @staticmethod
  def unpack(string):
    # its basically a struct
    cnt = int(string.splice(string.find(':')))
    result = {}
    while (cnt):
      cnt-=1
      key = DeSerialize.unpack_helper(string)
      value = DeSerialize.unpack_helper(string)
      if (key is not None and value is not None):
        result[str(key.value)]=value
    return PolError(result)
  
  def pack(self, string):
    _s = PolString(self._value.keys()[0]).pack('')
    string += 'e1:{}{}'.format(_s, self._value[self._value.keys()[0]].pack(''))
    return string
    
# uninit pol type
class PolUnInit (PolObject):
  def __init__(self, value):
    PolObject.__init__(self, value)
    
  def _setvalue(self, value):
    self._value = None
    return self._value
    
  def __repr__(self):
    return self.__str__()
  def __str__(self):
    return "Uninit"
    
  @staticmethod
  def unpack(string):
    return PolUnInit(None)
  def pack(self, string):
    string +='x'
    return string
    
# array pol type
class PolArray (PolObject):
  def __init__(self, value):
    PolObject.__init__(self, value)
    
  def _setvalue(self, value):
    if (isinstance(value,list)):
      self._value = value
    else:
      self._value = []
    return self._value
    
  @property
  def real(self):
    return [v.real for v in self._value]

  def __repr__(self):
    return self.__str__()
  def __str__(self):
    r="Array {"
    for v in self._value:
      r+=str(v)+", "
    return r.rstrip(', ')+"}"
    
  @staticmethod
  def unpack(string): 
    cnt = int(string.splice(string.find(':')))
    result = []
    while (cnt):
      cnt-=1
      res = DeSerialize.unpack_helper(string)
      if (res is not None):
        result.append(res)
    return PolArray(result)
    
  def pack(self, string):
    string += 'a{}:{}'.format(len(self._value),
      ''.join(v.pack('') for v in self._value))
    return string
   
#dictionary pol type
class PolDictionary (PolObject):
  def __init__(self, value):
    PolObject.__init__(self, value)
    
  def _setvalue(self, value):
    if (isinstance(value,dict)):
      self._value = value
    else:
      self._value = {}
    return self._value
    
  @property
  def real(self):
    return { k.real : v.real for k,v in self._value.items() }
      
  def __repr__(self):
    return self.__str__()
  def __str__(self):
    r="Dict {"
    for k,v in self._value.items():
      r+="{}:{}, ".format(k,v)
    return r.rstrip(', ')+"}"
    
  @staticmethod
  def unpack(string): 
    cnt = int(string.splice(string.find(':')))
    result = {}
    while (cnt):
      cnt-=1
      key = DeSerialize.unpack_helper(string)
      value = DeSerialize.unpack_helper(string)
      if (key is not None and value is not None):
        result[key]=value
    return PolDictionary(result)
    
  def pack(self, string):
    string += 'd{}:{}'.format(len(self._value.keys()),
      ''.join(
        '{}{}'.format(k.pack(''),v.pack('')) 
          for k,v in self._value.items()))
    return string
    
# struct pol type
class PolStruct (PolObject):
  def __init__(self, value):
    PolObject.__init__(self, value)
  def _setvalue(self, value):
    if (isinstance(value,dict)):
      self._value = value
    else:
      self._value = {}
    return self._value
    
  @property
  def real(self):
    return { str(k): v.real for k,v in self._value.items() }
      
  def __repr__(self):
    return self.__str__()
  def __str__(self):
    r="Struct {"
    for k,v in self._value.items():
      r+="{}:{}, ".format(k,v)
    return r.rstrip(', ')+"}"
    
  @staticmethod
  def unpack(string):
    cnt = int(string.splice(string.find(':')))
    result = {}
    while (cnt):
      cnt-=1
      key = DeSerialize.unpack_helper(string)
      value = DeSerialize.unpack_helper(string)
      if (key is not None and value is not None):
        result[str(key.value)]=value
    return PolStruct(result)
  
  def pack(self, string):
    string += 't{}:{}'.format(len(self._value.keys()),
      ''.join(
        '{}{}'.format(PolString(k).pack(''),v.pack('')) 
          for k,v in self._value.items()))
    return string
 
# custom exception
class UnpackException (Exception):
  pass

# unpacker class
class DeSerialize(object):
  # initialize with string wrapper class or with string
  def __init__(self, string):
    if isinstance(string,StrWrapper):
      self._string = string
    else:
      self._string = StrWrapper(string.rstrip())
    
  # start point, unpack the string
  def unpack(self):
    if (not self._string):
      return PolUnInit(None)
    try:
      res = self.unpack_helper(self._string)
      if (res is not None):
        if (not self._string):
          return res
        else:
          return res # cprops can be broken if n00p defines them in a itemdesc..
          #print("Remaining chars in line: "+str(self._string))
          #return PolUnInit(None)
    except Exception as e:
      traceback.print_exc()
      raise UnpackException("Something went wrong "+str(e)+" "+str(self._string))
    raise UnpackException("nothing happend? "+str(res)+" "+str(self._string))
    
  # call unpacker based on first char
  @staticmethod
  def unpack_helper(string):
    if (not string):
      return None
    typestr = string.next
    if (typestr =='s'):
      res = PolString.unpack(string,False)
    elif (typestr == 'S'):
      res = PolString.unpack(string,True)
    elif (typestr == 'i'):
      res = PolInt.unpack(string)
    elif (typestr == 'r'):
      res = PolDouble.unpack(string)
    elif (typestr == 'u'):
      res = PolUnInit.unpack(string)
    elif (typestr == 'a'):
      res = PolArray.unpack(string)
    elif (typestr == 'd'):
      res = PolDictionary.unpack(string)
    elif (typestr == 't'):
      res = PolStruct.unpack(string)     
    elif (typestr == 'e'):
      res = PolError.unpack(string)
    elif (typestr == 'x' or typestr == 'u'):
      return PolUnInit(None)
    else:
      return PolError("unknown type "+typestr)
    if (res is not None):
      return res

# Unpack a complete file
class DatafileReader:
  # file : file path to unpack
  # u_members : if set to None unpack every member
  #             list of strings : unpack only these members (case-sensitive)
  #             default: empty list -> no unpack
  # u_cprops :  if set to None unpack every cprop
  #             list of strings : unpack only these cprops (case-sensitive)
  #             default: empty list -> no unpack
  # objtypes :  if set to None yield all objtypes
  #             list of integers : only yield these entries 
  #             default: None -> yield all found entries
  # is_datafile: if set to True the given file is a 'real' datafile (ds folder)
  #              this means all entries are packed
  #              default: False -> given file is a 'normal' savefile (pcs.txt,...)
  # is_cfg : if set to True the given file is a configuration file (unspecified seperator)
  #          never unpack members
  #          default: False  
  def __init__(self,file, u_members=[], u_cprops=[],objtypes=None, is_datafile=False, is_cfg=False):
    self._file = file
    self._linenr=0
    self.unpackmembers=u_members
    self.unpackcprops=u_cprops
    self.objtypes=objtypes
    self.is_datafile = is_datafile
    self.is_cfg = is_cfg
  
  # open and yield entries
  # yields a tuple:
  # (entry type, entry name, dict of members)
  def __iter__(self):
    with codecs.open(self._file, 'r','ISO-8859-1') as f: 
      current=None
      skip = False
      for line in f:
        if self._linenr == 0: # q&d bom fix
          if len(line)>2:
            if (line[0]==chr((0xEF)) and
              line[1]==chr((0xBB)) and
              line[2]==chr((0xBF)) ):
              line = line[3:]
        line = StrWrapper(line.strip())
        self._linenr+=1
        #self._line=line
        if not line: # empty line
          continue
        p = line.peak        
        if p=='#' or p=='/': # comment line
          if p=='/':
            s = line.next
            if line.peak == '/':
              continue
            else:
              line.index = line.index -1
          else:
            continue
        # no current active entry create a new one
        if current is None:
          space = line.find(' ') 
          tab = line.find('\t')
          if space > -1 and tab > -1:
            etype = line.splice(space if space < tab else tab)
          elif space > -1 or tab > -1:
            etype = line.splice(space if space > -1 else tab)
          else:
            etype = line.move()
          name = line.move() if line else ''
          current=(etype.strip(),name.strip(),{"__order__":[]})
          continue
        elif p=='{':
          continue
        elif p=='}':
          if not skip: # is this entry marked as to be ignored
            yield current
          current = None
          skip = False
          continue
        if skip: # early out for skipped entries
          continue
          
        if self.is_datafile: # in case of datafiles the value key pair is seperated by ' '
          elem = line.splice(line.find(' '))
        # core bug atleast numpins has a space instead of a tab....
        else: #if self.is_cfg: # unspecified seperator
          elem = self.splice_line(line)

        if self.is_datafile: # its a datafile thus all members can be unpacked like a cprop
          if self.unpackmembers is None or elem in self.unpackmembers:
            current[2][elem] = DeSerialize(line).unpack()
          else:
            current[2][elem] = line.move()
          current[2]["__order__"].append(elem) #store order
        elif self.is_cfg: # its a cfg never unpack
          if elem.lower() == 'cprop': # fix missspell
            elem = 'CProp'
            if 'CProp' not in current[2]: # initialize sub dict
              current[2]['CProp']={"__order__":[]}
              current[2]["__order__"].append('CProp') #store order
            ckey = self.splice_cfg_cprop_line(line)
            current[2]['CProp']["__order__"].append(ckey) #store order
            current[2]['CProp'][ckey] = line.move().strip()
          else:
            if elem not in current[2]: # since multiple entries can exist always use a list
              current[2][elem] = []
            current[2][elem].append(line.move().strip())
            if elem not in current[2]["__order__"]:
              current[2]["__order__"].append(elem) #store order
          
        # 'normal' data file non cprop member
        elif elem != 'CProp':
          must_append = elem in current[2]
          if not must_append:
            current[2]["__order__"].append(elem) #store order
          else:
            if not isinstance(current[2][elem], list):
              current[2][elem]=[current[2][elem]]
          if self.objtypes is not None: # should entries be skipped
            if elem == 'ObjType': # usually the ~ second entry
              u=self.ishex(line)
              if u is not None:
                if u not in self.objtypes:
                  skip = True # not found in the given obtypes -> mark as skip
                  continue
                else:
                  current[2][elem]=u
                  continue
          # name and desc members can look like int/floats thus ignore them
          if elem not in ['Name','Desc'] and (self.unpackmembers is None or elem in self.unpackmembers):
            # guess which type it is
            u=self.isinteger(line)
            
            if u is not None:
              if must_append:
                current[2][elem].append(u)
              else:
                current[2][elem]=u
              continue
            u=self.ishex(line)
            if u is not None:
              if must_append:
                current[2][elem].append(u)
              else:
                current[2][elem]=u
              continue
            u=self.isfloat(line)
            if u is not None:
              if must_append:
                current[2][elem].append(u)
              else:
                current[2][elem]=u
              continue
          if must_append:
            current[2][elem].append(line.move())
          else:
            current[2][elem]=line.move()
        # cprop entry
        else:
          if 'CProp' not in current[2]: # initialize sub dict
            current[2]['CProp']={"__order__":[]}
            current[2]["__order__"].append('CProp') #store order
          cprop = line.splice(line.find(' ')) # key,value of cprops is seperated by ' '
          current[2]['CProp']["__order__"].append(cprop) #store order
          if self.unpackcprops is None or cprop in self.unpackcprops: # should it be unpacked
            current[2]['CProp'][cprop] = DeSerialize(line).unpack()
          else:
            current[2]['CProp'][cprop] = line.move()

  # helperfunction to split key value pairs
  @staticmethod
  def splice_line(line):
    space = line.find(' ') 
    tab = line.find('\t')
    if space > -1 and tab > -1:
      elem = line.splice(space if space < tab else tab)
    else:
      elem = line.splice(space if space > -1 else tab)
    elem = elem.strip()
    return elem
    
  # helperfunction to split cprop key value pairs in cfgs
  @staticmethod
  def splice_cfg_cprop_line(line):
    if not line:
      return ''
    space = line.find(' ') 
    tab = line.find('\t')
    if space > -1 and tab > -1:
      if space < tab:
        while line[space+1] == ' ': # find last of
          space+=1
        elem = line.splice(space)
      else:
        while line[tab+1] == '\t':
          tab+=1
        elem = line.splice(tab)
    else:
      if space > -1:
        while line[space+1] == ' ':
          space+=1
        elem = line.splice(space)
      else:
        while line[tab+1] == '\t':
          tab+=1
        elem = line.splice(tab)
    
    elem = elem.strip()
    if not len(elem): # still empty
      return DatafileReader.splice_cfg_cprop_line(line)
    return elem
    
  # helper method to guess entry type (integer)
  @staticmethod
  def isinteger(string):
    if string:
      i = string.index
      s=""
      if string.peak == '-':
        s=string.next
      while string:
        if string.peak.isdigit():
          s+=string.next
        else:
          s=''
          break
      if len(s):
        return int(s)
      else:
        string.index = i
    return None

  # helper method to guess entry type (float)
  @staticmethod
  def isfloat(string):
    if string:
      i = string.index
      s=""
      if string.peak in ['+','-']:
        s=string.next
      while string:
        c = string.next
        if c.isdigit() or c =='.':
          s+=c
          continue
        if c=='e':
          if string:
            if string.peak in ['+','-']:
              s+= c+string.next
              continue
        break
      if len(s):
        try:
          return float(s)
        except:
          print (self._linenr, self._line,string)
      string.index = i
    return None

  # helper method to guess entry type (hex)
  @staticmethod
  def ishex(string):
    if string:
      if string.peak != '0':
        return None
      i = string.index
      s=string.next
      if string.peak != 'x':
        string.index=i
        return None
      s+=string.next

      while string:
        if string.peak.isdigit():
          s+=string.next
        elif str.lower(string.peak) in ['a','b','c','d','e','f']:
          s+=string.next
        else:
          break
      if len(s):
        return int(s,16)
      else:
        string.index = i
    return None
    
# packs given tuple into fileobject
def packOntoFile(file, element, pretty_format=False):
  
  def write_member(key,value,formatstr,adjustwidth):
    if adjustwidth:
      key = key.ljust(adjustwidth)
    if isinstance(value, list):
      for v in value:
        _w_member(key, v)
    elif not isinstance(value, PolObject):
      file.write(formatstr.format(key,value))
    else:
      file.write(formatstr.format(key,value.pack('')))
  def write_cprop(ckey,cvalue,formatstr,cpropkey, adjustwidth):
    if adjustwidth:
      ckey = ckey.ljust(adjustwidth)
    if not isinstance(cvalue, PolObject):
      file.write(formatstr.format(cpropkey,ckey,cvalue))
    else:
      file.write(formatstr.format(cpropkey,ckey,cvalue.pack('')))

  if not isinstance(element, tuple):
    return  
  
  if pretty_format:
    try:
      ladjust = len(max((key for key in element[2] if key != '__order__'), key = len ))
    except ValueError:
      ladjust = 0
    ladjustcprop = len(max((key for key in element[2].get('CProp',['']) if key != '__order__'), key = len ))
    _w_member = partial(write_member, formatstr='    {}  {}\n', adjustwidth = ladjust)
    _w_cprop = partial(write_cprop, formatstr= '    {}  {}  {}\n',cpropkey = 'CProp'.ljust(ladjust), adjustwidth = ladjustcprop)
  else:
    _w_member = partial(write_member, formatstr='\t{}\t{}\n',adjustwidth=0)
    _w_cprop = partial(write_cprop, formatstr=  '\t{}\t{} {}\n',cpropkey = 'CProp', adjustwidth=0)
   
  file.write('{} {}\n{{\n'.format(element[0],element[1]))
  for key in element[2]['__order__']: # write in read order
    if key != 'CProp':
      try:
        value = element[2][key]
      except KeyError:
        continue
      _w_member(key,value)
    else:
      for ckey in element[2]['CProp']['__order__']: # write in read order
        try:
          cvalue = element[2]['CProp'][ckey]
        except KeyError:
          continue
        _w_cprop(ckey,cvalue)
        
      for ckey, cvalue in element[2]['CProp'].items(): # find any added members
        if ckey == '__order__':
          continue
        if ckey not in element[2]['CProp']['__order__']:
          _w_cprop(ckey,cvalue)
          
  for key,value in element[2].items(): # find any added members
    if key == '__order__':
      continue
    if key not in element[2]['__order__']:
      if key != 'CProp':
        _w_member(key,value)
   
  file.write('}\n\n')
  
  
  

if __name__ == '__main__':
  r='a6:S9:RawDamageS7:Niemandi0i231582994a2:i79r-5S35:scripts/include_ai/throwelement.ecl'
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(isinstance(parse,PolObject))
  print(parse.pack(''))
  #sys.exit(0)
  r="i21"
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(parse.pack(''))
  
  r="a1:S9:RawDamage"
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(parse.pack(''))

  print("last damage")
  r="a6:S9:RawDamagea2:S23:Zentauren-Elitekaempferi1132i0i4793722a2:i99799i0S32:pkg/std/combat/mainhitscript.ecl"
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(parse.pack(''))
  
  print("castpower")
  r='d5:S22:castbonus_alchemie_alli0S24:castbonus_flask_of_blessi1S25:castbonus_flask_of_clerici1S64:castbonus_flask_of_error{ errortext = "invalid parameter type" }i0S19:castbonus_magic_alli0'
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(parse.pack(''))
  
  print("lastalchyrecipe")
  r='t5:S6:anzahli1S4:NameS3:intS10:Reagenziena3:i3974i3186i6813S9:Reagsnamea3:S7:AlrauneS14:FlaschenkurbisS11:BambusstockS5:Toolsd5:i3624i1i3739i1i6218i1i6225i1i6236i1'
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(parse.pack(''))
  print("incognito spell")
  r='a3:i4995527S39:Turley aus Vesper mit ganz viel drumrumi29490'
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(parse.pack(''))
  
  r='t2:S10:ActivTrapsd3:i1a0:i4a0:i5a0:S13:CatchedViechsd7:i1i17i3i5i6i4i7i7i8i6i9i6i10i2'
  print(r)
  parse=DeSerialize(r).unpack()
  print(parse.real)
  print(parse.pack(''))
  
  sys.exit(0)
  pol = DatafileReader('pcs.txt')
  eremit=None
  for ele in pol:
    if ele[0]=='Character' and ele[2]['Name']=='Eremit':
      eremit=ele
    pprint(ele)  
  pprint(eremit)



'''
a6:
  S9:RawDamage
  a2:
    S23:Zentauren-Elitekaempfer
    i1132
  i0
  i4793722
  a2:
    i99799
    i0
  S32:pkg/std/combat/mainhitscript.ecl
'''
