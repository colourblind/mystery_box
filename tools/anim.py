import os
import camera_path

FPS = 12

points = [(3.5, 2.65, 4.3), (3.5, 2.65, 3.8), (3.5, 2.75, 3.5)]
#directions = [(0, 0, 1), (0, 0, 1), (0, 0.5, 0)]
directions = [(0, 0, 1), (0, 0, 1), (-3.5, -2.25, -3.5)]
times = [0, 1.5, 3]

def go():
    track = camera_path.Pather(points, directions, times)
    t = 0
    for i in xrange(int(times[-1] * FPS)):
        t = float(i) / FPS
        camera = track.get(t)
        camera_pos = '{0} {1} {2}'.format(camera[0][0], camera[0][1], camera[0][2])
        camera_target = '{0} {1} {2}'.format(camera[1][0] + camera[0][0], camera[1][1] + camera[0][1], camera[1][2] + camera[0][2])
        f = open('go.cfg', 'w')
        f.write(template.format(camera_pos, camera_target, i))
        f.flush()
        f.close
        os.system('ray_march go.cfg')
    
template = """scale: 2.5
camera_pos: {0}
camera_target: {1}
light_pos: 9 3 2
output_file: anim2\{2}.png
width: 320
height: 240
"""

if __name__ == '__main__':
    go()
