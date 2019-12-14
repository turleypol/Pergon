# -*- coding: utf-8 -*-

def sanitize(name):
  '''
  performs a xml sanitize (top3)
  '''
  return str(name).replace('"','&quot;').replace('<','&lt;').replace('>','&gt;')