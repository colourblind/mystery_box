import os
import sys
import json
import camera_path

def go(c):
    track = camera_path.Pather(c['key_frames'])
    execute_line = os.path.join('.', 'ray_march') + ' go.cfg'
    if not os.path.exists(c['output_dir']):
        os.mkdir(c['output_dir'])
    t = 0
    for i in xrange(int(c['key_frames'][-1]['time'] * c['fps'])):
        t = float(i) / c['fps']
        camera = track.get(t)
        camera_pos = '{0} {1} {2}'.format(camera[0][0], camera[0][1], camera[0][2])
        camera_target = '{0} {1} {2}'.format(camera[1][0], camera[1][1], camera[1][2])
        filename = os.path.join(c['output_dir'], str(i))
        f = open('go.cfg', 'w')
        f.write(template.format(camera_pos, camera_target, filename))
        f.flush()
        f.close
        os.system(execute_line)
    
template = """scale: 2.5
camera_pos: {0}
camera_target: {1}
light_pos: 9 3 2
output_file: {2}.png
width: 320
height: 240
bailout: 8
"""

if __name__ == '__main__':
    f = open(sys.argv[1], 'r')
    config = json.load(f)
    f.close()
    go(config)
