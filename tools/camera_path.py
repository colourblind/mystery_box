import math

class Vector(object):
    def __init__(self, v):
        self.x = float(v[0])
        self.y = float(v[1])
        self.z = float(v[2])
        
    def __add__(self, v):
        return Vector((self.x + v.x, self.y + v.y, self.z + v.z))
        
    def __sub__(self, v):
        return Vector((self.x - v.x, self.y - v.y, self.z - v.z))
        
    def __mul__(self, s):
        return Vector((self.x * s, self.y * s, self.z * s))
        
    def __div__(self, s):
        return Vector((self.x / s, self.y / s, self.z / s))
        
    def normalised(self):
        s = math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
        return self / s
        
    def get_tuple(self):
        return (self.x, self.y, self.z)
        
    def __str__(self):
        return '({0}, {1}, {2})'.format(self.x, self.y, self.z)
        

class Pather(object):
    def __init__(self, key_frames):
        self.points = [Vector(f['position']) for f in key_frames]
        self.directions = [Vector(f['target']) for f in key_frames]
        self.times = [f['time'] for f in key_frames]
        #self._interpolate = self._linear
        self._interpolate = self._cubic

    def get(self, time):
        i = 0
        # Find segment
        while i < len(self.times) and self.times[i] <= time:
            i = i + 1
        
        if i == len(self.times):
            segment_time = 1
            i = i - 1
        else:
            segment_time = float(time - self.times[i - 1]) / (self.times[i] - self.times[i - 1])
        
        p = self._interpolate(self.points, i, segment_time)
        d = self._interpolate(self.directions, i, segment_time) #.normalised()
            
        return (p.get_tuple(), d.get_tuple())
        
    def _linear(self, list, i, a):
        return list[self._clamp(i - 1)] + (list[self._clamp(i)] - list[self._clamp(i - 1)]) * a
        
    def _cubic(self, list, i, a):
        # http://paulbourke.net/miscellaneous/interpolation/
        #print('{0} {1}'.format(i, a))
        p1 = list[i - 1]
        p2 = list[i]
        
        if i - 2 < 0:
            p0 = p1 # - (p2 - p1)
        else:
            p0 = list[i - 2]
        
        if i + 1 >= len(list):
            p3 = p2 # + (p2 - p1)
        else:
            p3 = list[i + 1]
        
        a2 = a * a
        
        # Cubic
        #n0 = p3 - p2 - p0 + p1
        #n1 = p0 - p1 - n0
        #n2 = p2 - p0
        #n3 = p1
        
        # Catmull-Rom
        n0 = p0 * -0.5 + p1 * 1.5 - p2 * 1.5+ p3 * 0.5;
        n1 = p0 - p1 * 2.5 + p2 * 2 - p3 * 0.5;
        n2 = p0 * -0.5 + p2 * 0.5;
        n3 = p1;

        return n0 * a * a2 + n1 * a2 + n2 * a + n3;
        
