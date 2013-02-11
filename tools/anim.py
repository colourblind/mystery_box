import os
import camera_path

FPS = 12

points = [(3.5, 2.65, 4.32), (3.5, 2.65, 3.8), (3.5, 2.75, 3.5), (3.0, 2.4, 3.0), (0, 2.8, 3.8), (-4.0, 2.8, 3.8), (-6, 3, 6), (10, 4, 10)]
directions = [(3.5, 2.65, 6), (3.5, 2.65, 6), (0, 0.5, 0), (0, 0.5, 0), (-1, 2.4, 4.0), (-6, 2.4, 3.8), (0, 0, 0), (0, -0.75, 0)]
times = [0, 2, 4, 6, 8, 10, 12, 16]
output_dir = 'anim'

def go():
    track = camera_path.Pather(points, directions, times)
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)
    t = 0
    for i in xrange(int(times[-1] * FPS)):
        t = float(i) / FPS
        camera = track.get(t)
        camera_pos = '{0} {1} {2}'.format(camera[0][0], camera[0][1], camera[0][2])
        camera_target = '{0} {1} {2}'.format(camera[1][0], camera[1][1], camera[1][2])
        filename = os.path.join(output_dir, str(i))
        f = open('go.cfg', 'w')
        f.write(template.format(camera_pos, camera_target, filename))
        f.flush()
        f.close
        os.system('ray_march go.cfg')
    
template = """scale: 2.5
camera_pos: {0}
camera_target: {1}
light_pos: 9 3 2
output_file: {2}.png
width: 320
height: 240
"""

if __name__ == '__main__':
    go()
