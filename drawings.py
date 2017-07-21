def line(ca):
  return [ ca(*i) for i in [ (80, y) for y in range(130, 30, -5) ] ]

def genseq(a):
  return [(math.radians(i[0]), math.radians(i[1])) for i in [(a, -2*a), (-2*a, 4*a), (a, -2*a)]]